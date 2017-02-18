#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cassert>
#include <getopt.h>

using namespace std;

static bool verbose = false;

struct memory {
  virtual void read(intptr_t addr, int size) = 0;
  virtual void write(intptr_t addr, int size) = 0;
};

struct dram : public memory {
  struct statistics {
    int reads;
    int writes;

    statistics() : reads(0), writes(0) {}

    friend ostream& operator<<(ostream &os, const statistics &s);
  };

  statistics stats;

  void read(intptr_t addr, int size) { stats.reads++; }
  void write(intptr_t addr, int size) { stats.writes++; }
};

ostream& operator<<(ostream &os, const dram::statistics &s) {
  os << " - reads  : " << s.reads << endl;
  os << " - writes : " << s.writes << endl;
  return os;
}

enum replace_policy { REPLACE_RANDOM = 1, REPLACE_LRU };

struct cache : public memory {
  struct configuration {
    int size;
    int block;
    int associativity;

    replace_policy replacement;
    bool write_allocate;
    bool write_back;

    configuration() :
      size(256), block(16), associativity(1), replacement(REPLACE_RANDOM),
      write_allocate(false), write_back(false) {}

    friend ostream& operator<<(ostream &os, const configuration &c);
  };

  struct statistics {
    int read_hit;
    int read_miss;
    int write_hit;
    int write_miss;

    statistics() : 
      read_hit(0), read_miss(0), write_hit(0), write_miss(0) {}

    friend ostream& operator<<(ostream &os, const statistics &s);
  };

  struct line {
    intptr_t tag;
    int set_ord;
    bool valid;
    bool dirty;

    line() : tag(0), set_ord(0), valid(0), dirty(0) {}

    friend ostream& operator<<(ostream &os, const line &l);
  };
  
  struct set {
    line *lines;
    int assoc;

    
    set() : lines(0), assoc(0) {}
    ~set() { delete[] lines; }
    
    // Inicjacja zbioru liczności size
    void init(int size) { 
        lines = new line[size];
        assoc = size;
        for(int i = 0; i < assoc; ++i){
            lines[i].set_ord = i;
        }
    }
    
    int choose_victim(replace_policy r){
        if (r == REPLACE_RANDOM)
            return rand() % assoc;
        int tab[assoc];
        for(int i = 0; i < assoc; ++i){
            tab[i] = i;
        }
        for(int i = 2; i <= assoc; i*= 2 ){
            // Zapewne ta pętla także byłaby zrównoleglona w sprzęcie
            for(int j = 0; j < assoc - i; j += i){
                if (lines[tab[j]].set_ord > lines[tab[j + i/2]].set_ord){
                    lines[tab[j+ i/2]].set_ord++;
                } else {
                    lines[tab[j]].set_ord++;
                    tab[j] = tab[j + i/2];
                } 
            }
        }
        
        lines[tab[0]].set_ord = 0;
        return tab[0];
    }
    
    // Aktualizuje znaczniki kolejności dostępu do linii w zbiorze
    // i - wartość znacznika do którego dokonano ostatniego dostępu
    void update(int i){
        // Pętla zrównoleglona w sprzęcie
        for(int j = 0; j < assoc; ++j){
            if (i == lines[j].set_ord) lines[j].set_ord = 0;
            if (j < lines[j].set_ord) lines[j].set_ord++;
        }
    }
    friend ostream& operator<<(ostream &os, const set &s);
  };

  bool present;
  string id;
  configuration conf;
  statistics stats;
  memory &backend;

  set *sets;
  intptr_t tag_mask;
  intptr_t index_mask;
  intptr_t offset_mask;
  int index_shift;

  cache(string id, memory &backend) :
    present(false), id(" " + id + ": "), backend(backend), sets(0) {}
  ~cache();

  bool init();
  void read(intptr_t addr, int size);
  void write(intptr_t addr, int size);

  friend ostream& operator<<(ostream &os, const cache &cache);
};

ostream& operator<<(ostream &os, const cache::line &l) {
  os << setw(12) << right << hex << l.tag << dec
     << " v:" << (int)l.valid << " d:" << (int)l.dirty;
  return os;
}

ostream& operator<<(ostream &os, const cache::statistics &s) {
  os << " - read hit   : " << s.read_hit << endl;
  os << " - read miss  : " << s.read_miss << endl;
  os << " - write hit  : " << s.write_hit << endl;
  os << " - write miss : " << s.write_miss << endl;
  return os;
}

ostream& operator<<(ostream &os, const cache::configuration &c) {
  os << " - block size    : " << c.block << endl;
  os << " - associativity : " << c.associativity << endl;
  os << " - lines         : " << c.size << endl;
  return os;
}

cache::~cache() {
  if (sets)
    delete[] sets;
}

bool cache::init() {
  if (conf.size <= 0 or conf.size & (conf.size - 1))
    return false;
  if (conf.block <= 0 or conf.block & (conf.block - 1))
    return false;
  if (conf.associativity <= 0 or conf.associativity > 16)
    return false;
  if (conf.size % conf.associativity)
    return false;
    
  int no_of_sets = conf.size / conf.associativity;
  sets = new cache::set[no_of_sets];
  for(int i = 0; i < no_of_sets; ++i){
      sets[i].init(conf.associativity);
  }

  offset_mask = conf.block - 1;
  index_mask = (no_of_sets * conf.block - 1) & ~(offset_mask);
  index_shift = __builtin_ffs(conf.block) - 1;
  tag_mask = ~(no_of_sets * conf.block - 1);
  

  return true;
}

/* Implement cache read algorithm here! */
void cache::read(intptr_t addr, int size) {
  int index = (addr & index_mask) >> index_shift;
  intptr_t tag = addr & tag_mask;
  int offset = (addr & offset_mask);
  bool found = false;
  cache::set &s = sets[index];
  
  if(offset + size > conf.block){
    read((addr & (~offset_mask)) + conf.block, ( offset + size) % conf.block);
    size = conf.block - offset;
  }

  if (present) {
    // Ta pętla byłaby zrównoleglona w sprzęcie
    for(int i = 0; i < conf.associativity; ++i){
        if(s.lines[i].valid and s.lines[i].tag == tag){
            //read hit
            stats.read_hit++;
            if (verbose)
                cout << id << "hit " << index << ":" << i;
            s.update(i);
            found = true;
            break;
            }
        }
        if(!found){
            //read miss
            stats.read_miss++;
            if (verbose)
                cout << id << "miss";
            int victim = s.choose_victim(REPLACE_LRU);
            backend.read(addr,size);
            s.lines[victim].tag = tag;
            s.lines[victim].valid = true;
            s.lines[victim].dirty = false;
        }
    }
    else {
    backend.read(addr, size);
  }
}

/* Implement cache write algorithm here! */
void cache::write(intptr_t addr, int size) {
  int index = (addr & index_mask) >> index_shift;
  intptr_t tag = addr & tag_mask;
  int offset = (addr & offset_mask);
  bool found = false;
  cache::set &s = sets[index];
  
  if(offset + size > conf.block){
    write((addr & (~offset_mask)) + conf.block, ( offset + size) % conf.block);
    size = conf.block - offset;
  }

  if (present) {
    // Ta pętla byłaby zrównoleglona w sprzęcie
    for(int i = 0; i < conf.associativity; ++i){
        if(s.lines[i].valid and s.lines[i].tag == tag){
            //write hit
            stats.write_hit++;
            if (verbose)
                cout << id << "hit " << index << ":" << i;
            s.update(i);
            if(!conf.write_back){
                backend.write(addr, size);
            }
            s.lines[i].dirty = true;
            found = true;
            break;
            }
        }
        if(!found){
            //write miss
            stats.write_miss++;
            if (verbose)
                cout << id << "miss";
            int victim = s.choose_victim(REPLACE_LRU);
            backend.write(addr,size);
            if(conf.write_allocate){
                s.lines[victim].tag = tag;
                s.lines[victim].valid = true;
                s.lines[victim].dirty = false;    
            }
        }
    }
    else {
    backend.write(addr, size);
  }
}

#define VERBOSE       0x01
#define L1_PRESENT    0x10
#define L1_BLOCK      0x11
#define L1_SIZE       0x12
#define L1_W_BACK     0x13
#define L1_W_THROUGH  0x14
#define L1_ASSOC      0x15
#define L1_W_ALLOC    0x16
#define L1_W_NO_ALLOC 0x17
#define L2_PRESENT    0x20
#define L2_BLOCK      0x21
#define L2_SIZE       0x22
#define L2_W_BACK     0x23
#define L2_W_THROUGH  0x24
#define L2_ASSOC      0x25
#define L2_W_ALLOC    0x26
#define L2_W_NO_ALLOC 0x27

static struct option options[] = {
  {"verbose", no_argument, 0, VERBOSE},
  {"l1-block", required_argument, 0, L1_BLOCK},
  {"l1-size", required_argument, 0, L1_SIZE},
  {"l1-write-back", no_argument, 0, L1_W_BACK},
  {"l1-write-through", no_argument, 0, L1_W_THROUGH},
  {"l1-associativity", required_argument, 0, L1_ASSOC},
  {"l1-write-allocate", no_argument, 0, L1_W_ALLOC},
  {"l1-write-no-allocate", no_argument, 0, L1_W_NO_ALLOC},
  {"l2-present", no_argument, 0, L2_PRESENT},
  {"l2-block", required_argument, 0, L2_BLOCK},
  {"l2-size", required_argument, 0, L2_SIZE},
  {"l2-write-back", no_argument, 0, L2_W_BACK},
  {"l2-write-through", no_argument, 0, L2_W_THROUGH},
  {"l2-associativity", required_argument, 0, L2_ASSOC},
  {"l2-write-allocate", no_argument, 0, L2_W_ALLOC},
  {"l2-write-no-allocate", no_argument, 0, L2_W_NO_ALLOC},
  {NULL, 0, NULL, 0}
};

int main(int argc, char **argv) {
  int c, index = 0;

  dram mem;
  cache l2("L2", mem);
  cache l1("L1", l2);

  l1.present = true;

  while ((c = getopt_long(argc, argv, "", options, &index)) != -1) {
    if (c == VERBOSE)
      verbose = true;
    else if (c == L1_BLOCK)
      l1.conf.block = stoi(optarg);
    else if (c == L1_SIZE)
      l1.conf.size = stoi(optarg);
    else if (c == L1_W_BACK)
      l1.conf.write_back = true;
    else if (c == L1_W_THROUGH)
      l1.conf.write_back = false; 
    else if (c == L1_ASSOC)
      l1.conf.associativity = stoi(optarg);
    else if (c == L1_W_ALLOC)
      l1.conf.write_allocate = true;
    else if (c == L1_W_NO_ALLOC)
      l1.conf.write_allocate = false;
    else if (c == L2_PRESENT)
      l2.present = true;
    else if (c == L2_BLOCK)
      l2.conf.block = stoi(optarg);
    else if (c == L2_SIZE)
      l2.conf.size = stoi(optarg);
    else if (c == L2_W_BACK)
      l2.conf.write_back = true;
    else if (c == L2_W_THROUGH)
      l2.conf.write_back = false;
    else if (c == L2_W_ALLOC)
      l2.conf.write_allocate = true;
    else if (c == L2_W_NO_ALLOC)
      l2.conf.write_allocate = false;
    else if (c == L2_ASSOC)
      l2.conf.associativity = stoi(optarg);
    else
      cerr << "error: getopt returned character code: " << hex << c << endl;

    if (c & L2_PRESENT)
      l2.present = true;
  }

  if (l1.init()) {
    cerr << "L1 cache configuration:" << endl << l1.conf;
  } else {
    cerr << "error: incomplete or malformed L1 cache configuration" << endl;
    exit(1);
  }

  if (l2.present) {
    if (l2.init()) {
      cerr << "L2 cache configuration:" << endl << l2.conf;
    } else {
      cerr << "error: incomplete or malformed L2 cache configuration" << endl;
      exit(1);
    }
  }

  while (!cin.eof()) {
    char op;
    intptr_t addr;
    int size;
    
    cin >> op >> hex >> addr >> size;

    if (verbose)
      cout << op << " " << hex << addr << " " << dec << size;

    if (op == 'r')
      l1.read(addr, size);
    else
      l1.write(addr, size);

    if (verbose)
      cout << endl;
  }

  cout << "L1 cache statistics:" << endl << l1.stats;

  if (l2.present)
    cout << "L2 cache statistics:" << endl << l2.stats;

  cout << "Memory statistics:" << endl;
  cout << mem.stats;

  return 0;
}

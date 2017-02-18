/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2015 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */

// ManualExamples/buffer_linux.cpp modified by Krystian Bacławski.

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#include "pin.H"
#include "portability.H"
using namespace std;

// Name of the output file
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "pctrace.out", "output file");

// The ID of the buffer
BUFFER_ID bufId;

// Thread specific data
TLS_KEY mlog_key;

// Number of OS pages for the buffer
#define NUM_BUF_PAGES 1024

// Record of program counter change.
#define PC_CALL   'c'
#define PC_ICALL  'C'
#define PC_RET    'r'
#define PC_BRANCH 'b'
#define PC_JUMP   'j'
#define PC_IJUMP  'J'

struct PCEVENT {
  ADDRINT pc;
  ADDRINT target;
  ADDRINT retaddr;
  UINT32  type;
  BOOL    taken;
};

// MLOG - thread specific data that is not handled by the buffering API.
class MLOG {
  private:
    ofstream _ofile;

  public:
    MLOG(THREADID tid) {
      string filename = KnobOutputFile.Value() + "." + decstr(getpid_portable()) + "." + decstr(tid);

      _ofile.open(filename.c_str());

      if (!_ofile) {
        cerr << "Error: could not open output file." << endl;
        exit(1);
      }

      _ofile << hex;
    }

    ~MLOG() {
      _ofile.close();
    }

    VOID DumpBufferToFile(struct PCEVENT *event, UINT64 numElements,
                          THREADID tid)
    {
      for (UINT64 i = 0; i < numElements; i++, event++) {
        if (event->type) {
          _ofile << event->pc
                 << " " << (char)event->type
                 << " " << event->target;

          if (event->type == 'b')
            _ofile << " " << event->taken << endl;
          else if (event->type == 'c' or event->type == 'C')
            _ofile << " " << event->retaddr << endl;
          else
            _ofile << endl;
        }
      }
    }
};

// Insert code to write data to a thread-specific buffer for instructions that
// change program counter.
VOID Instruction(INS ins, VOID *v) {
  if (INS_IsRet(ins)) {
    /* Return from subroutine instruction */
    INS_InsertFillBufferPredicated(ins, IPOINT_BEFORE, bufId,
        IARG_INST_PTR, offsetof(struct PCEVENT, pc),
        IARG_BRANCH_TARGET_ADDR, offsetof(struct PCEVENT, target),
        IARG_UINT32, PC_RET, offsetof(struct PCEVENT, type),
        IARG_BOOL, true, offsetof(struct PCEVENT, taken),
        IARG_END);
  } else if (INS_IsBranchOrCall(ins)) {
    if (INS_HasFallThrough(ins)) {
      /* Conditional branch instruction */
      INS_InsertFillBufferPredicated(ins, IPOINT_BEFORE, bufId,
          IARG_INST_PTR, offsetof(struct PCEVENT, pc),
          IARG_BRANCH_TARGET_ADDR, offsetof(struct PCEVENT, target),
          IARG_UINT32, PC_BRANCH, offsetof(struct PCEVENT, type),
          IARG_BRANCH_TAKEN, offsetof(struct PCEVENT, taken),
          IARG_END);
    } else {
      /* Direct / indirect call or jump instruction */
      UINT32 type = INS_IsCall(ins) ? 'c' : 'j';

      if (INS_IsIndirectBranchOrCall(ins))
        type = toupper(type);

      if (INS_IsCall(ins)) {
        INS_InsertFillBufferPredicated(ins, IPOINT_BEFORE, bufId,
            IARG_INST_PTR, offsetof(struct PCEVENT, pc),
            IARG_BRANCH_TARGET_ADDR, offsetof(struct PCEVENT, target),
            IARG_ADDRINT, INS_NextAddress(ins), offsetof(struct PCEVENT, retaddr),
            IARG_UINT32, type, offsetof(struct PCEVENT, type),
            IARG_BOOL, true, offsetof(struct PCEVENT, taken),
            IARG_END);
      } else {
        INS_InsertFillBufferPredicated(ins, IPOINT_BEFORE, bufId,
            IARG_INST_PTR, offsetof(struct PCEVENT, pc),
            IARG_BRANCH_TARGET_ADDR, offsetof(struct PCEVENT, target),
            IARG_UINT32, type, offsetof(struct PCEVENT, type),
            IARG_BOOL, true, offsetof(struct PCEVENT, taken),
            IARG_END);
      }
    }
  }
}

// Called when a buffer fills up, or the thread exits, so we can process it or pass it off
// as we see fit.
// @param[in] id		buffer handle
// @param[in] tid		id of owning thread
// @param[in] ctxt		application context
// @param[in] buf		actual pointer to buffer
// @param[in] numElements	number of records
// @param[in] v			callback value
// @return  A pointer to the buffer to resume filling.
VOID *BufferFull(BUFFER_ID id, THREADID tid, const CONTEXT *ctxt, VOID *buf,
                 UINT64 numElements, VOID *v)
{
  struct PCEVENT *event = (struct PCEVENT *)buf;
  MLOG *mlog = static_cast<MLOG *>(PIN_GetThreadData(mlog_key, tid));
  mlog->DumpBufferToFile(event, numElements, tid);
  return buf;
}

// Note that opening a file in a callback is only supported on Linux systems.
VOID ThreadStart(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v) {
  // There is a new MLOG for every thread.  Opens the output file.
  MLOG * mlog = new MLOG(tid);
  // A thread will need to look up its MLOG, so save pointer in TLS
  PIN_SetThreadData(mlog_key, mlog, tid);
}

VOID ThreadFini(THREADID tid, const CONTEXT *ctxt, INT32 code, VOID *v) {
  MLOG * mlog = static_cast<MLOG*>(PIN_GetThreadData(mlog_key, tid));
  delete mlog;
  PIN_SetThreadData(mlog_key, 0, tid);
}

INT32 Usage() {
  cerr << "This tool dumps memory accesses to file." << endl;
  cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
  return -1;
}

int main(int argc, char *argv[]) {
  // Initialize PIN library. Print help message if -h(elp) is specified
  // in the command line or the command line is invalid
  if (PIN_Init(argc,argv))
    return Usage();

  // Initialize the memory reference buffer; set up the callback to process the
  // buffer.
  bufId = PIN_DefineTraceBuffer(sizeof(struct PCEVENT), NUM_BUF_PAGES,
                                BufferFull, 0);

  if (bufId == BUFFER_ID_INVALID) {
    cerr << "Error: could not allocate initial buffer" << endl;
    return 1;
  }

  // Initialize thread-specific data not handled by buffering api.
  mlog_key = PIN_CreateThreadDataKey(0);

  // add an instrumentation function
  INS_AddInstrumentFunction(Instruction, 0);

  // add callbacks
  PIN_AddThreadStartFunction(ThreadStart, 0);
  PIN_AddThreadFiniFunction(ThreadFini, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}


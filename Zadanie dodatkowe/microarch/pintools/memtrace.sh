#!/bin/sh

if [ ! -d "${PIN_ROOT}" ]; then
  echo "Please install Intel® Pin and set PIN_ROOT environment variable!"
  exit 1
fi

${PIN_ROOT}/pin -t obj-*/memtrace.so -- $@

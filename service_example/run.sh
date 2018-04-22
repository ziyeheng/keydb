#!/bin/bash

ulimit -c unlimited
nohup ./keydb ./conf/keydb.conf 1>/dev/null 2>&1 &

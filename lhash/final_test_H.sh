#!/bin/bash
echo "[testing modules] with 2 threads"
./runtest.sh "./testH 1" "open a hash then close it"
./runtest.sh "./testH 2" "all functions with a NULL h"
./runtest.sh "./testH 3" "put 3 elements"
./runtest.sh "./testH 4" "search in hash"
./runtest.sh "./testH 5" "search for something that's not in hash"
./runtest.sh "./testH 6" "remove from hash"
./runtest.sh "./testH 7" "remove something that's not in hash"
./runtest.sh "./testH 8" "hash apply"
echo "[done]"


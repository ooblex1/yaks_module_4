#!/bin/bash

if [ $# != 0 ]; then                                                             
    echo 'usage: hash'                                
    exit                                                                         
fi

echo "run valgrind on tests to check for errors or memory leaks"
valgrind --leak-check=full htest1
valgrind --leak-check=full htest2

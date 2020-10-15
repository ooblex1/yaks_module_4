#!/bin/bash

if [ $# != 0 ]; then                                                             
    echo 'usage: list'                                
    exit                                                                         
fi

echo "run valgrind on tests to check for errors or memory leaks"
valgrind --leak-check=full test1
valgrind --leak-check=full test2

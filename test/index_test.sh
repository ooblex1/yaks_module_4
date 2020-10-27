#!/bin/bash
valgrind --leak-check=full --show-leak-kinds=all ./index_test

diff ./oldindex ./newindex
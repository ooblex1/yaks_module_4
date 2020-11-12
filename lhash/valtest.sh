#!/bin/bash
echo [testing queue module]
valgrind --leak-check=full --show-leak-kinds=all ./testQ 1 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 2 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 3 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 4 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 5 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 6 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 7 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 8 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 9 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 10 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 11 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 12 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 13 
valgrind --leak-check=full --show-leak-kinds=all ./testQ 14 
valgrind --leak-check=full --show-leak-kinds=all ./testH 1 
valgrind --leak-check=full --show-leak-kinds=all ./testH 2 
valgrind --leak-check=full --show-leak-kinds=all ./testH 3 
valgrind --leak-check=full --show-leak-kinds=all ./testH 4 
valgrind --leak-check=full --show-leak-kinds=all ./testH 5 
valgrind --leak-check=full --show-leak-kinds=all ./testH 6 
valgrind --leak-check=full --show-leak-kinds=all ./testH 7 
valgrind --leak-check=full --show-leak-kinds=all ./testH 8 
echo [done]


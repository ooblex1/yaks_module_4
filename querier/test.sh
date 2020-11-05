# ENGS 50 Module 6 Testing Script F20
# Author Brandon Guzman


./query ./ # number of arg test 

./query ./ ../indexer/index_for_query # not valid crawler dir test

./query ./ file   # unreadable file test 
echo "error queries begin"
valgrind ./query ../crawler/pages ../indexer/index_for_query < error-queries   # prof's error tests
echo "error queries done"
valgrind query ../crawler/pages ../indexer/index_for_query -q good-queries.txt myoutput  # run quietly test with professor's queries 




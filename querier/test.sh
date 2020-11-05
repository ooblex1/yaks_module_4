# ENGS 50 Module 6 Testing Script F20
# Author Brandon Guzman


./query ./ # number of arg test 

./query ./ ../indexer/query_depth3 # not valid crawler dir test

./query ./ file   # unreadable file test 
echo "error queries begin"
valgrind ./query ../crawler/pages3 ../indexer/query_depth3 < error-queries   # prof's error tests
echo "error queries done"
valgrind query ../crawler/pages3 ../indexer/query_depth3 -q good-queries.txt myoutput  # run quietly test with professor's queries 




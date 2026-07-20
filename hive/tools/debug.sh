mkdir -p data/test data/hive
rm -rf data/test/*
gdb --tui --args ./Test data/test 

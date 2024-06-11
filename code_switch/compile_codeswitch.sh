
python3 $opdfi/code_switch/helper.py $1 /tmp/code_version_header.h

echo "compile code_switch:: $1"

clang++ -c  $opdfi/code_switch/code_switch.cpp -o $opdfi/code_switch/code_switch.o
clang++ -c -flto $opdfi/code_switch/code_switch.cpp -o $opdfi/code_switch/code_switch.bc
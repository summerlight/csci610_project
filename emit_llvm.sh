~/clang-llvm/build/bin/clang++ $1.cpp -gline-tables-only -S -emit-llvm -I"./range-v3/include" -I"./fmt/" -std=c++14
cp ./$1.ll ~/clang-llvm/
cd ~/clang-llvm/build/
ninja csci610
bin/opt -load lib/csci610.so -csci610 -S -o ../$1_mod.ll ../$1.ll
bin/llc -filetype=obj ../$1_mod.ll
cp ../$1_mod.o ~/csci610_project/$1.o
cd ~/csci610_project/
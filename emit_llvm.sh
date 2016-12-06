~/clang-llvm/build/bin/clang++ stack.cpp -gline-tables-only -S -emit-llvm -I"./range-v3/include" -I"./fmt/" -std=c++14
cp ./stack.ll ~/clang-llvm/
cd ~/clang-llvm/build/
ninja csci610
bin/opt -load lib/csci610.so -csci610 -S -o ../stack_mod.ll ../stack.ll
bin/llc -filetype=obj ../stack_mod.ll
cp ../stack_mod.o ~/csci610_project/stack.o
cd ~/csci610_project/
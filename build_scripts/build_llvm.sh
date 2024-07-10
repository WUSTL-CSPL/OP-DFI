cd ~/toolchain
git clone  https://github.com/llvm/llvm-project
cd llvm-project
git checkout 75e33f71c2dae584b13a7d1186ae0a038ba98838
git apply ~/opdfi/LLVM_patch/*

mkdir build
cd build
cmake -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_TARGETS_TO_BUILD="all" -DLLVM_ENABLE_LTO=ON ../llvm
make -j2
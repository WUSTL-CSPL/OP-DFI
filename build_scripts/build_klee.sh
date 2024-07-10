
# please first build llvm, and upgrade cmake to 3.28, then
cd ~/toolchain
git clone https://github.com/klee/klee
cd klee
git checkout fc83f06b17221bf5ef20e30d9da1ccff927beb17
git apply ~/opdfi/KLEE_patch/*

#build uClibc
cd ~/toolchain
git clone https://github.com/klee/klee-uclibc.git
cd klee-uclibc
./configure --make-llvm-lib
make -j2


#follow https://github.com/stp/stp to build STP.
cd ~/toolchain
git clone https://github.com/stp/stp
cd stp
git submodule init && git submodule update
./scripts/deps/setup-gtest.sh
./scripts/deps/setup-outputcheck.sh
./scripts/deps/setup-cms.sh
./scripts/deps/setup-minisat.sh
mkdir build
cd build
cmake ..
cmake --build .
export STP_DIR=/home/opdfi/toolchain/stp/build
export LD_LIBRARY_PATH="$STP_DIR/lib:$LD_LIBRARY_PATH"


#build libc++
cd ~/toolchain
LLVM_VERSION=13 BASE=/home/opdfi/toolchain/LIBCXX_DIR ENABLE_OPTIMIZED=1 DISABLE_ASSERTIONS=1 ENABLE_DEBUG=0 REQUIRES_RTTI=1 klee/scripts/build/build.sh libcxx

#finally, build klee
cd ~/toolchain/klee
mkdir build
cd build
cmake -DENABLE_SOLVER_STP=ON -DENABLE_POSIX_RUNTIME=ON -DKLEE_UCLIBC_PATH=/home/opdfi/toolchain/klee-uclibc \
 -DENABLE_UNIT_TESTS=ON  -DENABLE_KLEE_LIBCXX=ON \
  -DKLEE_LIBCXX_DIR=/home/opdfi/toolchain/LIBCXX_DIR/libc++-install-130 \
 -DKLEE_LIBCXX_INCLUDE_DIR=/home/opdfi/toolchain/LIBCXX_DIR/libc++-install-130/include/c++/v1 \
 -DENABLE_KLEE_EH_CXX=ON -DKLEE_LIBCXXABI_SRC_DIR=/home/opdfi/toolchain/LIBCXX_DIR/llvm-130/libcxxabi \
  .. 

make -j2
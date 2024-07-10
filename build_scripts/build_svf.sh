
cd ~/toolchain
git clone  https://github.com/SVF-tools/SVF
cd SVF
git checkout 06920202d216e003efcac1469fc78b12904cd2c6
git apply ~/opdfi/SVF_patch/*

source build.sh
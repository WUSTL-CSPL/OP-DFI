
# Opportunistic Security - OP-DFI

  

## Description

This repository contains the code related to the paper "Wang, Y., Li, A., Wang, J., Baruah, S., & Zhang, N. Opportunistic Data Flow Integrity for Real-time Cyber-physical Systems Using Worst Case Execution Time Reservation. In 33nd USENIX Security Symposium (USENIX Security 24)"

  

This repo provides a generalized framework for opportunistic protection, that is hardware independent and not restricted to DFI protection. Feel free to extend it to incorporate other security primitives and hardware.

  

## Table of Contents

- [Installation](#installation)

- [Usage](#usage)

- [License](#license)

- [Bibliography](#bibliography)

  
## Code Structure

```
📦 opdfi
├─ KLEE_patch # slack constraint collector
├─ LLVM_patch # code version generation, slack estimator and code switch instrumentation
├─ SVF_patch # data flow analysis
├─ code_switch 
│  └─ code_switch.cpp # code switch reference monitor
├─ slack_estimation
│  ├─ online_estimator.cpp # runtime slack estimation reference monitor
│  └─ prepare_formula.py # slack constraint selection and processing
├─ test/* # testing directory
└─ src_test/* #scripts used for testing
```
## Installation

#### Directory Setup
  - The running script assumes the  VM has the following file directory.
```

git clone https://github.com/WUSTL-CSPL/OP-DFI ~/opdfi

mkdir ~/toolchain && cd ~/toolchain

mkdir klee && mkdir SVF && mkdir llvm-project
```


-  Apply patches located in  ``~/opdfi/KLEE_patch``, ``~/opdfi/SVF_patch``, ``~/opdfi/LLVM_patch`` to KLEE (commit `fc83f06b17221bf5ef20e30d9da1ccff927beb17`), SVF (commit `06920202d216e003efcac1469fc78b12904cd2c6`) and LLVM (commit `75e33f71c2dae584b13a7d1186ae0a038ba98838`) respectively. Finally. compile klee, SVF and LLVM following guidance of the official repository.


#### Environment Setup
- After installation, setup the corresponding environmental variables by copying following command into ``~/.bashrc``.
```
export LLVM_DIR=~/toolchain/llvm-project/\
build
export KLEE_DIR=~/toolchain/klee/build
export SVF_DIR=~/toolchain/SVF
export KLEE_INC=$KLEE_DIR/../include
export PATH="$LLVM_DIR/bin:\
$KLEE_DIR/bin:$PATH"
export opdfi=/home/opdfi/opdfi
```

#### Basic Test
 - The successful setup of a KLEE environment can be verified with the following commands:
```
klee --version
```

- To check the installation of SVF, execute:
```
$SVF_DIR/Release-build/bin/wpa --version
```

- To check the installation of LLVM, execute:
```
clang -v
```

If the final outputs do not indicate any errors, it can be concluded that these dependencies are installed properly.



## Usage

  
#### Compilation

The following command will compile a demo on a sample program located in ``~/opdfi/src_test/crsf.cpp``.

  

```

cd ~/opdfi/test 
source ../compile.sh

```
The illustrations of these commands are as follows. The offline compilation can be conducted using the ``compile.sh`` script, which runs the klee executable on the sample program to generate path constraints. Then, the processed constraints are handled by ``prepare_formula.py`` to select the constraints to determine the relationship between the constraints and different levels of slack. In addition, the DFI protection level for different code versions is also determined as a part of the security policy.

Based on the security policy, the program code is first duplicated into multiple code versions using the LLVM passes located in ``\$LLVM\_DIR/../llvm/lib/Transforms/ code_switch_llvmpasses/code_version_generation/``. Next, each generated code version is instrumented with different levels of DFI protection using the SVF scripts located in ``$SVF_DIR/tools/OPDFI/dependency_analysis/``. Finally, slack estimator and code switch units  are inserted as reference monitors.
  
  The outputs of the compilation are located under the directory ``~/opdfi/test/compile\_results``, and consist of the final executable, the processed constraints for slack estimation, the generated code versions, and the security policy. The final executable is ``./execute_me``.
  
  #### Execution

```
 cd ~/opdfi/test 
qemu-aarch64 -L /usr/aarch64-linux-gnu/ ./compile_results/execute_me
```

The executable will run multiple iterations with the test inputs stored in ``test_inputs.txt``. During execution, the number of iterations, slack, and DFI protection coverage will be displayed. The runtime execution logs are stored in ``runtime_result.txt`` under the ``runtime_results/'' directory, where the first column is the estimated slack (in the form of a percentage of WCET), and the second column is the protection coverage. 



## License

MIT License, Apache License, etc.

  

## Bibliography

- Wang, Y., Li, A., Wang, J., Baruah, S., & Zhang, N. Opportunistic Data Flow Integrity for Real-time Cyber-physical Systems Using Worst Case Execution Time Reservation. In 33nd USENIX Security Symposium (USENIX Security 24).
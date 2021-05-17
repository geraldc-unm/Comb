#!/bin/bash

##############################################################################
## Copyright (c) 2018-2021, Lawrence Livermore National Security, LLC.
##
## Produced at the Lawrence Livermore National Laboratory
##
## LLNL-CODE-758885
##
## All rights reserved.
##
## This file is part of Comb.
##
## For details, see https://github.com/LLNL/Comb
## Please also see the LICENSE file for MIT license.
##############################################################################

COMPILER_SUFFIX=clang_9_0_0
BUILD_SUFFIX=lc_blueos_nvcc_10_1_${COMPILER_SUFFIX}

rm -rf build_${BUILD_SUFFIX} >/dev/null
mkdir build_${BUILD_SUFFIX} && cd build_${BUILD_SUFFIX}

module load cmake/3.9.2
module load cuda/10.1.168
module load clang/9.0.0

cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_OPENMP=ON \
  -DENABLE_CUDA=ON \
  -DCUDA_ARCH=sm_70 \
  -DCUDA_TOOLKIT_ROOT_DIR=/usr/tce/packages/cuda/cuda-10.1.168 \
  -C ../host-configs/lc-builds/blueos/nvcc_${COMPILER_SUFFIX}.cmake \
  -DCMAKE_INSTALL_PREFIX=../install_${BUILD_SUFFIX} \
  "$@" \
  ..

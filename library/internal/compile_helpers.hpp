//
// Created by Simon Cros on 1/6/25.
//

#ifndef COMPILE_HELPERS_HPP
#define COMPILE_HELPERS_HPP

#include <version>

// From https://github.com/NVIDIA/stdexec/pull/1003
// Based on https://github.com/llvm/llvm-project/issues/44178
#if defined(__cpp_lib_ranges) && (!defined(__clang__) || __clang_major__ >= 16 || defined(_LIBCPP_VERSION))
#define CAN_USE_RANGES
#endif

#endif //COMPILE_HELPERS_HPP

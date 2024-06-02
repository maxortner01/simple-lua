#pragma once

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

#define SL_ASSERT(expr, msg) if (!(expr)) { std::cout << "Assertion (" << #expr << ") failed, file " << std::filesystem::path(__FILE__).filename().string() << ", line " << __LINE__ << ": " << msg << "\n"; std::terminate(); }

#ifdef _MSC_VER
#   ifdef SL_BUILD
#       define SL_SYMBOL __declspec(dllexport)
#   else
#       define SL_SYMBOL __declspec(dllimport)
#   endif
#else
    #define SL_SYMBOL
#endif


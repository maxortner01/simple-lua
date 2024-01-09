#pragma once

#include <cstdlib>
#include <filesystem>

#define SL_ASSERT(expr, msg) if (!(expr)) { printf("Assertion (%s) failed, file %s, line %i: %s\n", #expr, std::filesystem::path(__FILE__).filename().c_str(), __LINE__, msg); std::terminate(); }
#define SL_ASSERT_ARGS(expr, msg, ...) if (!(expr)) { printf("Assertion (%s) failed, file %s, line %i: " msg "\n", #expr, std::filesystem::path(__FILE__).filename().c_str(), __LINE__, __VA_ARGS__); std::terminate(); }


#pragma once
// simple 1d arrays

#include <unordered_map>
typedef std::unordered_map<int64_t, std::string> iarray;
typedef std::unordered_map<std::string, std::string> aarray;

extern std::unordered_map<std::string, aarray> aarrays;
extern std::unordered_map<std::string, iarray> iarrays;

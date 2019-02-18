#ifndef UTILITY_H
#define UTILITY_H

#include <string>
using namespace std;

#include "./define.h"

bool validate_feagure_string(const string &);
uint32_t find_zeros_ahead(uint32_t);
string generate_zeros_str(uint32_t);
void generate_shifting_ele(uint32_t, uint32_t &, uint32_t &);

#endif // UTILITY_H

#ifndef DEFINE_H
#define DEFINE_H

//#define DEBUG

#define ZERO_9_APX             "000000000"
#define ZERO_8_APX             "00000000"
#define ZERO_7_APX             "0000000"
#define ZERO_6_APX             "000000"
#define ZERO_5_APX             "00000"
#define ZERO_4_APX             "0000"
#define ZERO_3_APX             "000"
#define ZERO_2_APX             "00"
#define ZERO_1_APX             "0"

#define BASE                        1000000
#define SECTION_LEN            6
#define SECTION_MAX           999999
#define SECTION_ZERO         ZERO_6_APX

#include <complex>
using complex_t = std::complex<double>;

#endif // DEFINE_H

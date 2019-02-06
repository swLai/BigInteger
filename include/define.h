#ifndef DEFINE_H
#define DEFINE_H


#include <complex>
using complex_t = std::complex<double>;
using uint32_t  = unsigned int;
using uint64_t  = unsigned long long;
using int32_t   = int;
using int64_t   = long long;


#define KARATSUBA               0
#define FFT                     1
#define MULTIPLY                FFT

#define ZERO_9_APX              "000000000"
#define ZERO_8_APX              "00000000"
#define ZERO_7_APX              "0000000"
#define ZERO_6_APX              "000000"
#define ZERO_5_APX              "00000"
#define ZERO_4_APX              "0000"
#define ZERO_3_APX              "000"
#define ZERO_2_APX              "00"
#define ZERO_1_APX              "0"

#if MULTIPLY == KARATSUBA
const uint32_t BASE             = 1000000000;
const uint32_t SECTION_LEN      = 9;
const uint32_t SECTION_MAX      = 999999999;
#define SECTION_ZERO            ZERO_9_APX
#else
const uint32_t BASE             = 1000000;
const uint64_t BASE_SQR         = 1000000000000;
const uint32_t SECTION_LEN      = 6;
const uint32_t SECTION_MAX      = 999999;
#define SECTION_ZERO            ZERO_6_APX
#endif // MULTIPLY


#endif // DEFINE_H

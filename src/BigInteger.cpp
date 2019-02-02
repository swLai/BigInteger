#include <iostream>
#include <sstream>
#include <vector>
#include <complex>
#include <cmath>

#include "../include/BigInteger.h"
#include "../include/define.h"
#include "../include/utility.h"

#ifndef ZERO
#define ZERO                 BigInteger(0)
#endif // ZERO

#ifndef ONE
#define ONE                  BigInteger(1)
#endif // ONE

#ifndef MIN_ONE
#define MIN_ONE              BigInteger(-1)
#endif // MIN_ONE

using namespace std;


/*
    Constructor
*/
BigInteger::BigInteger()
{
    // Initialize with 0
    set_word(0, 0);
    set_zeros_ahead(SECTION_LEN-1, 0);
    set_sign(false);
}

BigInteger::BigInteger(long long word)
{
    if (word)
    {
        if (word < 0)
            set_sign(true);
        else
            set_sign(false);

        unsigned i = 0;
        while (word)
        {
            unsigned word_u = abs(word) % BASE;
            set_word(word_u, i);
            set_zeros_ahead(find_zeros_ahead(word_u), i);
            word /= BASE;
            ++i;
        }
    }
    else
    {
        set_word(0, 0);
        set_zeros_ahead(SECTION_LEN-1, 0);
        set_sign(false);
    }
}

BigInteger::BigInteger(string init_str)
{
    if ( !validate_feagure_string(init_str) )
    {
        cout << "The value is not a complete integer number!\n" << endl;
        set_word(0, 0);
        set_zeros_ahead(SECTION_LEN-1, 0);
        set_sign(false);
        return;
    }

    set_sign(false);
    switch (init_str[0])
    {
    case '-':
        set_sign(true);
    case '+':
        init_str.erase(0, 1); // erase the sign character
        break;

    default:
        ;
    }

    int len = init_str.length();
    unsigned words = ceil(static_cast<double>(len) / SECTION_LEN);
    int shift = len;
    string feagures_str, feagure_sec;
    for (unsigned i = 0; i < words; ++i)
    {
        shift -= SECTION_LEN;
        if (shift < 0)
            feagure_sec = init_str.substr(0, shift + SECTION_LEN);
        else
            feagure_sec = init_str.substr(shift, SECTION_LEN);
        feagures_str += ' ' + feagure_sec;
    }

    stringstream ss;
    unsigned word;
    unsigned pos = 0;
    ss << feagures_str;
    while (ss >> word)
    {
        set_word(word, pos);
        set_zeros_ahead(find_zeros_ahead(word), pos);
        ++pos;
    }
}

BigInteger::BigInteger(const BigInteger &bi)
{
    *this = bi;
}

BigInteger::BigInteger(const BigInteger &bi, bool sign)
{
    *this = bi;
    this->sign = sign;
}

BigInteger::BigInteger(const BigInteger &bi, int pow, unsigned base)
{
    BigInteger zero = ZERO;

    if ( bi == zero || (static_cast<int>(bi.get_digits()) + pow) <= 0 )
    {
        *this = zero;
        return;
    }

    *this = bi;
    if (pow == 0)
        return;

    if (base == 10)
    {
        if (pow > 0)
        {
            unsigned multiplier, divisor;
            generate_shifting_ele(pow % SECTION_LEN, multiplier, divisor);

            unsigned len = this->get_words_len();
            unsigned upper, lower, residual = 0;
            for (unsigned i = 0; i < len; ++i)
            {
                lower = residual;
                upper = get_words(i) % multiplier;
                residual = get_words(i) / multiplier;
                set_word(upper * divisor + lower, i);
                set_zeros_ahead(find_zeros_ahead(get_words(i)), i);
            }

            if (residual)
            {
                set_word(residual, len);
                set_zeros_ahead(find_zeros_ahead(residual), len);
            }

            ins_zeros_section(pow / SECTION_LEN);
        }
        else
        {
            unsigned pow_abs = abs(pow);
            unsigned del_sections = pow_abs / SECTION_LEN;
            while (del_sections--)
            {
                del_word(0);
                del_zeros_ahead(0);
            }

            unsigned multiplier, divisor;
            generate_shifting_ele(pow_abs % SECTION_LEN, multiplier, divisor);
            unsigned len = get_words_len();
            unsigned upper, lower, residual = get_words(0) / divisor;
            for (unsigned i = 1; i < len; ++i)
            {
                lower = residual;
                upper = get_words(i) % divisor;
                residual = get_words(i) / divisor;
                set_word(upper * multiplier + lower, i - 1);
                set_zeros_ahead(find_zeros_ahead(get_words(i - 1)), i - 1);
            }

            if (residual)
            {
                set_word(residual, len - 1);
                set_zeros_ahead(find_zeros_ahead(residual), len - 1);
            }
            else
            {
                del_word(len - 1);
                del_zeros_ahead(len - 1);
            }
        }
    }
}

/*
    Streaming Operator
*/
void BigInteger::print(ostream &out) const
{
    if (is_neg())
        out << "-";

    int words = get_words_len();
    out << get_words(words - 1);
    for (int pos = words - 2; pos > -1; --pos)
        out << generate_zeros_str(get_zeros_ahead(pos)) << get_words(pos);
}

ostream& operator << (ostream &out, const BigInteger &bi)
{
    bi.print(out);
    return out;
}

istream& operator >> (istream &in, BigInteger &bi)
{
    string init_str;
    in >> init_str;
    bi = BigInteger(init_str);
    return in;
}

/*
    Arithmetic Operator
*/
static BigInteger shift_10r2p(const BigInteger &bi, int pow, unsigned base = 10)
{
    return BigInteger(bi, pow, base);
}

static BigInteger rem_10r2p(const BigInteger &bi, int pow, unsigned base = 10)
{
    BigInteger zero = ZERO;
    unsigned pow_abs = abs(pow);

    if (bi == zero)
        return zero;

    if (bi.get_digits() <= pow_abs)
        return bi;

    BigInteger q_10_nPow = shift_10r2p(bi, -pow_abs, base);
    BigInteger q_10_pPow = shift_10r2p(q_10_nPow, pow_abs, base);
    return bi - q_10_pPow;
}

static BigInteger add(const BigInteger &lhs, const BigInteger &rhs)
{
    BigInteger result = lhs;
    unsigned result_words = result.get_words_len();
    unsigned rhs_words = rhs.get_words_len();
    unsigned sub_result;
    unsigned carry = 0;

    for (unsigned i = 0; i < result_words; ++i)
    {
        if (i < rhs_words)
        {
            sub_result = result.get_words(i) + rhs.get_words(i) + carry;
            carry = sub_result > SECTION_MAX ? 1 : 0;
            sub_result %= BASE;

            result.set_word(sub_result, i);
            result.set_zeros_ahead(find_zeros_ahead(sub_result), i);
        }
        else
        {
            while (carry && i < result_words)
            {
                sub_result = result.get_words(i) + carry;

                if (sub_result > SECTION_MAX)
                {
                    result.set_word(0, i);
                    result.set_zeros_ahead(SECTION_LEN-1, i);
                    carry = 1;
                    ++i;
                }
                else
                {
                    result.set_word(sub_result, i);
                    result.set_zeros_ahead(find_zeros_ahead(sub_result), i);
                    return result;
                }
            }
        }
    }

    if (carry)
    {
        result.set_word(1, result_words);
        result.set_zeros_ahead(SECTION_LEN-1, result_words);
    }

    return result;
}

static BigInteger subtract(const BigInteger &lhs, const BigInteger &rhs)
{
    BigInteger result = lhs;
    unsigned result_words = result.get_words_len();
    unsigned rhs_words = rhs.get_words_len(), rhs_word;
    int sub_result;
    unsigned carry = 0;

    for (unsigned i = 0; i < result_words; ++i)
    {
        if (i < rhs_words)
        {
            sub_result = result.get_words(i) - carry;
            rhs_word = rhs.get_words(i);

            if (sub_result >= rhs_word)
            {
                sub_result -= rhs_word;
                carry = 0;
            }
            else
            {
                sub_result = (SECTION_MAX - rhs_word) + sub_result + 1;
                carry = 1;
            }

            result.set_word(sub_result, i);
            result.set_zeros_ahead(find_zeros_ahead(sub_result), i);
        }
        else
        {
            while (carry && i < result_words)
            {
                sub_result = result.get_words(i) - carry;
                if (sub_result < 0)
                {
                    result.set_word(SECTION_MAX, i);
                    result.set_zeros_ahead(0, i);
                    carry = 1;
                    ++i;
                }
                else
                {
                    result.set_word(sub_result, i);
                    result.set_zeros_ahead(find_zeros_ahead(sub_result), i);
                    carry = 0;
                }
            }

            break;
        }
    }

    int i =  result_words-1;
    while (i > 0 && result.get_words(i) == 0)
    {
        result.del_word(i);
        result.del_zeros_ahead(i);
        --i;
    }

    return result;
}

#if MULTIPLY == KARATSUBA
static BigInteger multiply_karatsuba(const BigInteger &lhs, const BigInteger &rhs)
{
    BigInteger zero = ZERO;

    unsigned n = max(lhs.get_digits(), rhs.get_digits());

    if (lhs == zero || rhs == zero)
        return zero;
    else if (n <= SECTION_LEN)
    {
        long long l = lhs.get_words(0);
        long long r = rhs.get_words(0);
        return BigInteger(l * r);
    }
    else
    {
        BigInteger a0, a1, b0, b1;
        BigInteger r, p, q;
        unsigned m  = floor(n / 2);
        a1 = shift_10r2p(lhs, -m);
        a0 = rem_10r2p(lhs, m);
        b1 = shift_10r2p(rhs, -m);
        b0 = rem_10r2p(rhs, m);
        r = multiply_karatsuba(a1 + a0, b1 + b0);
        p = multiply_karatsuba(a1, b1);
        q = multiply_karatsuba(a0, b0);
        return shift_10r2p(p, 2*m) + shift_10r2p(r - p - q, m) + q;
    }
}
#else
static void fft(vector<complex_t> &X, bool invert = false)
{
    unsigned n = X.size();
    for (unsigned i = 1, j = 0; i < n; ++i)
    {
        unsigned bit = n >> 1;
        for (; j >= bit; bit >>= 1)
            j -= bit;
        j += bit;
        if (i < j)
            swap(X[i], X[j]);
    }

    double _signed_2_pi_ = (invert ? -1 : 1) * 2 * acos(-1);
    for (unsigned len = 2; len <= n; len <<= 1)
    {
        unsigned m = len >> 1;
        double theta = _signed_2_pi_ / len;
        complex_t wlen (cos(theta), sin(theta));
        for (unsigned i = 0; i < n; i += len)
        {
            complex_t w (1);
            for (unsigned j = 0; j < m; ++j)
            {
                complex_t tmp = X[ i + j + m] * w;
                X[ i + j + m ] = X[ i + j ] - tmp;
                X[ i + j ] += tmp;
                w = w * wlen;
            }
        }
    }

    if (invert)
        for (auto &ele : X)
            ele /= n;
}

static BigInteger multiply_fft(const BigInteger &lhs, const BigInteger &rhs)
{
    unsigned lhs_words_len = lhs.get_words_len();
    unsigned rhs_words_len = rhs.get_words_len();

    unsigned n = pow(2, ceil(log2(lhs_words_len + rhs_words_len)));
    vector<complex_t> Z(n, 0);
    for (unsigned i = 0; i < n; ++i)
    {
        Z[i] = complex_t
        {
            i < lhs_words_len ? lhs.get_words(i) : 0,
            i < rhs_words_len ? rhs.get_words(i) : 0
        };
    }

    fft(Z);

    vector<complex_t> R(n, 0);
    for(unsigned i = 1; i < n; ++i)
    {
        double x_real = Z[i].real(), x_imag = Z[i].imag();
        double y_real = Z[n - i].real(), y_imag = Z[n - i].imag();
        complex_t LHS_i = complex_t{ (x_real + y_real) / 2, (x_imag - y_imag) / 2 };
        complex_t RHS_i = complex_t{ (x_imag + y_imag) / 2, -(x_real - y_real) / 2 };
        R[i] = LHS_i * RHS_i;
    }
    R[0] = Z[0].imag() * Z[0].real();

    fft(R, true);

    unsigned R_len = n-1;
    while ( static_cast<unsigned long long>(R[R_len].real() + 0.5) == 0 && --R_len > 0 );
    ++R_len;

    BigInteger result;
    unsigned long long word_64 = 0;
    unsigned word, i = 0;
    do
    {
        if (i < R_len)
            word_64 += static_cast<unsigned long long>(R[i].real() + 0.5);

        if (word_64)
        {
            word = word_64 % BASE;
            word_64 /= BASE;
            result.set_word(word, i);
            result.set_zeros_ahead(find_zeros_ahead(word), i);
        }
        else
        {
            result.set_word(0, i);
            result.set_zeros_ahead(SECTION_LEN-1, i);
        }

        ++i;
    }
    while ( i < R_len || word_64 );
    return result;
}
#endif // MULTIPLY

static BigInteger divide_iteration(const BigInteger &dividend, const BigInteger &divisor)
{
    unsigned divisor_words_len = divisor.get_words_len();
#if MULTIPLY == KARATSUBA
    unsigned long long divisor_leadings = (divisor_words_len > 1 ?
        static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 1)) * BASE +
            divisor.get_words(divisor_words_len - 2) :
        static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 1)));
    unsigned divisor_leadings_len = divisor_words_len > 1 ? 2 : 1;
#else
    unsigned long long divisor_leadings = (divisor_words_len > 2 ?
        static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 1)) * BASE_SQR +
            static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 2)) * BASE +
                divisor.get_words(divisor_words_len - 3) :
        divisor_words_len > 1 ?
            static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 1)) * BASE +
                divisor.get_words(divisor_words_len - 2) :
            static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 1)));
    unsigned divisor_leadings_len = divisor_words_len > 2 ? 3 : divisor_words_len > 1 ? 2 : 1;
#endif // MULTIPLY

    BigInteger divisor_abs(abs(divisor));
    BigInteger rem(abs(dividend)), quo;
    while (rem >= divisor_abs)
    {
        unsigned rem_words_len = rem.get_words_len();
#if MULTIPLY == KARATSUBA
        unsigned long long rem_leadings = (rem_words_len > 1 ?
            static_cast<unsigned long long>(rem.get_words(rem_words_len - 1)) * BASE +
                rem.get_words(rem_words_len - 2) :
            static_cast<unsigned long long>(rem.get_words(rem_words_len - 1)));
        unsigned rem_leadings_len = rem_words_len > 1 ? 2 : 1;
#else
        unsigned long long rem_leadings = (rem_words_len > 2 ?
            static_cast<unsigned long long>(rem.get_words(rem_words_len - 1)) * BASE_SQR +
                static_cast<unsigned long long>(rem.get_words(rem_words_len - 2)) * BASE +
                    rem.get_words(rem_words_len - 3) :
            rem_words_len > 1 ?
                static_cast<unsigned long long>(rem.get_words(rem_words_len - 1)) * BASE +
                    rem.get_words(rem_words_len - 2) :
                static_cast<unsigned long long>(rem.get_words(rem_words_len - 1)));
        unsigned rem_leadings_len = rem_words_len > 2 ? 3 : rem_words_len > 1 ? 2 : 1;
#endif // MULTIPLY
        unsigned words_len_diff = rem_words_len - divisor_words_len - rem_leadings_len + divisor_leadings_len;
        double head =  static_cast<double>(rem_leadings) / divisor_leadings;

        while (head < 1)
        {
            head *= BASE;
            --words_len_diff;
        }

        BigInteger dummy = BigInteger(static_cast<unsigned long long>(head));
        rem -= shift_10r2p(divisor * dummy, words_len_diff * SECTION_LEN);
        quo += shift_10r2p(dummy, words_len_diff * SECTION_LEN);
    }

    return quo;
}

BigInteger BigInteger::operator -() const
{
    return BigInteger(*this, !this->is_neg());
}

BigInteger operator + (BigInteger lhs, const BigInteger &rhs)
{
    BigInteger zero = ZERO;

    if (lhs == zero)
        return rhs;

    if (rhs == zero)
        return lhs;

    if (abs(lhs) == abs(rhs))
    {
        if (lhs.is_neg() != rhs.is_neg())
            return ZERO;
        else
            return BigInteger(add(lhs, rhs), lhs.is_neg());
    }
    else if (abs(lhs) > abs(rhs))
    {
        if (lhs.is_neg() == rhs.is_neg())
            return BigInteger(add(lhs, rhs), lhs.is_neg());
        else
            return BigInteger(subtract(lhs, rhs), lhs.is_neg());
    }
    else
    {
        if (rhs.is_neg() == lhs.is_neg())
            return BigInteger(add(rhs, lhs), rhs.is_neg());
        else
            return BigInteger(subtract(rhs, lhs), rhs.is_neg());
    }
}

BigInteger operator + (BigInteger bi, const long long &x)
{
    return bi + BigInteger(x);
}

BigInteger operator + (long long x, const BigInteger &bi)
{
    return bi + BigInteger(x);
}

BigInteger operator - (BigInteger lhs, const BigInteger &rhs)
{
    return lhs + (-rhs);
}

BigInteger operator - (BigInteger bi,  const long long &x)
{
    return bi - BigInteger(x);
}

BigInteger operator - (long long x,  const BigInteger &bi)
{
    return BigInteger(x) - bi;
}

BigInteger operator * (BigInteger lhs, const BigInteger &rhs)
{
    BigInteger zero = ZERO, one = ONE, min_one = MIN_ONE;

    if (lhs == zero || rhs == zero)
        return zero;

    if (lhs == one)
        return rhs;

    if (lhs == min_one)
        return -rhs;

    if (rhs == one)
        return lhs;

    if (rhs == min_one)
        return -lhs;

    return BigInteger(
#if MULTIPLY == KARATSUBA
        multiply_karatsuba(lhs, rhs),
#else
        multiply_fft(lhs, rhs),
#endif
        lhs.is_neg() ^ rhs.is_neg()
    );
}

BigInteger operator * (BigInteger bi,  const long long &x)
{
    return bi * BigInteger(x);
}

BigInteger operator * (long long x,  const BigInteger &bi)
{
    return bi * BigInteger(x);
}

BigInteger operator / (BigInteger lhs, const BigInteger &rhs)
{
    BigInteger zero = ZERO, one  = ONE, min_one = MIN_ONE;

    if (lhs < rhs)
        return zero;

    if (abs(lhs) == abs(rhs))
    {
        if (lhs.is_neg() == rhs.is_neg())
            return one;
        else
            return min_one;
    }

    if (lhs == zero || rhs == zero)
        return zero;  // including error, divisor cannot be zero in division

    if (rhs == one)
        return lhs;

    if (rhs == min_one)
        return -lhs;

    return BigInteger(
        divide_iteration(lhs, rhs),
        lhs.is_neg() ^ rhs.is_neg()
    );
}

BigInteger operator / (BigInteger bi,  const long long &x)
{
    return bi / BigInteger(x);
}

BigInteger operator / (long long x,  const BigInteger &bi)
{
    return BigInteger(x) / bi;
}

BigInteger operator % (BigInteger lhs, const BigInteger &rhs)
{
    return lhs - (lhs / rhs) * rhs;
}

BigInteger operator % (BigInteger bi,  const long long &x)
{
    return bi % BigInteger(x);
}

BigInteger operator % (long long x,  const BigInteger &bi)
{
    return BigInteger(x) % bi;
}

/*
    Increment / Decrement Operator
*/
BigInteger& BigInteger::operator++()
{
    *this = *this + ONE;
    return *this;
}

BigInteger BigInteger::operator++(int)
{
    BigInteger tmp(*this); // copy
    *this = *this + ONE;
    return tmp;   // return old value
}

BigInteger& BigInteger::operator --()
{
    *this = *this - ONE;
    return *this;
}

BigInteger BigInteger::operator --(int)
{
    BigInteger tmp(*this); // copy
    *this = *this - ONE;
    return tmp;   // return old value
}

/*
    Assignment Operator
*/
BigInteger& BigInteger::operator = (const BigInteger &rhs)
{
    this->words = rhs.get_words();
    this->zeros_ahead = rhs.get_zeros_ahead();
    this->sign = rhs.is_neg();
    return *this;
}

BigInteger& BigInteger::operator = (const long long &x)
{
    *this = BigInteger(x);
    return *this;
}

BigInteger& BigInteger::operator += (const BigInteger &rhs)
{
    *this = *this + rhs;
    return *this;
}

BigInteger& BigInteger::operator += (const long long &x)
{
    *this = *this + BigInteger(x);
    return *this;
}

BigInteger& BigInteger::operator -= (const BigInteger &rhs)
{
    *this = *this - rhs;
    return *this;
}

BigInteger& BigInteger::operator -= (const long long &x)
{
    *this = *this - BigInteger(x);
    return *this;
}

BigInteger& BigInteger::operator *= (const BigInteger &rhs)
{
    *this = *this * rhs;
    return *this;
}

BigInteger& BigInteger::operator *= (const long long &x)
{
    *this = *this * BigInteger(x);
    return *this;
}

BigInteger& BigInteger::operator /= (const BigInteger &rhs)
{
    *this = *this / rhs;
    return *this;
}

BigInteger& BigInteger::operator /= (const long long &x)
{
    *this = *this / BigInteger(x);
    return *this;
}

BigInteger& BigInteger::operator %= (const BigInteger &rhs)
{
    *this = *this % rhs;
    return *this;
}

BigInteger& BigInteger::operator %= (const long long &x)
{
    *this = *this % BigInteger(x);
    return *this;
}

/*
    Comparison Operator
*/
bool operator == (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs.get_digits() == rhs.get_digits())
    {
        if (lhs.is_neg() == rhs.is_neg())
        {
            unsigned lhs_words = lhs.get_words_len();
            for (unsigned i = 0; i < lhs_words; ++i)
            {
                if (lhs.get_words(i) != rhs.get_words(i))
                    return false;
            }
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool operator == (const BigInteger &lhs, const long long &x)
{
    BigInteger rhs(x);
    if (lhs == rhs)
        return true;
    else
        return false;
}

bool operator == (const long long &x, const BigInteger &rhs)
{
    BigInteger lhs(x);
    if (lhs == rhs)
        return true;
    else
        return false;
}

bool operator != (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs == rhs)
        return false;
    else
        return true;
}

bool operator != (const BigInteger &lhs, const long long &x)
{
    BigInteger rhs(x);
    if (lhs == rhs)
        return false;
    else
        return true;
}

bool operator != (const long long &x, const BigInteger &rhs)
{
    BigInteger lhs(x);
    if (lhs == rhs)
        return false;
    else
        return true;
}

bool operator > (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs == rhs)
        return false;

    if (lhs.get_digits() > rhs.get_digits())
        return !lhs.is_neg();
    else if (lhs.get_digits() < rhs.get_digits())
        return rhs.is_neg();
    else
    {
        // equal digits
        if (!lhs.is_neg() & rhs.is_neg())
            return true;
        else if (lhs.is_neg() & !rhs.is_neg())
            return false;
        else
        {
            // equal sign
            int lhs_words = lhs.get_words_len();
            for (int i = lhs_words - 1; i >= 0; --i)
            {
                if (lhs.get_words(i) > rhs.get_words(i))
                    return !lhs.is_neg();
                else if (lhs.get_words(i) < rhs.get_words(i))
                    return rhs.is_neg();
            }
        }
    }

    return false;
}

bool operator > (const BigInteger &lhs, const long long &x)
{
    BigInteger rhs(x);
    if (lhs > rhs)
        return true;
    else
        return false;
}

bool operator > (const long long &x, const BigInteger &rhs)
{
    BigInteger lhs(x);
    if (lhs > rhs)
        return true;
    else
        return false;
}

bool operator <= (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs > rhs)
        return false;
    else
        return true;
}

bool operator <= (const BigInteger &lhs, const long long &x)
{
    BigInteger rhs(x);
    if (lhs > rhs)
        return false;
    else
        return true;
}

bool operator <= (const long long &x, const BigInteger &rhs)
{
    BigInteger lhs(x);
    if (lhs > rhs)
        return false;
    else
        return true;
}

bool operator >= (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs == rhs || lhs > rhs)
        return true;
    else
        return false;
}

bool operator >= (const BigInteger &lhs, const long long &x)
{
    BigInteger rhs(x);
    if (lhs == rhs || lhs > rhs)
        return true;
    else
        return false;
}

bool operator >= (const long long &x, const BigInteger &rhs)
{
    BigInteger lhs(x);
    if (lhs == rhs || lhs > rhs)
        return true;
    else
        return false;
}

bool operator < (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs >= rhs)
        return false;
    else
        return true;
}

bool operator < (const BigInteger &lhs, const long long &x)
{
    BigInteger rhs(x);
    if (lhs >= rhs)
        return false;
    else
        return true;
}

bool operator < (const long long &x, const BigInteger &rhs)
{
    BigInteger lhs(x);
    if (lhs >= rhs)
        return false;
    else
        return true;
}

/*
    Math Functions
*/
BigInteger abs(const BigInteger &bi)
{
    return BigInteger(bi, false);
}

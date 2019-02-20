#include <iostream>
#include <sstream>
#include <vector>
#include <complex>
#include <cmath>

#include "../include/BigInteger.h"
#include "../include/define.h"
#include "../include/utility.h"

using namespace std;

static const BigInteger ZERO;
static const BigInteger ONE(1);
static const BigInteger MIN_ONE(-1);

/*
    Constructor
*/
BigInteger::BigInteger(int64_t word)
{
    if (word)
    {
        if (word < 0)
            set_sign(true);
        else
            set_sign(false);

        uint32_t i = 0;
        while (word)
        {
            uint32_t word_u = abs(word) % BASE;
            set_word(word_u, i);
            set_zeros_ahead(find_zeros_ahead(word_u), i);
            word /= BASE;
            ++i;
        }
    }
    else
    {
        *this = ::ZERO;
    }
}

BigInteger::BigInteger(const string &figure_str)
{
    if ( !validate_figure_string(figure_str) )
    {
        cout << "The value is not a complete integer number!\n" << endl;
        *this = ::ZERO;
        return;
    }

    // prevent -0 from getting into
    int32_t len = figure_str.length();
    if (len == 2 && (figure_str[0] == '-' || figure_str[0] == '+') && figure_str[1] == '0')
    {
        *this = ::ZERO;
        return;
    }

    set_sign(false);
    bool sign_flag = false;
    switch (figure_str[0])
    {
    case '-':
        set_sign(true);
    case '+':
        sign_flag = true;
        break;

    default:
        ;
    }

    uint32_t words = ceil(static_cast<double>(len-sign_flag) / SECTION_LEN);
    int32_t shift = len;
    string figures_str, figure_sec;
    for (uint32_t i = 0; i < words; ++i)
    {
        shift -= SECTION_LEN;
        if (shift < sign_flag)
            figure_sec = figure_str.substr(sign_flag, shift + SECTION_LEN - sign_flag);
        else
            figure_sec = figure_str.substr(shift, SECTION_LEN);
        figures_str += ' ' + figure_sec;
    }

    stringstream ss;
    uint32_t word;
    uint32_t pos = 0;
    ss << figures_str;
    while (ss >> word)
    {
        set_word(word, pos);
        set_zeros_ahead(find_zeros_ahead(word), pos);
        ++pos;
    }
}

BigInteger::BigInteger(const char *figure_str)
{
    *this = string(figure_str);
}

BigInteger::BigInteger(const BigInteger &bi, bool sign)
{
    *this = bi;
    this->sign = sign;
}

BigInteger::BigInteger(const BigInteger &bi, int32_t pow, uint32_t base)
{
    if ( bi == ::ZERO || (static_cast<int32_t>(bi.get_digits()) + pow) <= 0 )
    {
        *this = ::ZERO;
        return;
    }

    *this = bi;
    if (pow == 0)
        return;

    if (base == 10)
    {
        if (pow > 0)
        {
            uint32_t multiplier, divisor;
            generate_shifting_ele(pow % SECTION_LEN, multiplier, divisor);

            uint32_t len = get_words_len();
            uint32_t upper, lower, residual = 0;
            for (uint32_t i = 0; i < len; ++i)
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

            ins_front_zeros_sections(pow / SECTION_LEN);
        }
        else
        {
            uint32_t pow_abs = abs(pow);
            uint32_t del_sections = pow_abs / SECTION_LEN;
            del_front_sections(del_sections);

            uint32_t multiplier, divisor;
            generate_shifting_ele(pow_abs % SECTION_LEN, multiplier, divisor);
            uint32_t len = get_words_len();
            uint32_t upper, lower, residual = get_words(0) / divisor;
            for (uint32_t i = 1; i < len; ++i)
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

    int32_t words = get_words_len();
    out << get_words(words - 1);
    for (int32_t pos = words - 2; pos > -1; --pos)
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
    bi = init_str;
    return in;
}

/*
    Arithmetic Operator
*/
static BigInteger shift_10r2p(const BigInteger &bi, int32_t pow, uint32_t base = 10)
{
    return BigInteger(bi, pow, base);
}

static BigInteger rem_10r2p(const BigInteger &bi, int32_t pow, uint32_t base = 10)
{
    uint32_t pow_abs = abs(pow);

    if (bi == ::ZERO)
        return ::ZERO;

    if (bi.get_digits() <= pow_abs)
        return bi;

    BigInteger q_10_nPow = shift_10r2p(bi, -pow_abs, base);
    BigInteger q_10_pPow = shift_10r2p(q_10_nPow, pow_abs, base);
    return bi - q_10_pPow;
}

static BigInteger add(const BigInteger &lhs, const BigInteger &rhs)
{
    BigInteger result = lhs;
    uint32_t result_words = result.get_words_len();
    uint32_t rhs_words = rhs.get_words_len();
    uint32_t sub_result;
    uint32_t carry = 0;

    for (uint32_t i = 0; i < result_words; ++i)
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
    uint32_t result_words = result.get_words_len();
    uint32_t rhs_words = rhs.get_words_len(), rhs_word;
    int32_t sub_result;
    uint32_t carry = 0;

    for (uint32_t i = 0; i < result_words; ++i)
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

    int32_t i =  result_words-1;
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
    uint32_t n = max(lhs.get_digits(), rhs.get_digits());

    if (lhs == ::ZERO || rhs == ::ZERO)
        return ::ZERO;
    else if (n <= SECTION_LEN)
    {
        int64_t l = lhs.get_words(0);
        int64_t r = rhs.get_words(0);
        return BigInteger(l * r);
    }
    else
    {
        BigInteger a0, a1, b0, b1;
        BigInteger r, p, q;
        uint32_t m  = floor(n / 2);
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
    uint32_t n = X.size();
    for (uint32_t i = 1, j = 0; i < n; ++i)
    {
        uint32_t bit = n >> 1;
        for (; j >= bit; bit >>= 1)
            j -= bit;
        j += bit;
        if (i < j)
            swap(X[i], X[j]);
    }

    double _signed_2_pi_ = (invert ? -1 : 1) * 2 * acos(-1);
    for (uint32_t len = 2; len <= n; len <<= 1)
    {
        uint32_t m = len >> 1;
        double theta = _signed_2_pi_ / len;
        complex_t wlen (cos(theta), sin(theta));
        for (uint32_t i = 0; i < n; i += len)
        {
            complex_t w (1);
            for (uint32_t j = 0; j < m; ++j)
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
    uint32_t lhs_words_len = lhs.get_words_len();
    uint32_t rhs_words_len = rhs.get_words_len();

    uint32_t n = pow(2, ceil(log2(lhs_words_len + rhs_words_len)));
    vector<complex_t> Z(n, 0);
    for (uint32_t i = 0; i < n; ++i)
    {
        Z[i] = complex_t
        {
            i < lhs_words_len ? lhs.get_words(i) : 0,
            i < rhs_words_len ? rhs.get_words(i) : 0
        };
    }

    fft(Z);

    vector<complex_t> R(n, 0);
    for(uint32_t i = 1; i < n; ++i)
    {
        double x_real = Z[i].real(), x_imag = Z[i].imag();
        double y_real = Z[n - i].real(), y_imag = Z[n - i].imag();
        complex_t LHS_i = complex_t{ (x_real + y_real) / 2, (x_imag - y_imag) / 2 };
        complex_t RHS_i = complex_t{ (x_imag + y_imag) / 2, -(x_real - y_real) / 2 };
        R[i] = LHS_i * RHS_i;
    }
    R[0] = Z[0].imag() * Z[0].real();

    fft(R, true);

    uint32_t R_len = n-1;
    while ( static_cast<uint64_t>(R[R_len].real() + 0.5) == 0 && --R_len > 0 );
    ++R_len;

    BigInteger result;
    uint64_t word_64 = 0;
    uint32_t word, i = 0;
    do
    {
        if (i < R_len)
            word_64 += static_cast<uint64_t>(R[i].real() + 0.5);

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
    uint32_t divisor_words_len = divisor.get_words_len();
#if MULTIPLY == KARATSUBA
    uint64_t divisor_leadings = (divisor_words_len > 1 ?
        static_cast<uint64_t>(divisor.get_words(divisor_words_len - 1)) * BASE +
            divisor.get_words(divisor_words_len - 2) :
        static_cast<uint64_t>(divisor.get_words(divisor_words_len - 1)));
    uint32_t divisor_leadings_len = divisor_words_len > 1 ? 2 : 1;
#else
    uint64_t divisor_leadings = (divisor_words_len > 2 ?
        static_cast<uint64_t>(divisor.get_words(divisor_words_len - 1)) * BASE_SQR +
            static_cast<uint64_t>(divisor.get_words(divisor_words_len - 2)) * BASE +
                divisor.get_words(divisor_words_len - 3) :
        divisor_words_len > 1 ?
            static_cast<uint64_t>(divisor.get_words(divisor_words_len - 1)) * BASE +
                divisor.get_words(divisor_words_len - 2) :
            static_cast<uint64_t>(divisor.get_words(divisor_words_len - 1)));
    uint32_t divisor_leadings_len = divisor_words_len > 2 ? 3 : divisor_words_len > 1 ? 2 : 1;
#endif // MULTIPLY

    BigInteger divisor_abs{abs(divisor)};
    BigInteger rem{abs(dividend)}, quo;
    while (rem >= divisor_abs)
    {
        uint32_t rem_words_len = rem.get_words_len();
#if MULTIPLY == KARATSUBA
        uint64_t rem_leadings = (rem_words_len > 1 ?
            static_cast<uint64_t>(rem.get_words(rem_words_len - 1)) * BASE +
                rem.get_words(rem_words_len - 2) :
            static_cast<uint64_t>(rem.get_words(rem_words_len - 1)));
        uint32_t rem_leadings_len = rem_words_len > 1 ? 2 : 1;
#else
        uint64_t rem_leadings = (rem_words_len > 2 ?
            static_cast<uint64_t>(rem.get_words(rem_words_len - 1)) * BASE_SQR +
                static_cast<uint64_t>(rem.get_words(rem_words_len - 2)) * BASE +
                    rem.get_words(rem_words_len - 3) :
            rem_words_len > 1 ?
                static_cast<uint64_t>(rem.get_words(rem_words_len - 1)) * BASE +
                    rem.get_words(rem_words_len - 2) :
                static_cast<uint64_t>(rem.get_words(rem_words_len - 1)));
        uint32_t rem_leadings_len = rem_words_len > 2 ? 3 : rem_words_len > 1 ? 2 : 1;
#endif // MULTIPLY
        uint32_t words_len_diff = rem_words_len - divisor_words_len - rem_leadings_len + divisor_leadings_len;
        double head =  static_cast<double>(rem_leadings) / divisor_leadings;

        while (head < 1)
        {
            head *= BASE;
            --words_len_diff;
        }

        BigInteger dummy{static_cast<int64_t>(head)};
        rem -= shift_10r2p(divisor_abs * dummy, words_len_diff * SECTION_LEN);
        quo += shift_10r2p(dummy, words_len_diff * SECTION_LEN);
    }

    return quo;
}

BigInteger BigInteger::operator -() const
{
    // prevent -0 from getting into
    if (*this == ::ZERO)
        return ::ZERO;
    else
        return BigInteger(*this, !this->is_neg());
}

BigInteger operator + (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs == ::ZERO)
        return rhs;

    if (rhs == ::ZERO)
        return lhs;

    BigInteger lhs_abs{abs(lhs)}, rhs_abs{abs(rhs)};

    if (lhs_abs == rhs_abs)
    {
        if (lhs.is_neg() != rhs.is_neg())
            return ::ZERO;
        else
            return BigInteger(add(lhs, rhs), lhs.is_neg());
    }
    else if (lhs_abs > rhs_abs)
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

BigInteger operator - (const BigInteger &lhs, const BigInteger &rhs)
{
    return lhs + (-rhs);
}

BigInteger operator * (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs == ::ZERO || rhs == ::ZERO)
        return ::ZERO;

    if (lhs == ::ONE)
        return rhs;

    if (lhs == ::MIN_ONE)
        return -rhs;

    if (rhs == ::ONE)
        return lhs;

    if (rhs == ::MIN_ONE)
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

BigInteger operator / (const BigInteger &lhs, const BigInteger &rhs)
{
    if (lhs < rhs)
        return ::ZERO;

    if (abs(lhs) == abs(rhs))
    {
        if (lhs.is_neg() == rhs.is_neg())
            return ::ONE;
        else
            return ::MIN_ONE;
    }

    if (lhs == ::ZERO || rhs == ::ZERO)
        return ::ZERO;  // including error, divisor cannot be zero in division

    if (rhs == ::ONE)
        return lhs;

    if (rhs == ::MIN_ONE)
        return -lhs;

    return BigInteger(
        divide_iteration(lhs, rhs),
        lhs.is_neg() ^ rhs.is_neg()
    );
}

BigInteger operator % (const BigInteger &lhs, const BigInteger &rhs)
{
    return lhs - (lhs / rhs) * rhs;
}

/*
    Increment / Decrement Operator
*/
BigInteger& BigInteger::operator++()
{
    *this = *this + ::ONE;
    return *this;
}

BigInteger BigInteger::operator++(int)
{
    BigInteger tmp(*this); // copy
    *this = *this + ::ONE;
    return tmp;   // return old value
}

BigInteger& BigInteger::operator --()
{
    *this = *this - ::ONE;
    return *this;
}

BigInteger BigInteger::operator --(int)
{
    BigInteger tmp(*this); // copy
    *this = *this - ::ONE;
    return tmp;   // return old value
}

/*
    Assignment Operator
*/
BigInteger& BigInteger::operator += (const BigInteger &rhs)
{
    *this = *this + rhs;
    return *this;
}

BigInteger& BigInteger::operator -= (const BigInteger &rhs)
{
    *this = *this - rhs;
    return *this;
}

BigInteger& BigInteger::operator *= (const BigInteger &rhs)
{
    *this = *this * rhs;
    return *this;
}

BigInteger& BigInteger::operator /= (const BigInteger &rhs)
{
    *this = *this / rhs;
    return *this;
}

BigInteger& BigInteger::operator %= (const BigInteger &rhs)
{
    *this = *this % rhs;
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
            uint32_t lhs_words = lhs.get_words_len();
            for (uint32_t i = 0; i < lhs_words; ++i)
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

bool operator != (const BigInteger &lhs, const BigInteger &rhs)
{
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
            int32_t lhs_words = lhs.get_words_len();
            for (int32_t i = lhs_words - 1; i >= 0; --i)
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

bool operator <= (const BigInteger &lhs, const BigInteger &rhs)
{
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

bool operator < (const BigInteger &lhs, const BigInteger &rhs)
{
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

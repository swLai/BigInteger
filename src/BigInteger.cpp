#include <iostream>
#include <sstream>
#include <vector>
#include <complex>
#include <cmath>

#include "../include/define.h"
#include "../include/utility.h"
#include "../include/pair.h"
#include "../include/BigInteger.h"

#ifndef ZERO
    #define ZERO                    BigInteger(0)
#endif // ZERO
#ifndef ONE
    #define ONE                     BigInteger(1)
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

BigInteger::BigInteger(int word)
{
    unsigned word_u = abs(word) % BASE;
    set_word(word_u, 0);
    set_zeros_ahead(find_zeros_ahead(word_u), 0);

    word_u /= BASE;
    if (word_u)
    {
        set_word(word_u, 1);
        set_zeros_ahead(find_zeros_ahead(word_u), 1);
    }

    if (word < 0)
        set_sign(true);
    else
        set_sign(false);
}

BigInteger::BigInteger(string init_str)
{
    if (!feagure_string_valid_check(init_str))
    {
        cout << "The value is not a complete integer number!\n" << endl;
        set_word(0, 0);
        set_zeros_ahead(SECTION_LEN-1, 0);
        set_sign(false);
        return;
    }

    int len = init_str.length();
    if (init_str[0] != '-' && init_str[0] != '+')
        set_sign(false);
    else
    {
        if (init_str[0] == '-') set_sign(true);
        else set_sign(false);
        init_str.erase(0, 1); // erase the sign character
        len--;
    }

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
    this->words = bi.get_words();
    this->zeros_ahead = bi.get_zeros_ahead();
    this->sign = bi.is_neg();
}

BigInteger::BigInteger(const BigInteger &bi, bool sign)
{
    this->words = bi.get_words();
    this->zeros_ahead = bi.get_zeros_ahead();
    this->sign = sign;
}

BigInteger::BigInteger(const BigInteger &bi, int pow, unsigned base)
{
    BigInteger zero = ZERO;

    if (bi == zero || (bi.get_digits() + pow) <= 0 )
    {
        *this = zero;
        return;
    }

    if (pow == 0)
    {
        *this = bi;
        return;
    }

    if (base == 10)
    {
        stringstream ss;
        string  feagure_str;
        ss << bi; ss >> feagure_str;

        if (pow > 0)
            *this = BigInteger(BigInteger(feagure_str + return_zeros(pow)), bi.is_neg());
        else
            *this = BigInteger(BigInteger(feagure_str.substr(0, feagure_str.size() + pow)), bi.is_neg());
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
        out << return_zeros(get_zeros_ahead(pos)) << get_words(pos);
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

static BigInteger shift_10r2p(const BigInteger &bi, int pow, unsigned base = 10)
{
    if ((int)bi.get_digits() <= -pow)
        return ZERO;
    return BigInteger(bi, pow, base);
}

static BigInteger rem_10r2p(const BigInteger &bi, int pow, unsigned base = 10)
{
    unsigned pow_abs = abs(pow);

    if (bi.get_digits() <= pow_abs)
        return bi;

    BigInteger q_10_nPow = shift_10r2p(bi, -pow_abs, base);
    BigInteger q_10_pPow = shift_10r2p(q_10_nPow, pow_abs, base);
    return bi - q_10_pPow;
}

static BigInteger multiply_karatsuba(const BigInteger &lhs, const BigInteger &rhs)
{
    BigInteger zero = ZERO;
    BigInteger a0, a1, b0, b1;
    BigInteger r, p, q;

   unsigned n = max(lhs.get_digits(), rhs.get_digits()), m;

    if (lhs == zero || rhs == zero)
        return zero;
    else if (n <= SECTION_LEN)
    {
        BigInteger result;
        unsigned l = lhs.get_words(0);
        unsigned r = rhs.get_words(0);
        stringstream ss;
        string s;
        ss << (unsigned long long) l * r;
        ss >> result;
        return result;
    }
    else
    {
        m  = floor(n / 2);
        a1 = shift_10r2p(lhs, -m); a0 = rem_10r2p(lhs, m);
        b1 = shift_10r2p(rhs, -m); b0 = rem_10r2p(rhs, m);
        r = multiply_karatsuba(a1 + a0, b1 + b0);
        p = multiply_karatsuba(a1, b1);
        q = multiply_karatsuba(a0, b0);
        return shift_10r2p(p, 2*m) + shift_10r2p(r - p - q, m) + q;
    }
}

static void fft(vector<complex_t> &X, bool invert = false)
{
    long double _signed_2_pi_ = (invert ? -1 : 1) * 2 * acos(-1);

    unsigned n = X.size();
    Pair(n).tidy_that(X);

    for (unsigned len = 2; len <= n; len <<= 1)
	{
	    unsigned m = len >> 1;
	    long double theta = _signed_2_pi_ / len;
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
}

static BigInteger multiply_fft(const BigInteger &lhs, const BigInteger &rhs)
{
    unsigned lhs_words_len = lhs.get_words_len();
    unsigned rhs_words_len = rhs.get_words_len();

    unsigned n = pow(2, ceil(log2(lhs_words_len + rhs_words_len)));
    vector<complex_t> Z(n, 0);
    for (unsigned i = 0; i < n; ++i)
    {
        Z[i] = complex_t {
            i < lhs_words_len ? lhs.get_words(i) : 0,
            i < rhs_words_len ? rhs.get_words(i) : 0
        };
    }

    fft(Z);

    vector<complex_t> R(n, 0);
	for(unsigned i = 1; i < n; ++i)
	{
		long double x_real = Z[i].real(), x_imag = Z[i].imag();
		long double y_real = Z[n - i].real(), y_imag = Z[n - i].imag();
		complex_t LHS_i = complex_t{ (x_real + y_real) / 2, (x_imag - y_imag) / 2 };
		complex_t RHS_i = complex_t{ (x_imag + y_imag) / 2, -(x_real - y_real) / 2 };
		R[i] = LHS_i * RHS_i;
	}
	R[0] = Z[0].imag() * Z[0].real();

	fft(R, true);

	unsigned R_len = n-1;
	while ( static_cast<unsigned long long>(R[R_len].real() / n + 0.5) == 0 && R_len > 0 ) --R_len;
	++R_len;

	BigInteger result;
    unsigned long long word_64 = 0;
    unsigned word, i = 0;
    do {
        if (i < R_len) word_64 += static_cast<unsigned long long>(R[i].real() / n + 0.5);
        word = word_64 % BASE;
        word_64 /= BASE;
        result.set_word(word, i);
        result.set_zeros_ahead(find_zeros_ahead(word), i);
        ++i;
    } while ( i < R_len || word_64 );
	return result;
}

static BigInteger divide_recur(const BigInteger &rem_km1, const BigInteger &divisor, const BigInteger &quo_km1,
                                          const unsigned &divisor_words_len, const unsigned long long &divisor_leadings)
{
    unsigned rem_km1_words_len = rem_km1.get_words_len();
    unsigned words_len_diff = rem_km1_words_len - divisor_words_len;
    double head = static_cast<double>(rem_km1.get_words(rem_km1_words_len - 1)) / divisor_leadings  * (divisor_words_len > 1 ? BASE : 1);

    if (head < 1)
    {
        head *= BASE;
        --words_len_diff;
    }

    stringstream ss;
    BigInteger quo_k;
    ss << static_cast<unsigned>(head) << return_zeros(words_len_diff * SECTION_LEN);
    ss >> quo_k;

    BigInteger rem_k = rem_km1 - quo_k * divisor;

    if (rem_k < divisor)
        return quo_km1 + quo_k;
    else
        return quo_km1 + divide_recur(rem_k, divisor, quo_k, divisor_words_len, divisor_leadings);
}

static BigInteger divide(const BigInteger &dividend, const BigInteger &divisor, const BigInteger &zero)
{
    unsigned divisor_words_len = divisor.get_words_len();
    unsigned long long divisor_leadings = divisor_words_len > 1 ?
        static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 1)) * BASE + divisor.get_words(divisor_words_len - 2) :
        static_cast<unsigned long long>(divisor.get_words(divisor_words_len - 1));

    return divide_recur(dividend, divisor, zero, divisor_words_len, divisor_leadings);
}

BigInteger BigInteger::operator -() const
{
    return BigInteger(*this, !this->is_neg());
}

BigInteger operator + (BigInteger lhs, const BigInteger &rhs)
{
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

BigInteger operator+(BigInteger bi, const int &x)
{
    if (x == 0) return bi;
    return bi + BigInteger(x);
}

BigInteger operator+(int x, const BigInteger &bi)
{
    if (x == 0) return bi;
    return bi + BigInteger(x);
}

BigInteger operator - (BigInteger lhs, const BigInteger &rhs)
{
    return lhs + (-rhs);
}

BigInteger operator * (BigInteger lhs, const BigInteger &rhs)
{
    BigInteger zero = ZERO, one = ONE, min_one = MIN_ONE;

    if (lhs == zero || rhs == zero) return zero;
    if (lhs == one) return rhs;
    if (lhs == min_one) return -rhs;
    if (rhs == one) return lhs;
    if (rhs == min_one) return -lhs;

    return BigInteger(
        multiply_fft(lhs, rhs), // multiply_karatsuba(lhs, rhs),
        lhs.is_neg() ^ rhs.is_neg()
    );
}

BigInteger operator / (BigInteger lhs, const BigInteger &rhs)
{
    BigInteger zero = ZERO, one  = ONE, min_one = MIN_ONE;

    if (lhs < rhs) return zero;
    if (abs(lhs) == abs(rhs))
    {
        if (lhs.is_neg() == rhs.is_neg()) return one;
        else return min_one;
    }
    if (lhs == zero || rhs == zero) return zero;  // including error, divisor cannot be zero in division
    if (rhs == one) return lhs;
    if (rhs == min_one) return -lhs;

    return BigInteger(
        divide(lhs, rhs, zero),
        lhs.is_neg() ^ rhs.is_neg()
    );
}

BigInteger operator % (BigInteger lhs, const BigInteger &rhs)
{
    return lhs - (lhs / rhs) * rhs;
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
        } else
            return false;
    } else
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
    if (lhs >= rhs )
        return false;
    else
        return true;
}


/*
    Assignment Operator
*/
BigInteger& BigInteger::operator=(const BigInteger &rhs)
{
    this->words = rhs.get_words();
    this->zeros_ahead = rhs.get_zeros_ahead();
    this->sign = rhs.is_neg();
    return *this;
}

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
    Math Functions
*/
BigInteger abs(const BigInteger &bi)
{
    return BigInteger(bi, false);
}

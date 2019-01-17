#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "../include/BigInteger.h"
#include "../include/define.h"
#include "../include/utility.h"

#define ZERO_9_APX             "000000000"
#define ZERO_1_APX             "0"

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
    unsigned word_u = (word & SECTION_MASK) % BASE;
    set_word(word_u, 0);
    set_zeros_ahead(find_zeros_ahead(word_u), 0);
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

    unsigned words = ceil((float)len / SECTION_LEN);
    int shift = len;
    string feagures_str, feagure_sec;
    for (unsigned i = 0; i < words; i++)
    {
        shift -= SECTION_LEN;
        if (shift < 0) {
            feagure_sec = init_str.substr(0, shift + SECTION_LEN);
        } else {
            feagure_sec = init_str.substr(shift, SECTION_LEN);
        }
        feagures_str += ' ' + feagure_sec;
    }

    stringstream ss;
    unsigned word;
    unsigned pos = 0;
    ss << feagures_str;
    while (ss >> word) {
        set_word(word, pos);
        set_zeros_ahead(find_zeros_ahead(word), pos);
        pos++;
    }
}

BigInteger::BigInteger(const BigInteger &bi){
    this->words = bi.get_words();
    this->zeros_ahead = bi.get_zeros_ahead();
    this->sign = bi.is_neg();
}

BigInteger::BigInteger(const BigInteger &bi, bool sign) {
    this->words = bi.get_words();
    this->zeros_ahead = bi.get_zeros_ahead();
    this->sign = sign;
}

BigInteger::BigInteger(const BigInteger &bi, int pow, unsigned base)
{
    BigInteger zero = ZERO;
    if (bi == zero)
        *this = zero;
    else
    {
        if (base == 10)
        {
            stringstream ss;
            string  shifted_str, feagure_str;
            ss << bi; ss >> feagure_str;

            if (pow >= 0)
            {
                string zeros_str;
                for (unsigned i = 0; i < pow; i++)
                    zeros_str += ZERO_1_APX;
                shifted_str = feagure_str + zeros_str;
            }
            else
            {
                if (feagure_str.size() + pow > 0)
                    shifted_str = feagure_str.substr(0, feagure_str.size() + pow);
                else
                    shifted_str = ZERO_1_APX;
            }
            *this = BigInteger(BigInteger(shifted_str), bi.is_neg());
        }
    }
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
    Streaming Operator
*/
void BigInteger::print(ostream &out) const
{
    if (is_neg())
        out << "-";

    int words = get_words_len();
    unsigned zeros;
    string zeros_str;
    out << get_words(words - 1);
    for (int pos = words - 2; pos > -1; pos--)
    {
        zeros = get_zeros_ahead(pos);
        for(unsigned i = 0; i < zeros; i++)
            zeros_str += ZERO_1_APX;
        out << zeros_str << get_words(pos);
        zeros_str.erase(0);
    }
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
    BigInteger result;
    int result_words = lhs.get_words_len();
    int rhs_words = rhs.get_words_len();
    unsigned lhs_word, rhs_word;
    unsigned sub_result;
    unsigned carry = 0;

    for (int i = 0; i < result_words; i++)
    {
        lhs_word = lhs.get_words(i) + carry;
        rhs_word = i < rhs_words ? rhs.get_words(i) : 0;

        sub_result = lhs_word + rhs_word;
        carry = sub_result > SECTION_MAX ? 1 : 0;
        sub_result %= BASE;

        result.set_word(sub_result, i);
        result.set_zeros_ahead(find_zeros_ahead(sub_result), i);
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
    BigInteger result;
    int result_words = lhs.get_words_len();
    int rhs_words = rhs.get_words_len();
    unsigned lhs_word, rhs_word;
    int sub_result;
    unsigned carry = 0;

    for (int i = 0; i < result_words; i++)
    {
        lhs_word = lhs.get_words(i) - carry;
        rhs_word = i < rhs_words ? rhs.get_words(i) : 0;

        if (lhs_word >= rhs_word)
        {
            sub_result = lhs_word - rhs_word;
            carry = 0;
        }
        else
        {
            sub_result = (SECTION_MAX - rhs_word) + lhs_word + 1;
            carry = 1;
        }

        result.set_word(sub_result, i);
        result.set_zeros_ahead(find_zeros_ahead(sub_result), i);
    }

    int i =  result_words-1;
    while (i > 0 && result.get_words(i) == 0)
    {
        result.del_word(i);
        result.del_zeros_ahead(i);
        i--;
    }

    return result;
}

static BigInteger shift_10(const BigInteger &bi, int pow, unsigned base = 10)
{
    if ((int)bi.get_digits() <= -pow)
        return ZERO;
    return BigInteger(bi, pow, base);
}

static BigInteger rem_10(const BigInteger &bi, int pow, unsigned base = 10)
{
    if (bi.get_digits() <= abs(pow))
        return bi;

    BigInteger q_10_nPow = shift_10(bi, -abs(pow), base);
    BigInteger q_10_pPow = shift_10(q_10_nPow, abs(pow), base);
    return bi - q_10_pPow;
}

static BigInteger multiply(const BigInteger &lhs, const BigInteger &rhs)
{
    BigInteger a0, a1, b0, b1;
    BigInteger r, p, q;

   unsigned n = max(lhs.get_digits(), rhs.get_digits()), m;

    if (lhs == ZERO || rhs == ZERO)
        return ZERO;
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
        a1 = shift_10(lhs, -m); a0 = rem_10(lhs, m);
        b1 = shift_10(rhs, -m); b0 = rem_10(rhs, m);
        r = multiply(a1 + a0, b1 + b0);
        p = multiply(a1, b1);
        q = multiply(a0, b0);
        return shift_10(p, 2*m) + shift_10(r - p - q, m) + q;
    }
}

static BigInteger divide(const BigInteger &rem_km1, const BigInteger &divisor, const BigInteger &quo_km1)
{
    static unsigned divisor_words_len = divisor.get_words_len();
    static long long divisor_top = divisor_words_len > 1 ?
                                          (long long)divisor.get_words(divisor_words_len - 1) * BASE + divisor.get_words(divisor_words_len - 2) :
                                          (long long)divisor.get_words(divisor_words_len - 1);
    unsigned rem_km1_words_len = rem_km1.get_words_len();
    unsigned words_len_diff = rem_km1_words_len - divisor_words_len;
    double head = (double)rem_km1.get_words(rem_km1_words_len - 1) / divisor_top * (divisor_words_len > 1 ? BASE : 1);

    if (rem_km1 < divisor)
        return quo_km1;

    if (head < 1)
    {
        head *= BASE;
        words_len_diff--;
    }

    string zeros_str;
    for (unsigned i = 0; i < words_len_diff; i++)
        zeros_str += ZERO_9_APX;

    stringstream ss;
    BigInteger quo_k;
    ss << (unsigned)head << zeros_str;
    ss >> quo_k;

    BigInteger dummy = quo_k * divisor;
    BigInteger rem_k = rem_km1 - dummy;

    if (rem_k < divisor)
        return quo_km1 + quo_k;
    else
        return quo_km1 + divide(rem_k, divisor, quo_k);
}

BigInteger BigInteger::operator -() const
{ // unary minus sign
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

    return BigInteger(multiply(lhs, rhs), lhs.is_neg() ^ rhs.is_neg());
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

    return BigInteger(divide(lhs, rhs, zero), lhs.is_neg() ^ rhs.is_neg());
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
            for (unsigned i = 0; i < lhs_words; i++)
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
            for (int i = lhs_words - 1; i >= 0; i--)
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
    Math Functions
*/
BigInteger abs(const BigInteger &bi)
{
    return BigInteger(bi, false);
}

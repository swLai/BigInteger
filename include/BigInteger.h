#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <vector>
using namespace std;

#include "define.h"

class BigInteger
{
private:
    bool sign;
    vector<uint32_t> words;
    vector<uint32_t> zeros_ahead;

public:
    // no initializer
    BigInteger();
    // initialized by integer
    BigInteger(int64_t);
    // initialized by feagure string
    BigInteger(string);
    BigInteger(const char *);
    // initialized by computational BIs
    BigInteger(const BigInteger &);
    // initialized by other BI but self-decided sign
    BigInteger(const BigInteger &, bool);
    // for shifting
    BigInteger(const BigInteger &, int32_t, uint32_t);

    // Element Operator
    void set_sign(bool sign)
    {
        this->sign = sign;
    }
    void set_word(uint32_t word, uint32_t pos)
    {
        if (pos < this->words.size())
        {
            this->words[pos] = word;
        }
        else
        {
            // out of range
            this->words.push_back(word);
        }
    }
    void set_zeros_ahead(uint32_t zeros, uint32_t pos)
    {
        if (pos < this->zeros_ahead.size())
        {
            this->zeros_ahead[pos] = zeros;
        }
        else
        {
            // out of range
            this->zeros_ahead.push_back(zeros);
        }
    }
    void del_word(uint32_t pos)
    {
        if (pos < this->words.size())
        {
            this->words.erase(this->words.begin() + pos);
        }
    }
    void del_zeros_ahead(uint32_t pos)
    {
        if (pos < this->zeros_ahead.size())
        {
            this->zeros_ahead.erase(this->zeros_ahead.begin() + pos);
        }
    }
    void ins_front_zeros_sections(uint32_t times)
    {
        while (times--)
        {
            this->words.insert(this->words.begin(), 0);
            this->zeros_ahead.insert(this->zeros_ahead.begin(), SECTION_LEN - 1);
        }
    }
    void del_front_sections(uint32_t times)
    {
        while (times--)
        {
            this->words.erase(this->words.begin());
            this->zeros_ahead.erase(this->zeros_ahead.begin());
        }
    }
    bool is_neg(void) const
    {
        return this->sign;
    }
    vector<uint32_t> get_zeros_ahead(void) const
    {
        return this->zeros_ahead;
    }
    uint32_t get_zeros_ahead(uint32_t pos)  const
    {
        return this->zeros_ahead[pos];
    }
    vector<uint32_t> get_words(void) const
    {
        return this->words;
    }
    uint32_t get_words(uint32_t pos) const
    {
        return this->words[pos];
    }
    uint32_t get_words_len(void) const
    {
        return this->words.size();
    }
    uint32_t get_digits(void) const
    {
        uint32_t words = this->words.size();
        uint32_t zeros_ahead = this->zeros_ahead[words-1];
        return (words * SECTION_LEN - zeros_ahead);
    }

    // Array Operator
    BigInteger& operator [] (uint32_t n)
    {
        return *(this + (n * sizeof(BigInteger)));
    }

    // Increment / Decrement Operator
    BigInteger& operator ++(); // prefix
    BigInteger  operator ++(int); // postfix
    BigInteger& operator --(); // prefix
    BigInteger  operator --(int); // postfix

    // Assignment Operator
    BigInteger& operator  = (const BigInteger &);
    BigInteger& operator += (const BigInteger &);
    BigInteger& operator -= (const BigInteger &);
    BigInteger& operator *= (const BigInteger &);
    BigInteger& operator /= (const BigInteger &);
    BigInteger& operator %= (const BigInteger &);

    // Arithmetic Operator (friend)
    BigInteger operator -() const;
    friend BigInteger operator + (const BigInteger &, const BigInteger &);
    friend BigInteger operator - (const BigInteger &,  const BigInteger &);
    friend BigInteger operator * (const BigInteger &, const BigInteger &);
    friend BigInteger operator / (const BigInteger &, const BigInteger &);
    friend BigInteger operator % (const BigInteger &, const BigInteger &);

    // Streaming Operator (friend)
    void print(ostream &) const;
    friend istream& operator >> (istream &, BigInteger &);
    friend ostream& operator << (ostream &, const BigInteger &);
};

// Streaming Operator
istream& operator >> (istream &, BigInteger &);
ostream& operator << (ostream &, const BigInteger &);

// Arithmetic Operator
BigInteger operator + (const BigInteger &, const BigInteger &);
BigInteger operator - (const BigInteger &, const BigInteger &);
BigInteger operator * (const BigInteger &, const BigInteger &);
BigInteger operator / (const BigInteger &, const BigInteger &);
BigInteger operator % (const BigInteger &, const BigInteger &);

// Comparison Operator
bool operator == (const BigInteger &, const BigInteger &);
bool operator != (const BigInteger &, const BigInteger &);
bool operator > (const BigInteger &, const BigInteger &);
bool operator < (const BigInteger &, const BigInteger &);
bool operator >= (const BigInteger &, const BigInteger &);
bool operator <= (const BigInteger &, const BigInteger &);

// Math Functions
BigInteger abs(const BigInteger &);

#endif // BIGINTEGER_H

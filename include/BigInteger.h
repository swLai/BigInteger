#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <vector>
#include "define.h"

using namespace std;

class BigInteger
{
    private:
        bool sign;
        vector<unsigned> words;
        vector<unsigned> zeros_ahead;

    public:
        // no initializer
        BigInteger();
        // initialized by integer
        BigInteger(int);
        // initialized by feagure string
        BigInteger(string);
        // initialized by computational BIs
        BigInteger(const BigInteger &);
        // initialized by other BI but self-decided sign
        BigInteger(const BigInteger &, bool);
        // for shifting
        BigInteger(const BigInteger &, int, unsigned);

        // Element Operator
        void set_word(unsigned word, unsigned pos) {
            if (pos < this->words.size()) {
                this->words[pos] = word;
            } else {
                // out of range
                this->words.push_back(word);
            }
        }
        void set_zeros_ahead(unsigned zeros, unsigned pos) {
            if (pos < this->zeros_ahead.size()) {
                this->zeros_ahead[pos] = zeros;
            } else {
                // out of range
                this->zeros_ahead.push_back(zeros);
            }
        }
        void set_sign(bool sign) {
            this->sign = sign;
        }
        bool is_neg(void) const {
            return this->sign;
        }
        vector<unsigned> get_zeros_ahead(void ) const {
            return this->zeros_ahead;
        }
        unsigned get_zeros_ahead(unsigned pos)  const{
            return this->zeros_ahead[pos];
        }
        vector<unsigned> get_words(void) const {
            return this->words;
        }
        unsigned get_words(unsigned pos) const {
            return this->words[pos];
        }
        unsigned get_words_len(void) const {
            return this->words.size();
        }
        unsigned get_digits(void) const {
            unsigned words = this->words.size();
            unsigned zeros_ahead = this->get_zeros_ahead(words-1);
            return (SECTION_LEN * words - zeros_ahead);
        }
        void del_word(unsigned pos) {
            if (pos < this->words.size()) {
                this->words.erase(this->words.begin() + pos);
            }
        }
        void del_zeros_ahead(unsigned pos) {
            if (pos < this->zeros_ahead.size()) {
                this->zeros_ahead.erase(this->zeros_ahead.begin() + pos);
            }
        }

        // Array Operator
        BigInteger& operator [] (unsigned n) {
            return *(this + (n * sizeof(BigInteger)));
        }

        // Increment / Decrement Operator
        BigInteger& operator ++(); // prefix
        BigInteger  operator ++(int); // postfix
        BigInteger& operator --(); // prefix
        BigInteger  operator --(int); // postfix

        // Assignment Operator
        BigInteger& operator = (const BigInteger &);
        BigInteger& operator += (const BigInteger &);
        BigInteger& operator -= (const BigInteger &);
        BigInteger& operator *= (const BigInteger &);
        BigInteger& operator /= (const BigInteger &);
        BigInteger& operator %= (const BigInteger &);

        // Arithmetic Operator (friend)
        BigInteger operator -() const;
        friend BigInteger operator+(BigInteger, const BigInteger &);
        friend BigInteger operator-(BigInteger,  const BigInteger &);
        friend BigInteger operator * (BigInteger, const BigInteger &);
        friend BigInteger operator / (BigInteger, const BigInteger &);
        friend BigInteger operator % (BigInteger, const BigInteger &);

        // Streaming Operator (friend)
        void print(ostream &) const;
        friend istream& operator >> (istream &, BigInteger &);
        friend ostream& operator << (ostream &, const BigInteger &);
};

// Streaming Operator
istream& operator >> (istream &, BigInteger &);
ostream& operator << (ostream &, const BigInteger &);

// Arithmetic Operator
BigInteger operator+(BigInteger, const BigInteger &);
BigInteger operator-(BigInteger, const BigInteger &);
BigInteger operator * (BigInteger, const BigInteger &);
BigInteger operator / (BigInteger, const BigInteger &);
BigInteger operator % (BigInteger, const BigInteger &);

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

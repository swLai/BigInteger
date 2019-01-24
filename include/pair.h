#ifndef PAIR_H
#define PAIR_H

#ifdef DEBUG
#include <iostream>
#endif

#include <vector>

using namespace std;

class Pair
{
    private:
        unsigned* seq;

    public:
        Pair(unsigned N)
        {
            unsigned D = N >> 1;
            unsigned L = D >> 1;
            unsigned S = L >> 1;

            this->seq = new unsigned [N];
            for (unsigned i = 0; i < S; i++)
            {
                unsigned tmp = i << 1;
                this->seq[tmp] = tmp;
                this->seq[tmp + L] = tmp + 1;
            }

            for (unsigned i = 0; i < L; i++)
            {
                unsigned pos = i << 1, pos_shift = pos + D;
                unsigned  val = this->seq[pos] << 1;
                this->seq[pos] = val;
                this->seq[pos + 1]= val + D;
                this->seq[pos_shift] = val + 1;
                this->seq[pos_shift + 1] = val + 1 + D;
            }
        }

        unsigned* get_sequence(void)
        {
#ifdef DEBUG
            for (unsigned i = 0; i < sizeof(this->seq); i++)
                cout << this->seq[i] << ' ';
            cout << endl << endl;
#endif // DEBUG
            return this->seq;
        }

        template <class T>
        void tidy_that(vector<T> &x)
        {
            vector<T> x_tmp(x);
            unsigned n = x.size();
            for (unsigned i = 0; i < n; i++)
                x[i] = x_tmp[this->seq[i]];
        }
};

#endif // PAIR_H

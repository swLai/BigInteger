#ifndef PAIR_H
#define PAIR_H

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

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

#ifdef DEBUG
            cout << "D=" << D;
            cout << ", L=" << L;
            cout << ", S=" << S << endl;
            cout << endl;
#endif // DEBUG

            seq = new unsigned [N];

            for (unsigned i = 0; i < S; i++)
            {
                unsigned tmp = 2 * i;
                seq[tmp] = tmp;
                seq[tmp + L] = tmp + 1;
            }

            for (unsigned i = 0, k = 0; i < L; ++i, k+=2)
            {
                unsigned pos = i << 1, pos_shift = pos + D;
                unsigned  val = seq[pos] << 1;
                seq[pos] = val;
                seq[pos + 1]= val + D;
                seq[pos_shift] = val + 1;
                seq[pos_shift + 1] = val + 1 + D;
            }
        }
        unsigned* get_sequence(void)
        {
#ifdef DEBUG
            for (unsigned i = 0; i < sizeof(seq); i++)
                cout << this->seq[i] << ' ';
            cout << endl << endl;
#endif // DEBUG
            return this->seq;
        }
};

#endif // PAIR_H

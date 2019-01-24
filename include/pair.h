#ifndef PAIR_H
#define PAIR_H

//#define DEBUG
#define EVEN    0
#define ODD     1

#include <iostream>
using namespace std;

class Pair {

    private:
        unsigned D;
        unsigned *pairs[2];
        unsigned *seq;
        unsigned *gc;

    public:
        Pair(unsigned N)
        {
            D = N >> 1;
            unsigned L = D >> 1;
            unsigned S = L >> 1;
            unsigned M = L - 1;

#ifdef DEBUG
            cout << "D=" << D;
            cout << ", L=" << L;
            cout << ", S=" << S;
            cout << ", M=" << M << endl;
            cout << endl;
#endif // DEBUG

            for (auto &p : pairs)
                p = new unsigned [D];
            seq = new unsigned [N];
            gc = new unsigned [L];

            for (unsigned i = 0; i < S; i++)
            {
                unsigned tmp = 2 * i;
                gc[i] = tmp;
                gc[i + S] = tmp + 1;
            }

#ifdef DEBUG
            cout << "group code: ";
            for (unsigned i = 0; i < L; i++)
                cout << gc[i] << ' ';
            cout << endl << endl;
#endif // DEBUG

            for (unsigned i = 0, k = 0; i < L; ++i, k+=2)
            {
                pairs[EVEN][gc[i]] = k;
                pairs[ODD][gc[i]] = k + D;
                pairs[EVEN][gc[i] + L] = k + 1;
                pairs[ODD][gc[i] + L] = k + 1 + D;
            }

#ifdef DEBUG
            for (unsigned i = 0; i < D; i++)
                cout << "(" << pairs[EVEN][i] << ", " << pairs[ODD][i] << ")";
            cout << endl << endl;
#endif // DEBUG
        }
        unsigned get_pairs_length(void) {
            return this->D;
        }
        unsigned ** get_pairs(void)
        {
            return this->pairs;
        }
        unsigned * get_sequence(void)
        {
            for (unsigned i = 0; i < this->D; i++)
            {
                this->seq[2*i] = this->pairs[EVEN][i];
                this->seq[2*i + 1] = this->pairs[ODD][i];
            }
#ifdef DEBUG
            for (unsigned i = 0; i < this->D << 1; i++)
                cout << this->seq[i] << ' ';
            cout << endl << endl;
#endif // DEBUG

            return this->seq;
        }
};


#endif // PAIR_H

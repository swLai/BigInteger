#ifndef PAIR_H
#define PAIR_H

#include <vector>
using namespace std;

class Pair
{
    private:
        vector<unsigned> seq;

    public:
        Pair(unsigned N)
        {
            seq = vector<unsigned>(N, 0);
            pair_generate(seq);
        }

        void pair_generate(vector<unsigned> &seq)
        {
            auto N_reduced = seq.size() >> 2;

            switch (N_reduced)
            {
                case 0:
                    seq = vector<unsigned>{0, 1};
                    return;

                case 1:
                    seq = vector<unsigned>{0, 2, 1, 3};
                    return;

                default:
                {
                    vector<unsigned> par(N_reduced, 0);
                    pair_generate(par);
                    for (auto &ele : par) ele <<= 2;
                    vector<unsigned> gen{0, 2, 1, 3};
                    unsigned i = 0;
                    for (auto g : gen)
                        for (auto ele : par)
                            seq[i++] = ele + g;
                    return;
                }
            }
        }

        template <typename T>
        void tidy_that(vector<T> &x)
        {
            vector<T> x_tmp(x);
            auto n = x_tmp.size();
            while (n--)
                x[n] = x_tmp[ this->seq[n] ];
        }
};

#endif // PAIR_H

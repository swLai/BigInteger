#include "../include/utility.h"
#include "../include/define.h"

using namespace std;

bool validate_feagure_string(string &s)
{
    int len = s.length();

    if (len == 0)
        return false;

    if (len == 1)
    {
        char c = s[0] - '0';
        if (c > 9 || c < 0)
            return false;
        else
            return true;
    }

    switch(s[0])
    {
    case '-':
    case '+':
        for (int i = 1; i < len; i++)
        {
            char c = s[i] - '0';
            if (c > 9 || c < 0)
                return false;
        }
        break;

    default:
        for (int i = 0; i < len; i++)
        {
            char c = s[i] - '0';
            if (c > 9 || c < 0)
                return false;
        }
    }

    return true;
}

unsigned find_zeros_ahead(unsigned word)
{
    unsigned zeros_ahead = 0;
    unsigned filter = BASE / 10;

    while ((filter^0x1) && word < filter)
    {
        ++zeros_ahead;
        filter /= 10;
    }
    return zeros_ahead;
}

void generate_shifting_ele(unsigned pow, unsigned &multiplier, unsigned &divisor)
{
    switch (pow)
    {
    case 0:
        multiplier = BASE;
        divisor = 1;
        return;

    case 1:
        multiplier = BASE / 10;
        divisor = 10;
        return;

    case 2:
        multiplier = BASE / 100;
        divisor = 100;
        return;

    case 3:
        multiplier = BASE / 1000;
        divisor = 1000;
        return;

    case 4:
        multiplier = BASE / 10000;
        divisor = 10000;
        return;

    case 5:
        multiplier = BASE / 100000;
        divisor = 100000;
        return;

    case 6:
        multiplier = BASE / 1000000;
        divisor = 1000000;
        return;

    case 7:
        multiplier = BASE / 10000000;
        divisor = 10000000;
        return;

    case 8:
        multiplier = BASE / 100000000;
        divisor = 100000000;
        return;

    default:
        return;
    }
}

string generate_zeros_str(unsigned zeros)
{
    string zeros_str;
    unsigned sections = zeros / SECTION_LEN;
    unsigned zeros_remain = zeros % SECTION_LEN;

    while(sections--)
        zeros_str += SECTION_ZERO;

    switch (zeros_remain)
    {
    case 0:
        return zeros_str;

    case 1:
        return zeros_str + ZERO_1_APX;

    case 2:
        return zeros_str + ZERO_2_APX;

    case 3:
        return zeros_str + ZERO_3_APX;

    case 4:
        return zeros_str + ZERO_4_APX;

    case 5:
        return zeros_str + ZERO_5_APX;

    case 6:
        return zeros_str + ZERO_6_APX;

    case 7:
        return zeros_str + ZERO_7_APX;

    case 8:
        return zeros_str + ZERO_8_APX;

    default:
        return zeros_str;
    }
}

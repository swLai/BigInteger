#include <string>
#include <sstream>
#include <cmath>
#include <iostream>
#include "../include/utility.h"
#include "../include/define.h"

using namespace std;

bool feagure_string_valid_check(string &s)
{
    int len = s.length();

    if (len == 0) {
        cout << "I am here" << endl;
        return false;
    }

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
    string s;
    stringstream ss;
    ss << word;
    ss >> s;

    unsigned len = s.length();
    return abs(SECTION_LEN - len);
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

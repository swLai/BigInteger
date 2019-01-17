#include <string>
#include <sstream>
#include <cmath>
#include "../include/utility.h"
#include "../include/define.h"

using namespace std;

bool feagure_string_valid_check(string &s)
{
    int len = s.length();

    if (len == 0) return false;
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

#include "utils.h"

vector<string> TTT_utils::split(string str, string delim) {

    vector<string> arr;

    auto start = 0U;
    auto end = str.find(delim);
    int index = 0;
    while (end != string::npos)
    {
        arr.push_back(str.substr(start, end - start));
        start = end + delim.length();
        end = str.find(delim, start);
    }

    arr.push_back(str.substr(start, end));

    return arr;
}

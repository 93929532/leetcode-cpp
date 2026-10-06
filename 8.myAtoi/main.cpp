#include <iostream>
#include <string>
#include <climits>
#include <regex>

using namespace std;

class Solution {
public:
    int myAtoi(string s) {
        regex re(R"(^[ ]*([+-]?)(\d+))");
        smatch m;
        if (!regex_search(s, m, re)) {
            return 0;
        }

        int sign = (m[1].str() == "-") ? -1 : 1;

        long long num = 0;
        for (char c : m[2].str()) {
            num = num * 10 + (c - '0');
            if (sign == 1 && num > INT_MAX) return INT_MAX;
            if (sign == -1 && num > (long long)INT_MAX + 1) return INT_MIN;
        }
        return static_cast<int>(sign * num);
    }
};

int main() {
    Solution sol;

    struct TestCase {
        string input;
        int expected;
    };

    TestCase cases[] = {
        {"42", 42},
        {" -042", -42},
        {"1337c0d3", 1337},
        {"0-1", 0},
        {"words and 987", 0},
        {"   ", 0},
        {"-91283472332", INT_MIN},
        {"91283472332", INT_MAX},
        {"+", 0},
        {"-", 0},
        {"2147483648", INT_MAX},
        {"-2147483648", INT_MIN},
        {"+0", 0},
        {"00000", 0},
    };

    int passed = 0;
    int total = 0;
    for (const auto& tc : cases) {
        ++total;
        int got = sol.myAtoi(tc.input);
        bool ok = (got == tc.expected);
        if (ok) {
            ++passed;
        }
        std::cout << (ok ? "[PASS] " : "[FAIL] ")
                  << "myAtoi(\"" << tc.input << "\") = " << got
                  << " (expected " << tc.expected << ")" << std::endl;
    }

    std::cout << "\nResult: " << passed << "/" << total
              << " tests passed." << std::endl;
    return passed == total ? 0 : 1;
}

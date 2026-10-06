#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
public:
    int romanToInt(string s) {
        /*
        vector<pair<string, int>> Roman_0 = {
            {"I", 1},
            {"V", 5},
            {"X", 10},
            {"L", 50},
            {"C", 100},
            {"D", 500},
            {"M", 1000}
        };

        vector<pair<string, int>> Roman_1 = {
            {"IV", 4},
            {"IX", 9},
            {"XL", 40},
            {"XC", 90},
            {"CD", 400},
            {"CM", 900},
        };

        int num = 0;
        int i;

        for(i = 0 ;i < Roman_1.size() ;i ++)
        {
            while(s.find(Roman_1[i].first) != string::npos)
            {
                num += Roman_1[i].second;
                //s.replace(s.find(Roman_1[i].first) ,Roman_1[i].first.size() ,"");
                s.erase(s.find(Roman_1[i].first) ,Roman_1[i].first.size());
            }
        }
        for(i = 0 ;i < Roman_0.size() ;i ++)
        {
            while(s.find(Roman_0[i].first) != string::npos)
            {
                num += Roman_0[i].second;
                //s.replace(s.find(Roman_0[i].first) ,Roman_0[i].first.size() ,"");
                s.erase(s.find(Roman_0[i].first) ,Roman_0[i].first.size());
            }
        }

        return num;
        */
        // 每个罗马字符对应的数值
        unordered_map<char, int> romanMap = {
            {'I', 1},
            {'V', 5},
            {'X', 10},
            {'L', 50},
            {'C', 100},
            {'D', 500},
            {'M', 1000}
        };

        // ================== 相邻比较法 ==================
        // 核心规律：从左往右扫，如果「当前字符 < 下一个字符」，
        // 说明当前字符是减法组合（IV/IX/XL/XC/CD/CM）的左半边，要减去它；
        // 否则它就是普通加法部分，直接加上。
        int num = 0;

        for (size_t i = 0; i < s.size(); i++)
        {
            int cur = romanMap[s[i]];

            // 先假设「没有下一位」，把 next 当成 0
            int next = 0;
            // 如果后面还有字符，才去真正取下一位的值
            if (i + 1 < s.size())
            {
                next = romanMap[s[i + 1]];
            }

            if (cur < next)
                num -= cur;   // 小的在大数的左边 → 减
            else
                num += cur;   // 正常情况 → 加
        }

        return num;
    }
};

// ==================== 以下为测试代码，不属于题目答案 ====================

struct TestCase {
    string roman;
    int expected;
};

int main() {
    vector<TestCase> tests = {
        {"III", 3},
        {"LVIII", 58},
        {"MCMXCIV", 1994},
        {"IV", 4},
        {"IX", 9},
        {"XL", 40},
        {"XC", 90},
        {"CD", 400},
        {"CM", 900},
        {"MMXXIV", 2024},
    };

    Solution sol;
    int passed = 0;

    for (const auto& t : tests) {
        int got = sol.romanToInt(t.roman);
        bool ok = (got == t.expected);
        if (ok) ++passed;

        cout << (ok ? "[PASS] " : "[FAIL] ")
             << "romanToInt(\"" << t.roman << "\") = " << got
             << ", 期望 = " << t.expected << '\n';
    }

    cout << "\n通过 " << passed << " / " << tests.size() << " 个测试用例\n";

    return 0;
}

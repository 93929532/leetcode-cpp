#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>


using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    bool isAnagram(string s, string t) {

        array<int, 26> cnts{}, cntt{};
        
        for(char c : s) cnts[c - 'a']++;
        for(char c : t) cntt[c - 'a']++;

        for(int i = 0; i < 26; i++) {
            if(cnts[i] != cntt[i])
                return false;
        }

        return true;  
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase {
    string name;      // 用例名称
    string s;         // 输入 s
    string t;         // 输入 t
    bool expected;    // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    Solution sol;
    bool actual = sol.isAnagram(c.s, c.t);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  s = \"" << c.s << "\""
         << "  t = \"" << c.t << "\""
         << "  期望: " << boolalpha << c.expected
         << "  实际: " << actual << noboolalpha
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");  // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "anagram", "nagaram", true},
        {"示例 2", "rat", "car", false},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"完全相同", "abc", "abc", true},
        {"单字符相同", "a", "a", true},
        {"单字符不同", "a", "b", false},
        {"长度不同（短在前）", "ab", "a", false},
        {"长度不同（长在前）", "a", "ab", false},
        {"字母相同但数量不同", "aab", "abb", false},
        {"相同字母不同顺序", "listen", "silent", true},
        {"字母表全用一遍", "abcdefghijklmnopqrstuvwxyz", "zyxwvutsrqponmlkjihgfedcba", true},
        {"只差一个字符的顺序", "abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxyy", false},
        {"有重复字母的异位词", "aacc", "ccaa", true},
        {"重复字母交叉排列", "aacc", "acac", true},
        {"首尾字母互换", "az", "za", true},
        {"两边同样多但字母不同", "aabb", "ccdd", false},
    };

    size_t passed = 0;
    size_t total = 0;

    cout << "================ 题目示例 ================" << endl;
    for (const auto& c : sampleCases) {
        ++total;
        if (runCase(c)) ++passed;
    }

    cout << endl << "================ 补充用例 ================" << endl;
    for (const auto& c : extraCases) {
        ++total;
        if (runCase(c)) ++passed;
    }

    cout << endl << "================ 结果统计 ================" << endl;
    cout << "共 " << total << " 个用例，通过 " << passed
         << " 个，失败 " << (total - passed) << " 个。" << endl;
    cout << (passed == total ? "全部通过 ✔" : "存在失败用例 ✘") << endl;

    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    bool checkInclusion(string s1, string s2)
    {
        // TODO: 在这里写你的解法

        if (s1.size() > s2.size())
            return false;

        vector<int> need(26, 0), cnt(26, 0);

        for (int i = 0; i < s1.size(); i++)
        {
            need[s1[i] - 'a']++;
            cnt[s2[i] - 'a']++;
        }

        if (cnt == need)
            return true;

        for (size_t left = 0, right = s1.size(); right < s2.size(); left++, right++)
        {
            cnt[s2[right] - 'a']++;
            cnt[s2[left] - 'a']--;

            if (cnt == need)
                return true;
        }

        return false;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase
{
    string name;   // 用例名称
    string s1;     // 要找的排列（模式串）
    string s2;     // 被搜索的长串
    bool expected; // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
// 注意：题目签名是 string s1, string s2（都按值传递），解法拿到的是副本
static bool runCase(const TestCase &c)
{
    Solution sol;
    bool actual = sol.checkInclusion(c.s1, c.s2);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  s1 = \"" << c.s1 << "\", s2 = \"" << c.s2 << "\""
         << "  期望: " << boolalpha << c.expected
         << "  实际: " << actual << noboolalpha
         << endl;
    return pass;
}

int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul"); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "ab", "eidbaooo", true},
        {"示例 2", "ab", "eidboaoo", false},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"s1 比 s2 长", "abc", "ab", false},
        {"s1 等于 s2", "ab", "ab", true},
        {"单字符但不存在", "a", "bbb", false},
        {"单字符且存在", "a", "ba", true},
        {"排列在开头", "ab", "abxxxx", true},
        {"排列在结尾", "ab", "xxxxab", true},
        {"窗口必须恰好等长", "abc", "aabbcc", false},
        {"s1 内部有重复字符", "aab", "abaa", true},
        {"全是同一个字符", "aaa", "aaaa", true},
        {"长度够但字符对不上", "abcd", "abc", false},
        {"整个 s2 就是一个排列", "abc", "cba", true},
        {"错一格也要算命中", "abc", "abcc", true},
        {"26 个字母的逆序排列",
         "abcdefghijklmnopqrstuvwxyz", "zyxwvutsrqponmlkjihgfedcba", true},
    };

    size_t passed = 0;
    size_t total = 0;

    cout << "================ 题目示例 ================" << endl;
    for (const auto &c : sampleCases)
    {
        ++total;
        if (runCase(c))
            ++passed;
    }

    cout << endl
         << "================ 补充用例 ================" << endl;
    for (const auto &c : extraCases)
    {
        ++total;
        if (runCase(c))
            ++passed;
    }

    cout << endl
         << "================ 结果统计 ================" << endl;
    cout << "共 " << total << " 个用例，通过 " << passed
         << " 个，失败 " << (total - passed) << " 个。" << endl;
    cout << (passed == total ? "全部通过 ✔" : "存在失败用例 ✘") << endl;

    return 0;
}

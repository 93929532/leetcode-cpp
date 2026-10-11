#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    int characterReplacement(string s, int k)
    {
        // TODO: 在这里写你的解法

        vector<int> count(26, 0);
        int best = 0;

        for (int left = 0, right = 0, maxCnt = 0; right < (int)s.size(); right++)
        {
            count[s[right] - 'A']++;
            maxCnt = max(maxCnt, count[s[right] - 'A']);

            if (right - left + 1 - maxCnt > k)
            {
                count[s[left] - 'A']--;
                left++;
            }

            best = right - left + 1;
        }

        return best;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase
{
    string name;  // 用例名称
    string s;     // 输入字符串（只有大写字母）
    int k;        // 最多可替换的字符数
    int expected; // 期望的最长长度
};

// 运行一条用例，打印一行信息，返回该用例是否通过
// 注意：题目签名是 string s（按值传递），解法拿到的是副本，不会影响这里的 c.s
static bool runCase(const TestCase &c)
{
    Solution sol;
    int actual = sol.characterReplacement(c.s, c.k);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  s = \"" << c.s << "\", k = " << c.k
         << "  期望: " << c.expected
         << "  实际: " << actual
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
        {"示例 1", "ABAB", 2, 4},
        {"示例 2", "AABABBA", 1, 4},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"单字符", "A", 0, 1},
        {"全相同且 k=0", "AAAA", 0, 4},
        {"交替且 k=0", "ABAB", 0, 1},
        {"两个大块且 k=0", "AAAAABBBBB", 0, 5},
        {"k 大到能全换", "ABCDE", 4, 5},
        {"k 等于串长", "ABCDE", 5, 5},
        {"k 超过串长", "ABC", 10, 3},
        {"答案在中间", "BAAAB", 2, 5},
        {"需要真正收缩窗口", "ABABAB", 1, 3},
        {"三个块", "AAABBBAAA", 1, 4},
        {"k 几乎够全长", "ABCDEFGHIJ", 9, 10},
        {"四字符重复", "AABBAABB", 1, 3},
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

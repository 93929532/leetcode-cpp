#include <iostream>
#include <string>
#include <vector>
#include <algorithm>     // min：更新最优窗口时会用到
#include <climits>       // INT_MAX：记录最优长度
#include <unordered_map> // 坑 6 提到的替代方案：unordered_map<char,int>

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    string minWindow(string s, string t)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   滑动窗口的骨架你早就见过了（3 无重复字符的最长子串）：**右指针扩张、左指针收缩**。
        //   但本题的难点不在骨架，而在两个新问题：
        //     a) 怎么知道"当前窗口已经覆盖了 t 的全部字符"？
        //     b) 什么时候左指针**可以**收缩、什么时候必须停？
        //
        //   思路一 · 暴力（O(m² · n) 甚至更差，只说思路）
        //     枚举所有 O(m²) 个子串，每个再统计频次和 t 比。面试里明确说"这个不够"，
        //     然后顺势引出滑窗 —— 这是标准开场。
        //
        //   思路二 · 滑动窗口 + 频率表 + required 计数器（标准解，O(m + n)）
        //     need[c] 表示"窗口还差几个字符 c"；required 表示"一共还差多少个字符"（含重复）。
        //     先用 t 初始化：need[c]++，required = t.size()。
        //
        //     扩张（右指针 r 扫过 s）：
        //         if (need[s[r]] > 0) --required;   // 这个字符是"欠着的"，补上一个
        //         --need[s[r]];                     // 不管有没有用都减：负数表示窗口里多出来了
        //
        //     收缩（只在 required == 0，即窗口已覆盖 t 时做）：
        //         while (need[s[l]] < 0) {          // 左端字符是多余的，可以扔掉
        //             ++need[s[l]];
        //             ++l;
        //         }
        //         // 此刻 [l, r] 是一个合法窗口：用 r - l + 1 更新最优答案（并记下起点）
        //
        //     为什么是 O(m)：r 从 0 走到 m-1 一遍，l 也只向右走一遍，合计 O(m)；
        //     再加上统计 t 的 O(n)。每个字符最多被左右指针各访问一次。
        //
        //   思路三 · 先过滤再滑窗（官方优化）
        //     预处理出 filtered_s：只保留 s 中"出现在 t 里"的字符**及其原始下标**。
        //     窗口只在有用字符之间滑动，省掉大量无用字符的比较。
        //     代价是多一次 O(m) 预处理和 O(m) 空间，s 很长而 t 很短时收益明显。
        //
        //   七个坑：
        //     1. **required 要数「字符总数（含重复）」，不是「不同字符数」**。
        //        t = "AAB" 不能记成 2 个不同字符，必须记成 3 —— 这是最常见的错法。
        //     2. **判断"窗口已覆盖"别去遍历 128 位数组**。每次 O(128) 虽不慢，
        //        但维护一个 required 计数器能把它降成 O(1)，这是本题最主要的加分点。
        //     3. **收缩的条件是 need[s[l]] < 0（多余），不是 <= 0**。
        //        need == 0 表示这个字符刚好用完，再移走窗口就缺字符了 ——
        //        写成 <= 0 会得到偏短的**非法**窗口（覆盖不全），而且小数据未必暴露。
        //     4. **返回的是子串，不是长度**：要同时记最长/最短长度和起始下标。
        //     5. **无解返回空串**：若 t 里有 s 中不存在的字符，required 永远到不了 0，
        //        循环结束后自然返回 ""。
        //     6. **字符集只有大小写字母 → 开 int need[128] 就够**，而且比
        //        unordered_map<char,int> 快得多（少了哈希与节点分配）。本条会实测给你看。
        //     7. **窗口长度是 r - l + 1**（闭区间），别写成 r - l。
        //
        //   进阶：
        //     · 同族题：567 字符串的排列（窗口长度固定，判断 t 的排列是否出现）、
        //       438 找到字符串中所有字母异位词、3 无重复字符的最长子串（你已建）、
        //       424 替换后的最长重复字符（你计划里序号 17，窗口条件换成"最多改 k 个"）
        //     · 「need 允许为负 + required 计数器」是通用套路：
        //       凡是"窗口要满足某种字符配额"的题都能套
        //     · 若字符集扩展到 Unicode，need 得换成哈希表（下标不再是 128）

        unordered_map<char, int> sCnt, tCnt;

        for (char c : t)
            tCnt[c]++;

        int left, right, bestL = 0, bestLen = 0;
        for (left = 0, right = 0; right < (int)s.size(); right++)
        {
            sCnt[s[right]]++;

            while (check(sCnt, tCnt)) // 只要还覆盖得住，就一直缩
            {
                int len = right - left + 1;

                if (bestLen == 0 || len < bestLen)
                {
                    bestLen = len;
                    bestL = left;
                } // 只记下标，不建串

                sCnt[s[left]]--;
                left++;
            }
        }
        return bestLen == 0 ? "" : s.substr(bestL, bestLen); // 最后裁一次
    }

private:
    bool check(const unordered_map<char, int> &sCnt, const unordered_map<char, int> &tCnt)
    {
        for (const auto &[c, cnt] : tCnt)
        {
            if (sCnt.find(c) == sCnt.end() || sCnt.at(c) < cnt)
                return false;
        }
        return true;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 带引号打印字符串；过长只打印前 limit 个字符，避免刷屏
static string quoteTruncated(const string &s, size_t limit = 60)
{
    if (s.size() <= limit)
        return "\"" + s + "\"";
    return "\"" + s.substr(0, limit) + "…\"（共 " + to_string(s.size()) + " 字符）";
}

// 单条测试用例
//
// 注意：题目保证答案唯一，所以这里可以直接精确比较字符串。
//       （如果用例设计得不好、存在多个等长合法窗口，不同实现可能返回不同的那个。）
struct TestCase
{
    string name;     // 用例名称
    string s;        // 源串
    string t;        // 目标串（要覆盖的字符集合，含重复）
    string expected; // 期望的最小子串（无解时为空串）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    Solution sol;
    string got = sol.minWindow(c.s, c.t); // 签名是按值传递，用例数据不会被改动

    bool pass = (got == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  s = " << quoteTruncated(c.s)
         << "  t = " << quoteTruncated(c.t, 20) << "\n"
         << "       期望: " << quoteTruncated(c.expected)
         << "   实际: " << quoteTruncated(got) << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "ADOBECODEBANC", "ABC", "BANC"},
        {"示例 2", "a", "a", "a"},
        {"示例 3（t 里有 s 没有的字符）", "a", "aa", ""},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"t 比 s 长", "ab", "abc", ""},
        {"s 里缺一个字符", "abc", "abd", ""},
        {"s 与 t 完全相同", "ab", "ab", "ab"},
        {"窗口在开头", "abcde", "ab", "ab"},
        {"窗口在结尾", "xxabc", "abc", "abc"},
        {"单个字符出现多次", "abac", "b", "b"},
        {"重复需求恰好用完整个 s", "AAB", "AAB", "AAB"},
        {"重复需求 + 前面多个多余的 A", "AAAAAAB", "AAB", "AAB"},
        {"大小写敏感（正例：'Ab' 命中）", "aAbB", "Ab", "Ab"},
        {"大小写敏感（无解：需要两个 A）", "aA", "AA", ""},
        {"经典长例", "cabwefgewcwaefgcf", "cae", "cwae"},
    };

    // 最大规模用例：m = 10^5（题目约束上限）
    //   99999 个 'a' + 一个 'b'，t = "ab"：唯一的最短窗口就是最后两个字符 "ab"
    //   这条用例专门压 O(m²) 的实现（滑窗是 O(m)，暴力是 10^10 量级，跑不动）
    {
        string s(99999, 'a');
        s += 'b';
        extraCases.push_back({"最大规模 m=10^5（99999 个 a + 一个 b）", s, "ab", "ab"});
    }

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

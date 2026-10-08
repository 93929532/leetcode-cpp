#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    int strStr(string haystack, string needle) {

        int i, j;

        for (i = 0; i < haystack.size(); i++) {
            for (j = 0; j < needle.size(); j++) {
                if ((i + j) == haystack.size() || haystack[i + j] != needle[j])
                    break;
            }
            if (j == needle.size())
                return i;
        }

        return -1;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase {
    string name;      // 用例名称
    string haystack;  // 输入串
    string needle;    // 模式串
    int expected;     // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    Solution sol;
    int actual = sol.strStr(c.haystack, c.needle);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  haystack = \"" << c.haystack << "\""
         << "  needle = \"" << c.needle << "\""
         << "  期望: " << c.expected
         << "  实际: " << actual
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "sadbutsad", "sad", 0},
        {"示例 2", "leetcode", "leeto", -1},
    };

    // 额外的边界与补充用例
    // 注意：题目约束 1 <= haystack.length，空 haystack 不在约束范围内，故不设该用例
    //      （现有解法在外层循环不执行时会返回 -1）
    const vector<TestCase> extraCases = {
        {"空 needle 视为命中 0",  "abc", "", 0},
        {"单字符命中",            "a", "a", 0},
        {"命中在末尾",            "abc", "c", 2},
        {"needle 比 haystack 长", "aaa", "aaaa", -1},
        {"需要在中途重新起步",    "mississippi", "issip", 4},
        {"相邻重复字符",          "hello", "ll", 2},
        {"完全不匹配",            "abcdef", "gh", -1},
        {"重复前缀取最左匹配",    "aaaaa", "aaa", 0},
        {"needle 等于 haystack",  "abcd", "abcd", 0},
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

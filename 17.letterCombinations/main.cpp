#include <iostream>
#include <string>
#include <vector>
#include <algorithm>   // std::sort：答案顺序无关，比较前统一排序

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    vector<string> letterCombinations(string digits) {

        vector<string> mapping = {"abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
        vector<string> result;

        for (char d : digits)
        {
            int num = d - '2';   // 将字符转换为对应的数字索引

            if (num < 0 || num >= static_cast<int>(mapping.size()))
            {
                return {};   // 如果输入的数字不在 2-9 范围内，返回空结果
            }

            if (result.empty())   // 结果集还空着，说明当前是第一个数字
            {
                for (char c : mapping[num])
                {
                    result.push_back(string(1, c));   // 每个字母各自成为一个组合
                }
            }
            else
            {
                vector<string> temp;

                for (const string& prev : result)   // 遍历已有的每个组合
                {
                    for (char c : mapping[num])   // 遍历新数字对应的每个字母
                    {
                        temp.push_back(prev + c);   // 组合当前结果集与新数字对应的字母
                    }
                }

                result = temp;   // 更新结果集
            }
        }
        return result;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把组合结果排序后拼成 "ad,ae,af" 形式的字符串
// 题目允许答案按任意顺序返回，所以比较前先统一排序
static string joinSorted(vector<string> v) {
    sort(v.begin(), v.end());

    string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) s += ",";
        s += v[i];
    }
    return s;
}

// 单条测试用例
struct TestCase {
    string name;              // 用例名称
    string digits;            // 输入数字串
    vector<string> expected;  // 期望结果（顺序无关）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    Solution sol;
    vector<string> actual = sol.letterCombinations(c.digits);

    const string got  = joinSorted(actual);
    const string want = joinSorted(c.expected);

    bool pass = (got == want);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  digits = \"" << c.digits << "\""
         << "  期望: " << (want.empty() ? "(空)" : want)
         << "  实际: " << (got.empty() ? "(空)" : got)
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "23", {"ad", "ae", "af", "bd", "be", "bf", "cd", "ce", "cf"}},
        {"示例 2", "2",  {"a", "b", "c"}},
        {"示例 3", "",   {}},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"单个数字 7", "7", {"p", "q", "r", "s"}},
        {"单个数字 9", "9", {"w", "x", "y", "z"}},
        {"7 与 9 组合（4×4）", "79",
         {"pw", "px", "py", "pz", "qw", "qx", "qy", "qz",
          "rw", "rx", "ry", "rz", "sw", "sx", "sy", "sz"}},
        {"2 与 9 组合（3×4）", "29",
         {"aw", "ax", "ay", "az", "bw", "bx", "by", "bz", "cw", "cx", "cy", "cz"}},
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

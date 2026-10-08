#include <iostream>
#include <vector>
#include <string>
#include <climits>   // INT_MIN / INT_MAX
#include <cstdlib>   // std::abs(long long)

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    int divide(int dividend, int divisor) {

        if (dividend == INT_MIN && divisor == -1) return INT_MAX;   // 先把唯一溢出点挡掉

        bool negative = (dividend < 0) ^ (divisor < 0);
        long long a = abs(static_cast<long long>(dividend));
        long long b = abs(static_cast<long long>(divisor));

        long long ans = 0;
        while (a >= b) {
            long long t = b, cnt = 1;
            while (t <= a - t) { t += t; cnt += cnt; }   // 倍增：找最大 2^k 倍
            a -= t;
            ans += cnt;
        }

        return static_cast<int>(negative ? -ans : ans);
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase {
    string name;      // 用例名称
    int dividend;     // 被除数
    int divisor;      // 除数
    int expected;     // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    Solution sol;
    int actual = sol.divide(c.dividend, c.divisor);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  " << c.dividend << " / " << c.divisor
         << "  期望: " << c.expected
         << "  实际: " << actual
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例（示例 1、示例 2）
    const vector<TestCase> sampleCases = {
        {"示例 1", 10, 3, 3},
        {"示例 2", 7, -3, -2},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"被除数为 0",         0, 1, 0},
        {"同号相除",           -1, -1, 1},
        {"INT_MIN 除以 1",     INT_MIN, 1, INT_MIN},
        {"唯一的溢出点",       INT_MIN, -1, INT_MAX},
        {"INT_MAX 除以 -1",    INT_MAX, -1, -INT_MAX},
        {"INT_MIN 除以 2",     INT_MIN, 2, -1073741824},
        {"除数为 1",           -7, 1, -7},
        {"被除数小于除数",     1, 2, 0},
        {"不能整除向下取整",   100, 3, 33},
        {"除数与被除数同为正", 2147483647, 2, 1073741823},
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

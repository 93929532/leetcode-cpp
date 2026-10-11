#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    int maxProfit(vector<int> &prices)
    {
        // TODO: 在这里写你的解法

        int slow, fast;
        int best = 0;

        for (slow = 0, fast = 0; fast < (int)prices.size(); fast++)
        {
            int value = prices[fast] - prices[slow];
            if (value > best)
                best = value;

            if (prices[fast] < prices[slow])
                slow = fast;
        }

        return best;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 "[7, 1, 5, 3, 6, 4]" 形式
static string toString(const vector<int> &nums)
{
    string s = "[";
    for (size_t i = 0; i < nums.size(); ++i)
    {
        if (i > 0)
            s += ", ";
        s += to_string(nums[i]);
    }
    s += "]";
    return s;
}

// 单条测试用例
struct TestCase
{
    string name;        // 用例名称
    vector<int> prices; // 输入：每日股价
    int expected;       // 期望的最大利润
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    // 题目签名是 vector<int>&，这里复制一份，避免解法改动输入影响结果打印
    vector<int> prices = c.prices;

    Solution sol;
    int actual = sol.maxProfit(prices);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  prices = " << toString(c.prices)
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
        {"示例 1", {7, 1, 5, 3, 6, 4}, 5},
        {"示例 2", {7, 6, 4, 3, 1}, 0},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"单元素", {5}, 0},
        {"两个元素上升", {1, 2}, 1},
        {"两个元素下降", {2, 1}, 0},
        {"价格全相同", {3, 3, 3, 3}, 0},
        {"单调上涨", {1, 2, 3, 4, 5}, 4},
        {"单调下跌", {5, 4, 3, 2, 1}, 0},
        {"谷底在中间", {3, 8, 1, 4}, 5},
        {"最低价在最高价之后", {5, 10, 1}, 5},
        {"先跌后涨", {10, 1, 5, 6, 7, 8}, 7},
        {"只有一次上涨", {2, 4, 1}, 2},
        {"达到取值上界", {0, 10000}, 10000},
        {"上界但一路下跌", {10000, 0}, 0},
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

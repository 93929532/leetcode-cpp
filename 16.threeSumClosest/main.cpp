#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>   // std::abs(int) 等整数重载

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {

        sort(nums.begin(), nums.end());

        const int n = static_cast<int>(nums.size());
        int ans = nums[0] + nums[1] + nums[2];

        for (int i = 0; i < n - 2; i++)
        {
            // 跳过重复的固定元素，避免同一组组合被反复计算
            if (i > 0 && nums[i] == nums[i - 1]) continue;

            int L = i + 1, R = n - 1;

            while (L < R)
            {
                int sum = nums[i] + nums[L] + nums[R];
                if (abs(sum - target) < abs(ans - target))
                {
                    ans = sum;
                }

                if (sum < target)
                {
                    L++;
                }
                else if (sum > target)
                {
                    R--;
                }
                else
                {
                    // 恰好等于 target，已是理论最优（题目保证唯一解）
                    return sum;
                }
            }
        }
        return ans;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [-4,-1,1,2] 这样的字符串
static string vectorToString(const vector<int>& nums) {
    string s = "[";
    for (size_t i = 0; i < nums.size(); ++i) {
        if (i > 0) s += ",";
        s += to_string(nums[i]);
    }
    s += "]";
    return s;
}

// 单条测试用例
struct TestCase {
    string name;        // 用例名称
    vector<int> nums;   // 输入数组
    int target;         // 目标和
    int expected;       // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 解法会原地排序，先拷贝一份，保留原始输入用于打印

    Solution sol;
    int actual = sol.threeSumClosest(nums, c.target);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << vectorToString(c.nums)
         << "  target = " << c.target
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
        {"示例 1", {-1, 2, 1, -4}, 1, 2},
        {"示例 2", {0, 0, 0}, 1, 0},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"含 0 的四元素",       {1, 1, 1, 0}, -100, 2},
        {"含重复负数",          {1, 1, -1, -1, 3}, -1, -1},
        {"三个数全相同",        {-1000, -1000, -1000}, 10000, -3000},
        {"target 恰为一组解",   {1, 2, 3, 4, 5}, 6, 6},
        {"全零且 target 偏大",  {0, 0, 0, 0}, 5, 0},
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

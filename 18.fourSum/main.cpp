#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    vector<vector<int>> fourSum(vector<int>& nums, int target) {

        vector<vector<int>> result = {};

        int n = (int)nums.size();
        if (n < 4)
            return result;

        sort(nums.begin(), nums.end());

        for (int i = 0; i < n - 3; i++) {
            for (int j = i + 1; j < n - 2; j++) {

                int L = j + 1, R = n - 1;

                while (L < R) {

                    // 必须用 long long: 四数之和最大 4e9, 超出 int 上限会溢出
                    long long sum = 0LL + nums[i] + nums[j] + nums[L] + nums[R];

                    if (sum < target) {
                        L++;
                    }
                    else if (sum > target) {
                        R--;
                    }
                    else {
                        result.push_back({nums[i], nums[j], nums[L], nums[R]});
                        L++;
                        R--;
                    }
                }
            }
        }

        sort(result.begin(), result.end());
        auto newEnd = unique(result.begin(), result.end());
        result.erase(newEnd, result.end());
        return result;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [-1,0,1] 这样的字符串
static string vectorToString(const vector<int>& nums) {
    string s = "[";
    for (size_t i = 0; i < nums.size(); ++i) {
        if (i > 0) s += ",";
        s += to_string(nums[i]);
    }
    s += "]";
    return s;
}

// 把结果格式化成 [[-2,-1,1,2],[-2,0,0,2]] 这样的字符串
static string toString(const vector<vector<int>>& res) {
    string s = "[";
    for (size_t i = 0; i < res.size(); ++i) {
        s += vectorToString(res[i]);
        if (i + 1 < res.size()) s += ",";
    }
    return s + "]";
}

// 四元组内部先排序，再对整体排序，便于与期望结果比较（题目不要求顺序）
static void normalize(vector<vector<int>>& res) {
    for (auto& v : res) sort(v.begin(), v.end());
    sort(res.begin(), res.end());
}

// 单条测试用例
struct TestCase {
    string name;                   // 用例名称
    vector<int> nums;              // 输入数组
    int target;                    // 目标和
    vector<vector<int>> expected;  // 期望结果（顺序无关）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 解法会原地排序，先拷贝一份，保留原始输入用于打印

    Solution sol;
    vector<vector<int>> actual = sol.fourSum(nums, c.target);

    vector<vector<int>> want = c.expected;
    normalize(actual);
    normalize(want);

    bool pass = (actual == want);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << vectorToString(c.nums)
         << "  target = " << c.target
         << "  期望: " << toString(want)
         << "  实际: " << toString(actual)
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 0, -1, 0, -2, 2}, 0,
         {{-2, -1, 1, 2}, {-2, 0, 0, 2}, {-1, 0, 0, 1}}},
        {"示例 2", {2, 2, 2, 2, 2}, 8,
         {{2, 2, 2, 2}}},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"无解",              {1, 2, 3, 4}, 100, {}},
        {"元素不足 4 个",     {1, 2, 3}, 6, {}},
        {"四个 0 命中",       {0, 0, 0, 0}, 0, {{0, 0, 0, 0}}},
        {"连续递增取前四个",  {1, 2, 3, 4, 5}, 10, {{1, 2, 3, 4}}},
        {"含重复值的多解",    {-2, -1, 0, 0, 1, 2}, 0,
         {{-2, -1, 1, 2}, {-2, 0, 0, 2}, {-1, 0, 0, 1}}},
        // 溢出用例：四个 1e9 相加 = 4e9，超过 int 上限 (2147483647)
        // 4e9 溢出后的结果恰好等于 -294967296，若用 int 累加会误判为命中
        {"四数溢出(LeetCode 用例)", {1000000000, 1000000000, 1000000000, 1000000000}, -294967296, {}},
        {"四数溢出(target=0)",      {1000000000, 1000000000, 1000000000, 1000000000}, 0, {}},
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

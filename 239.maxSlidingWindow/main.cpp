#include <iostream>
#include <string>
#include <vector>
#include <deque>       // 单调队列用它；思路二/三也用得到
#include <queue>       // 思路三的替代方案：大顶堆
#include <algorithm>   // max：解法与测试骨架都可能用到

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   暴力很好写：枚举 n-k+1 个窗口，每个扫 k 个元素取最大 —— O(n·k)。
        //   本题的考点是把它降到 O(n)，而做到这件事的数据结构叫**单调队列**。
        //
        //   思路一 · 暴力 O(n·k)
        //     面试开场说出来，确认题意，然后说"窗口每滑一格都重扫一遍太浪费，
        //     因为相邻两个窗口有 k-1 个元素是重合的"。
        //
        //   思路二 · 单调队列（deque）O(n)，本题标准解
        //     维护一个**存下标**的双端队列 dq，队列里对应的值**单调递减**。
        //     对每个 i 依次做四件事：
        //         ① 弹队尾：while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
        //            —— 这些元素比新来的小（或相等），只要 nums[i] 还在窗口里，
        //               它们就永远不可能成为最大值，可以直接扔掉。
        //         ② 入队：dq.push_back(i);
        //         ③ 弹队首（过期）：if (dq.front() <= i - k) dq.pop_front();
        //            —— 队首下标已经滑出窗口 [i-k+1, i] 了。
        //         ④ 当 i >= k-1 时，nums[dq.front()] 就是当前窗口最大值，收进答案。
        //     为什么是 O(n)：每个下标最多被 push 一次、pop 一次，
        //     所以 ①③ 的总操作次数是 O(n)，不是每步 O(k)。
        //
        //   思路三 · 大顶堆（优先队列）O(n log n)，好写但更慢
        //     堆里存 {值, 下标}，每步 push 当前元素，然后把"堆顶已过期"的弹掉，
        //     堆顶就是答案。正确性显然，代价是 log 因子 + 懒惰删除。
        //
        //   思路四 · 分块前后缀最大值 O(n)，不用 deque 的技巧
        //     把数组按 k 个一块切开，预处理"每块内的前缀最大值"和"后缀最大值"。
        //     任意长度为 k 的窗口，一定横跨两个相邻块，答案是
        //         max(左块的**后缀**最大值, 右块的**前缀**最大值)
        //     这是稀疏表/分块的思想，能在"不许用 deque"的追问下用出来。
        //
        //   八个坑：
        //     1. **队列里存下标，不是值**。存了值就没法判断"是否滑出窗口"了 —— 这是本题第一号坑。
        //     2. **过期判断是 `dq.front() <= i - k`**（注意是 `<=`）：
        //        窗口是 [i-k+1, i]，下标 i-k 已经出界了。
        //     3. **四步的顺序不能乱**：先弹队尾（维护单调）、再入队、再弹过期、最后取答案。
        //        尤其是"弹过期"必须在取答案之前。
        //     4. **结果个数是 n-k+1**：只在 `i >= k-1` 时才往答案里放，
        //        所以最后答案数组的长度是 n-k+1，不是 n。
        //     5. **弹队尾用 `<=` 还是 `<`**：用 `<=` 会把相等的旧元素也弹掉（队列里值严格递减），
        //        用 `<` 则保留相等元素（队列里非严格递减）。**两种都对**，
        //        因为相等时留新的更"耐用"（更晚过期）。别在这里纠结太久。
        //     6. **k = 1** 结果是原数组；**k = n** 结果只有一个元素（整个数组的最大值）。
        //        这两条边界用同一份代码都能过，值得各测一遍。
        //     7. **`deque` 的 front/back 前必须判空**，否则空 deque 上取值是 UB。
        //     8. **空数组**（约束 n ≥ 1，不会出现）：`i - k` 之类的计算要小心无符号下溢。

        vector<int> res;

        int left, right;
        for (left = 0, right = k; right < nums.size() - 1; left++, right++)
        {
            int best = nums[left], i = left;
            while (i != right)
            {
                best = max(best, nums[i]);
                i++;
            }

            res.push_back(best);
        }

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [3,3,5,5,6,7] 便于打印；过长的数组只打印首 6 末 3
static string vectorToString(const vector<int>& v) {
    if (v.size() <= 12) {
        string s = "[";
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) s += ",";
            s += to_string(v[i]);
        }
        return s + "]";
    }

    string s = "[";
    for (int i = 0; i < 6; ++i) {
        if (i) s += ",";
        s += to_string(v[i]);
    }
    s += ", …(略去 " + to_string(v.size() - 9) + " 个)…, ";
    for (size_t i = v.size() - 3; i < v.size(); ++i) {
        if (i != v.size() - 3) s += ",";
        s += to_string(v[i]);
    }
    return s + "]";
}

// 单条测试用例
//
// 注意：本题输出是**按下标一一对应**的（第 j 个输出对应第 j 个窗口），
//       顺序有意义，所以直接精确比较，不能排序后比。
struct TestCase {
    string name;          // 用例名称
    vector<int> nums;     // 输入数组
    int k;                // 窗口大小
    vector<int> expected; // 期望输出（长度应为 nums.size() - k + 1）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    vector<int> actual = sol.maxSlidingWindow(nums, c.k);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  k = " << c.k
         << "  nums = " << vectorToString(c.nums) << "\n"
         << "       期望: " << vectorToString(c.expected)
         << "\n       实际: " << vectorToString(actual) << endl;

    if (!pass && actual.size() != c.expected.size()) {
        cout << "       （输出个数是 " << actual.size()
             << "，应为 " << c.expected.size() << " = n - k + 1）" << endl;
    }
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 3, -1, -3, 5, 3, 6, 7}, 3, {3, 3, 5, 5, 6, 7}},
        {"示例 2", {1}, 1, {1}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"k = 1（结果就是原数组）",       {1, -1, 3, 2}, 1, {1, -1, 3, 2}},
        {"k = n（只有一个输出）",         {5, 2, 9, 1}, 4, {9}},
        {"两个元素 k = 2",                {2, 1}, 2, {2}},
        {"全部相同",                      {3, 3, 3, 3}, 2, {3, 3, 3}},
        {"单调递增",                      {1, 2, 3, 4, 5}, 2, {2, 3, 4, 5}},
        {"单调递减",                      {5, 4, 3, 2, 1}, 2, {5, 4, 3, 2}},
        {"全负数",                        {-5, -2, -8, -1}, 2, {-2, -2, -1}},
        {"含重复的最大值",                {1, 3, 3, 1}, 2, {3, 3, 3}},
        {"最大值从窗口左侧滑出",          {9, 1, 1, 1, 1, 8}, 3, {9, 1, 1, 8}},
        {"约束边界的极值 ±10^4",          {-10000, 10000, -10000, 10000}, 3, {10000, 10000}},
        {"最小值在窗口里",                {0, -10000, 0, 0}, 2, {0, 0, 0}},
    };

    // 最大规模用例 1：n = 10^5、k = n/2，数组严格递增
    //   递增时每个窗口的最大值就是最右边的元素 -> 期望输出 = {k, k+1, ..., n}
    //   这条用例专门压 O(n·k) 的暴力实现（5×10^9 次比较，跑不动）
    {
        const int n = 100000, k = 50000;
        vector<int> nums;
        nums.reserve(n);
        for (int v = 1; v <= n; ++v) nums.push_back(v);

        vector<int> expected;
        expected.reserve(n - k + 1);
        for (int i = k - 1; i < n; ++i) expected.push_back(nums[i]);

        extraCases.push_back({"最大规模 n=10^5, k=50000（严格递增）", nums, k, expected});
    }

    // 最大规模用例 2：n = 10^5、k = n/2，全部元素相同
    //   专门验证"相等元素该不该弹出队尾"这个细节在大数据下依然正确
    {
        const int n = 100000, k = 50000;
        vector<int> nums(n, 7);
        vector<int> expected(n - k + 1, 7);
        extraCases.push_back({"最大规模 n=10^5, k=50000（全部相同）", nums, k, expected});
    }

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

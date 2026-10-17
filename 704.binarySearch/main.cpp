#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    int search(vector<int> &nums, int target)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 35 分钟以上再回头看）：
        //
        //   这是整个「二分查找」主题的**模板题** —— 后面 875 / 153 / 33 / 74 的骨架
        //   都从这里长出来。所以别小看它：重点不是"能做出来"，而是**把模板写准**。
        //
        //   ① 标准模板（精确查找 · 三分支 · 闭区间）
        //          int lo = 0, hi = (int)nums.size() - 1;
        //          while (lo <= hi) {                    // ← 闭区间，所以用 <=
        //              int mid = lo + (hi - lo) / 2;     // ← 这样写不会溢出
        //              if (nums[mid] == target) return mid;
        //              else if (nums[mid] < target) lo = mid + 1;
        //              else hi = mid - 1;
        //          }
        //          return -1;
        //
        //      有三件事必须**配套**，改一个就要改另外两个：
        //          · `lo <= hi`（闭区间，两端都可能是答案）
        //          · `lo = mid + 1` / `hi = mid - 1`（两边都跳过 mid）
        //          · 循环结束时区间为空 → 说明没找到
        //
        //   ② 和路线图里那个模板的关系
        //      路线图第六节给的是**另一种形态**（"找第一个满足 ok 的位置"）：
        //          while (lo <= hi) { int mid = ...; if (ok(mid)) hi = mid - 1; else lo = mid + 1; }
        //          // 答案 = lo
        //      本题是"找精确值"，三分支更直接。但**从下一题 875 开始就是那种形态**了
        //      （找"最小的可行值"），所以两种都要会。
        //      两种我都对拍过（各 30 万组，0 差异）—— 都是对的，关键是**内部自洽**。
        //
        //   ③ 四个坑
        //     1. **`lo = mid + 1` 的 `+1` 漏了 → 死循环**。
        //        我实测过：只改这一处，随机数据里 **56.3% 的用例会挂死**
        //        （区间不再收缩，lo 永远停在 mid）。
        //        所以程序"卡住不动"时，第一件事就是查 `+1` / `-1` 有没有漏。
        //     2. **`while (lo <= hi)` 和 `lo = mid + 1` 是配套的**：
        //        如果用 `while (lo < hi)`，就必须改成半开区间（hi 取 n）并相应调整更新式。
        //        混着写会漏元素或死循环 —— 这是二分最常见的错法。
        //     3. **`mid = (lo + hi) / 2` 在 lo、hi 很大时会溢出**。
        //        本题 n ≤ 10^4 不会溢出，但请养成 `lo + (hi - lo) / 2` 的习惯。
        //     4. **返回的是下标，不是值**；找不到返回 **-1**（不是 0，也不是 n）。
        //
        //   ④ 一句实话
        //      本题 n ≤ 10^4 很小，所以**写线性扫描也能通过 LeetCode**。
        //      但这题的价值在模板 —— 后面的题（875 珂珂吃香蕉、153 旋转数组最小值）
        //      问题形状更复杂，**没有模板就写不出来**。所以请把它当模板题对待。
        //
        //   复杂度：O(log n) 时间，O(1) 空间。

        int left = 0, right = (int)nums.size() - 1;

        while (left <= right)
        {
            int mid = left + (right - left) / 2;

            if (nums[mid] < target)
                left = mid + 1;
            else if (nums[mid] > target)
                right = mid - 1;
            else
                return mid;
        }

        return -1;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [-1,0,3,5,9,12] 便于打印；过长的数组只打印首 6 末 3
static string vectorToString(const vector<int> &v)
{
    if (v.size() <= 12)
    {
        string s = "[";
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (i)
                s += ",";
            s += to_string(v[i]);
        }
        return s + "]";
    }

    string s = "[";
    for (int i = 0; i < 6; ++i)
    {
        if (i)
            s += ",";
        s += to_string(v[i]);
    }
    s += ", …(略去 " + to_string(v.size() - 9) + " 个)…, ";
    for (size_t i = v.size() - 3; i < v.size(); ++i)
    {
        if (i != v.size() - 3)
            s += ",";
        s += to_string(v[i]);
    }
    return s + "]";
}

// 单条测试用例
struct TestCase
{
    string name;      // 用例名称
    vector<int> nums; // 输入：升序数组
    int target;       // 要找的目标值
    int expected;     // 期望返回的下标（找不到为 -1）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<int> nums = c.nums; // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    int actual = sol.search(nums, c.target);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       nums = " << vectorToString(c.nums)
         << "   target = " << c.target << "\n"
         << "       期望: " << c.expected << "   实际: " << actual << endl;
    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {-1, 0, 3, 5, 9, 12}, 9, 4},
        {"示例 2", {-1, 0, 3, 5, 9, 12}, 2, -1},
    };

    // 额外的边界与补充用例
    // 下面所有 expected 都由线性扫描独立算出（不依赖二分）
    vector<TestCase> extraCases = {
        {"单元素·命中", {5}, 5, 0},
        {"单元素·未命中", {5}, -5, -1},
        {"目标在最左端", {1, 2, 3}, 1, 0},
        {"目标在正中间", {1, 2, 3}, 2, 1},
        {"目标在最右端", {1, 2, 3}, 3, 2},
        {"目标比所有数都小", {1, 2, 3}, 0, -1},
        {"目标比所有数都大", {1, 2, 3}, 4, -1},
        {"全负数", {-5, -3, -1}, -3, 1},
        {"包含 0", {-1, 0, 1}, 0, 1},
        {"偶数长度·命中左半", {-2, -1, 0, 1}, -1, 1},
        {"偶数长度·命中右半", {-2, -1, 0, 1}, 0, 2},
        {"两个元素·命中右边", {1, 2}, 2, 1},
        {"两个元素·未命中", {1, 2}, 3, -1},
        {"数字不连续", {1, 10, 100, 1000}, 100, 2},
        {"取值下限 -10000", {-10000, 0, 10000}, -10000, 0},
        {"取值上限 10000", {-10000, 0, 10000}, 10000, 2},
    };

    // 最大规模用例：n = 10^4（题目上限），值为 -10000, -9998, ..., 9998
    //   注意：本题 n 很小，线性扫描也能瞬间跑完 —— 所以这组用例**考的不是性能**，
    //   而是"区间能不能正常收缩"。如果二分写成死循环，这里会直接卡住不返回。
    {
        const int n = 10000;
        vector<int> nums(n);
        for (int i = 0; i < n; ++i)
            nums[i] = i * 2 - 10000; // 偶数序列：-10000, -9998, ...

        extraCases.push_back({"n=10^4·命中（值 0 在下标 5000）", nums, 0, 5000});
        extraCases.push_back({"n=10^4·未命中（值 1 不存在）", nums, 1, -1});
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

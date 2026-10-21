#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 154. 寻找旋转排序数组中的最小值 II（Find Minimum in Rotated Sorted Array II）· 二分查找 · Hard
//
// 这是 153 的直接续集：**唯一的区别是允许重复元素**。
// 别小看这一个变化 —— 它把 153 里"看一眼就知道往哪边走"的二分，
// 变成了"有时候什么也看不出来、只能一格一格试"，复杂度从 O(log n) 退化到 O(n)。
//
//   先把 153 的模板摆在这里（跟右端点比）：
//     while (lo < hi) {
//         int mid = lo + (hi - lo) / 2;
//         if      (nums[mid] > nums[hi]) lo = mid + 1;   // 最小值在右半边
//         else if (nums[mid] < nums[hi]) hi = mid;       // 最小值在左半边（含 mid）
//         else                            ???            // 相等：153 里不会发生，154 里必须处理
//     }
//
//     · "相等"意味着什么都推不出来：
//         例 [1,1,1,0,1]：mid 指到 1（和 nums[hi]=1 相等），但最小值 0 藏在 **右边**；
//         例 [1,0,1,1,1]：mid 也是 1（同样相等），最小值 0 却在 **左边**。
//       同一个"相等"背后可以是完全相反的局面 —— 这就是二分的**信息丢失**。
//
//     · 那这一格只能丢掉一个**确定安全**的元素：
//           hi--;        // 只丢掉 nums[hi] 这一个
//       为什么安全？因为 nums[mid] == nums[hi] 且 mid < hi，
//       "最小值就是 nums[hi]" 这个可能性已经被 nums[mid] 顶替了，
//       所以区间 [lo, hi-1] 里仍然保留着至少一份最小值。
//
//   完整写法（在 153 的基础上只多一个分支）：
//     int lo = 0, hi = (int)nums.size() - 1;
//     while (lo < hi) {
//         int mid = lo + (hi - lo) / 2;
//         if      (nums[mid] > nums[hi]) lo = mid + 1;
//         else if (nums[mid] < nums[hi]) hi = mid;
//         else                            hi--;      // 相等：只能排除右端点
//     }
//     return nums[lo];
//
//   三个坑（下面三条用例就是专门抓它们的）：
//     1. **相等时不能写 `hi = mid`** —— 那等于断言"最小值不在 (mid, hi]"，
//        而这个结论根本推不出来。反例 [1,1,1,0,1]：
//          lo=0, hi=4, mid=2 -> nums[2]==nums[4]==1 -> 若 hi=2，区间缩成 [1,1,1]，错答 1。
//       所以必须写成 hi--，一次只退一格。
//     2. **相等时也不能写 `hi = mid - 1`** —— mid 自己可能就是最小值。
//        反例 [2,1,1,1]（是 [1,1,1,2] 左旋 3 次）：
//          lo=0, hi=3, mid=1 -> nums[1]==nums[3]==1 -> hi=0，循环结束，返回 nums[0]=2，错。
//     3. **不能省略相等分支**。如果 153 你写的是"两分支 + else hi=mid"，
//        直接搬过来就是坑 1 的错法 —— 这题的相等必须单独拆一支出来。
//
//   复杂度（这题最值得讲的一问）：
//     · 最好 / 平均：O(log n)，大多数时候 nums[mid] != nums[hi]，正常对半砍
//     · **最坏：O(n)**，大量重复时每轮只能 hi-- 退一格
//     最坏例子：
//       ① [1,0,1,1,...,1]（0 在第 2 位、后面全是 1）—— mid 每轮都撞上 1，和 nums[hi] 相等，
//          只能 hi-- 一格一格退，约 n 步；
//       ② 全相等的数组 [2,2,2,...,2] 同理。
//     这不是实现不好，而是**问题本身信息量不够**：相等时不提供任何方向。
//     面试被问"能不能保持 O(log n)"，正确答案是"**不能**，除非先额外预处理去重"。
//
//   面试追问：
//     · 为什么 153 是 O(log n)、154 却只能 O(n)？
//       -> 153 保证互不相同，比较结果必然有方向；154 相等时两种局面都可能，只能保守丢一格
//     · 平均复杂度还是 O(log n) 吗？-> 是，只要不是刻意构造的大量重复
//     · 33 / 81 什么关系？-> 33 在旋转数组里找 target（无重复），81 是它的含重复版，
//       81 同样"相等时只敢排除一个"，最坏也是 O(n)
//
//   写完可以顺手对照 153 的两种写法：`hi = mid` 那种两分支版在 153 里正确，
//   到了这里必须在相等处拆出 hi--，否则 [1,1,1,0,1] 立刻挂。
//
//   同族题：153（本题的无重复版）、33 搜索旋转排序数组、81（含重复版搜索）、
//           162 寻找峰值、852 山脉数组的峰顶索引、875 爱吃香蕉的珂珂
class Solution
{
public:
    int findMin(vector<int> &nums)
    {
        // TODO: 在这里写你的解法

        int left = 0, right = (int)nums.size() - 1;

        while (left < right)
        {
            int mid = left + (right - left) / 2;

            if (nums[mid] > nums[right])
                left = mid + 1;
            else if (nums[mid] < nums[right])
                right = mid;
            else
                left--;
        }

        return nums[left];
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase
{
    string name;      // 用例名称
    vector<int> nums; // 输入：旋转过的非降序数组（**允许重复**）
    int expected;     // 期望结果
};

// 把数组格式化成 [2,2,2,0,1]，过长时截断
static string joinNums(const vector<int> &v, size_t limit = 24)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += to_string(v[i]);
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 个元素）";
    return s + "]";
}

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    int actual = 0;

    try
    {
        Solution sol;
        vector<int> nums = c.nums; // 传副本：万一解法改动了入参，打印时也不受影响
        actual = sol.findMin(nums);
    }
    catch (const exception &e)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << e.what() << endl;
        return false;
    }

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  n = " << c.nums.size() << "\n"
         << "       nums     = " << joinNums(c.nums) << "\n"
         << "       期望     = " << c.expected << "\n"
         << "       实际     = " << actual << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 3, 5}, 1},
        {"示例 2（最小值藏在重复元素后面）", {2, 2, 2, 0, 1}, 0},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 长度 1 / 2
        {"单元素", {1}, 1},
        {"两个元素相等（抓 hi = mid - 1 越界）", {1, 1}, 1},
        {"两个元素旋转", {2, 1}, 1},

        // 全相等：最坏 O(n) 的一种极端
        {"全相等 5 个元素", {2, 2, 2, 2, 2}, 2},

        // ★ 三个"相等陷阱"（下面括号里的"挂几条"是实测结果，不是推测）：
        //   陷阱 1 → 坑 1 + 坑 3：把 153 的两分支版（相等时 hi = mid）直接搬过来就错
        //   陷阱 3 → 坑 2：专抓"相等时写 hi = mid - 1"
        //   陷阱 2 是"最小值在 mid 左边"的对称情形，正常实现也必须答对（不是来抓 bug 的）
        {"陷阱 1：最小值在 mid 右边 [1,1,1,0,1]", {1, 1, 1, 0, 1}, 0},
        {"陷阱 2：最小值在 mid 左边 [1,0,1,1,1]", {1, 0, 1, 1, 1}, 0},
        {"陷阱 3：连续重复把最小值包住 [2,1,1,1]", {2, 1, 1, 1}, 1},

        // 大量重复包围最小值
        {"重复包围一", {2, 1, 2, 2, 2}, 1},
        {"重复包围二", {3, 3, 1, 3}, 1},
        {"重复包围三", {3, 3, 1, 1, 2}, 1},
        {"重复包围四", {2, 2, 2, 0, 2}, 0},
        {"重复包围五", {10, 1, 10, 10, 10}, 1},

        // 未旋转（旋转 n 次）
        {"未旋转（互不相同）", {1, 2, 3}, 1},
        {"未旋转（含重复）", {1, 1, 2, 3, 3}, 1},

        // 边界数值 -5000..5000
        {"跨零旋转", {5000, -5000, 5000}, -5000},
        {"边界值加重复", {5000, 5000, -5000, -5000}, -5000},
        {"最小值在最前", {1, 0}, 0},
    };

    // 长数组压力用例：n = 5000（题目上界）
    //   前两条是**最坏 O(n)** 的构造：让 nums[mid] 反复等于 nums[hi]，
    //   二分退化成"每轮只 hi-- 一格"，正好用来确认你的实现没有依赖"每轮砍一半"
    {
        TestCase c;
        c.name = "长数组 5000：全相等（最坏 O(n)）";
        for (int i = 0; i < 5000; ++i)
            c.nums.push_back(2);
        c.expected = 2;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "长数组 5000：[1,0,1,1,…]（mid 反复撞 1，最坏 O(n)）";
        // 等价于 [0,1,1,...,1] 左旋 4999 次
        c.nums.push_back(1);
        c.nums.push_back(0);
        for (int i = 0; i < 4998; ++i)
            c.nums.push_back(1);
        c.expected = 0;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "长数组 5000：未旋转递增（走最好情况 O(log n)）";
        for (int i = 0; i < 5000; ++i)
            c.nums.push_back(i);
        c.expected = 0;
        extraCases.push_back(c);
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

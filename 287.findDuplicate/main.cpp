#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 287. 寻找重复数（Find the Duplicate Number）· 链表/快慢指针 · Medium
//
// 题目给了三个硬约束，**这题的价值全在约束里**，先把它们看清：
//   · 数组长度 = n + 1，值域是 [1, n]  -> 由鸽巢原理，**必然有重复**
//   · **不能修改数组**  -> 排序、打标记都不行
//   · **只能用 O(1) 额外空间** -> 哈希表不行
//   follow-up 还要求**线性时间** -> 二分答案的 O(n log n) 不是最优解
//
//   正解：把数组看成一个**函数图**（functional graph）：
//       每个下标 i 有一条出边指向 nums[i]。
//       因为 i ∈ [0, n]、nums[i] ∈ [1, n]，从下标 0 出发一路走下去一定会进环
//       （n+1 个点、每个点只有 1 条出边，走够 n+1 步必然重复访问某个点）。
//       而"某个值 v 出现两次"意味着两个不同的下标都指向 v —— 那正是**环的入口**。
//       于是问题变成：**求环的入口**，也就是 142 环形链表 II。
//
//   代码（Floyd 判环 + 找入口两段）：
//       int slow = nums[0], fast = nums[0];
//       do {
//           slow = nums[slow];            // 走 1 步
//           fast = nums[nums[fast]];      // 走 2 步
//       } while (slow != fast);           // ① 先让两人相遇
//
//       slow = nums[0];                   // ② 一个指针回到"起点"
//       while (slow != fast) {
//           slow = nums[slow];
//           fast = nums[fast];            // 两人都改成走 1 步
//       }
//       return slow;                      // 相遇点就是重复值
//
//   四个坑：
//     1. **必须用 do-while**：初始 slow == fast，写成 while 会一次都不进。
//     2. **第二阶段 slow 要回到 `nums[0]` 而不是 `0`**：
//        图的"起点"是下标 0 指向的那个节点（nums[0]），回到 0 会多走一步。
//     3. **不能把 nums 当 visited 数组用**（比如 nums[nums[i]] = -nums[nums[i]]）：
//        题目禁止修改数组，骨架会比对调用前后的数组快照。
//     4. **别用求和 / 异或**：重复的次数未知（可能重复 3 次以上），差值算不出重复值。
//
//   为什么第二阶段能对上（面试会问）：
//       设起点到环入口距离 a、环长 L、相遇点距入口 b。
//       slow 走了 a + b 步，fast 走了 a + b + kL 步，而 fast = 2 * slow
//         => a + b = kL  =>  a = kL - b
//       把 slow 放回起点、两人同速走：slow 走 a 步到入口时，
//       fast 从相遇点走 a = kL - b 步，也正好落在入口。所以第二次相遇就是入口。
//
//   复杂度：Floyd O(n) 时间 / O(1) 空间（满足 follow-up 的线性要求）。
//
//   另一条**合法但更慢**的思路（面试可当对照说）：
//       二分答案：对值域 [1, n] 二分 mid，统计 nums 中 <= mid 的个数 cnt；
//       若 cnt > mid，说明重复值在 [1, mid]，否则在 [mid+1, n]。
//       O(n log n) 时间 / O(1) 空间、不改数组，满足前两个约束但不满足线性。
//
//   同族题：141 环形链表（判环）、142 环形链表 II（找入口，本题就是它的数组版）、
//           645 错误的集合、41 缺失的第一个正数
class Solution
{
public:
    int findDuplicate(vector<int> &nums)
    {
        // TODO: 在这里写你的解法

        int slow = nums[0], fast = nums[0];

        // ① 找相遇点
        do
        {
            slow = nums[slow];       // 走 1 步
            fast = nums[nums[fast]]; // 走 2 步
        } while (slow != fast);

        // ② 找入口：一个指针回起点，两人同速走
        slow = nums[0];
        while (slow != fast)
        {
            slow = nums[slow];
            fast = nums[fast];
        }
        return slow; // 相遇点 = 重复值
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [1,3,4,2,2]，过长只打印首 10 个
static string vectorToStr(const vector<int> &v, size_t limit = 10)
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

// 单条测试用例
struct TestCase
{
    string name;
    vector<int> nums; // 长度 n+1，值域 [1, n]，恰有一个值重复
    int expected;     // 期望的重复值
};

// 数据自检：本题的输入必须满足
//   · nums.size() == n + 1
//   · 每个值都在 [1, n]
//   · **只有一个值出现多次**（题目："there is only one repeated number"）
// 注意：不要求 1..n 每个值都出现 —— LeetCode 自己的示例 3 [3,3,3,3,3] 就不满足，
// 但它合法（唯一重复的值是 3）。数据不合法时提前报错，
// 免得把"用例写错"当成"解法写错"来查。
static bool checkConstraints(const TestCase &c, string &why)
{
    int n = (int)c.nums.size() - 1;
    if (n < 1)
    {
        why = "长度不足（至少要 2 个元素）";
        return false;
    }
    vector<int> cnt(n + 1, 0);
    for (int v : c.nums)
    {
        if (v < 1 || v > n)
        {
            why = "值 " + to_string(v) + " 超出合法范围 [1," + to_string(n) + "]";
            return false;
        }
        ++cnt[v];
    }
    int dup = 0;
    for (int v = 1; v <= n; ++v)
        if (cnt[v] > 1)
            ++dup;
    if (dup != 1)
    {
        why = "出现多次的值有 " + to_string(dup) + " 个，题目要求恰好 1 个";
        return false;
    }
    return true;
}

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    string why;
    if (!checkConstraints(c, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    vector<int> nums = c.nums; // 传副本给解法；同时留一份原始快照做比对
    const vector<int> before = c.nums;

    int got = 0;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.findDuplicate(nums);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    if (threw)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << err << endl;
        return false;
    }

    // 题目明确要求"不能修改数组"
    bool intact = (nums == before);

    bool pass = (got == c.expected) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  n = " << (c.nums.empty() ? 0 : c.nums.size() - 1) << "\n"
         << "       nums = " << vectorToStr(c.nums) << "\n"
         << "       期望 = " << c.expected << "   实际 = " << got;
    if (!intact)
        cout << "\n       ✗ 数组被修改了（题目要求不能修改 nums）";
    cout << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 3, 4, 2, 2}, 2},
        {"示例 2", {3, 1, 3, 4, 2}, 3},
        {"示例 3（全部相同）", {3, 3, 3, 3, 3}, 3},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 最短的合法输入：n=1，长度 2
        {"最短输入 [1,1]", {1, 1}, 1},

        // n=2（长度 3）的三种形态
        {"n=2，重复 1 且在末尾", {1, 2, 1}, 1},
        {"n=2，重复 2 且在开头", {2, 2, 1}, 2},
        {"n=2，全部相同", {2, 2, 2}, 2},

        // n=3（长度 4）
        {"n=3，重复最大值 3", {1, 2, 3, 3}, 3},
        {"n=3，重复 2 且在开头", {2, 2, 1, 3}, 2},
        {"n=3，重复 1 且在中间", {3, 1, 1, 2}, 1},

        // n=5（长度 6）：重复值在末尾 / 在开头
        {"n=5，重复 5 且在末尾", {1, 2, 3, 4, 5, 5}, 5},
        {"n=5，重复 5 且在开头", {5, 1, 2, 3, 4, 5}, 5},

        // 环比较长：重复值出现在靠后的位置
        {"n=8，重复值在倒数第二个位置", {1, 2, 3, 4, 5, 6, 7, 8, 8}, 8},

        // 重复 3 次以上（排除"求和/异或"那类假设只重复两次的写法）
        {"重复 3 次", {2, 2, 2, 3, 1}, 2},
    };

    // 大规模用例：n = 100000（题目上限）
    {
        const int n = 100000;
        // ① 重复值在中间：下标 0..n-1 放 1..n，最后一个位置再放一次 50000
        {
            vector<int> v;
            v.reserve(n + 1);
            for (int i = 1; i <= n; ++i)
                v.push_back(i);
            v.push_back(50000);
            extraCases.push_back({"大规模 n=100000，重复 50000", v, 50000});
        }
        // ② 重复值在最前面
        {
            vector<int> v;
            v.reserve(n + 1);
            v.push_back(1);
            for (int i = 1; i <= n; ++i)
                v.push_back(i);
            extraCases.push_back({"大规模 n=100000，重复 1", v, 1});
        }
        // ③ 重复值在末尾
        {
            vector<int> v;
            v.reserve(n + 1);
            for (int i = 1; i <= n; ++i)
                v.push_back(i);
            v.push_back(n);
            extraCases.push_back({"大规模 n=100000，重复 n", v, n});
        }
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

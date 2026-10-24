#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 33. 搜索旋转排序数组（Search in Rotated Sorted Array）· 二分查找 · Medium
//
// 153 是"找最小值"，这题是"找某个具体值" —— 同样的"两段各自有序"结构，
// 但二分时的分支从 2 个变成了 4 个。
//
//   核心观察：不管数组怎么旋转，**mid 一定落在某一段里，而这一段必然是有序的**。
//   所以二分的每一轮先问两件事：
//       ① 哪半边是有序的？
//       ② target 在不在这个有序半边里？→ 在就往这边收，不在就往另一边收。
//
//   完整写法（闭区间）：
//     int lo = 0, hi = (int)nums.size() - 1;
//     while (lo <= hi) {
//         int mid = lo + (hi - lo) / 2;
//         if (nums[mid] == target) return mid;
//
//         if (nums[lo] <= nums[mid]) {                       // 左半边 [lo, mid] 有序
//             if (nums[lo] <= target && target < nums[mid])  //   target 落在左半边
//                 hi = mid - 1;
//             else
//                 lo = mid + 1;
//         } else {                                            // 右半边 [mid, hi] 有序
//             if (nums[mid] < target && target <= nums[hi])   //   target 落在右半边
//                 lo = mid + 1;
//             else
//                 hi = mid - 1;
//         }
//     }
//     return -1;
//
//   四个坑：
//     1. **`nums[lo] <= nums[mid]` 的等号**：mid == lo 时左半边只有一个元素，
//        它当然"有序"，必须走左边分支。写成 `<` 会误判成"右半边有序"，直接跑偏。
//     2. **区间判断的端点要带等号**：左边那支写的是 `nums[lo] <= target && target < nums[mid]`。
//        反例：{4,5,6,7,0,1,2} 找 4 —— 若把下界写成 `nums[lo] < target`，
//        target == nums[lo] 不满足 → 走去右半边 → 漏掉下标 0，错答 -1。
//     3. **循环条件是 `lo <= hi`**（闭区间找等值），最后 `return -1`。
//        这里不能照搬 153 的 `lo < hi` —— 找具体值必须检查到最后一个元素。
//     4. **收缩可以放心写 ±1**（`mid - 1` / `mid + 1`）：
//        因为上面已经判断过 `nums[mid] != target`，mid 可以排除；
//        而 153 找最小值时 mid 可能是答案，所以只能写 `hi = mid`。
//
//   另一条路（更"模块化"，面试也常见）：
//     先用 153 的二分找到旋转点 pivot（最小值的下标），
//     再在 [0, pivot-1] 和 [pivot, n-1] 两段各做一次普通二分，看 target 落在哪段。
//     同样是 O(log n)，代码更长但每段都是"标准二分"，不容易把边界写错。
//
//   复杂度：时间 O(log n)，空间 O(1)。
//
//   面试追问：
//     · 有重复元素怎么办？-> 81：当 nums[lo] == nums[mid] 时无法判断哪边有序，
//       只能 lo++ 缩小范围，最坏退化 O(n)
//     · 为什么必须先判断"哪边有序"？-> 旋转后只有一处"断点"，断点两侧各有一段单调；
//       只有单调的那段才能用"target 是否在 [左端, 右端] 内"来直接判断方向
//     · 和 153/154 什么关系？-> 153 找最小值用 2 个分支，本题找 target 用 4 个分支；
//       154 / 81 分别是它们"含重复元素"的版本
//
//   同族题：153 / 154（找最小值）、81（本题的含重复版）、704（无旋转的普通二分）、
//           162 寻找峰值、875 爱吃香蕉的珂珂
class Solution
{
public:
    int search(vector<int> &nums, int target)
    {
        int left = 0, right = nums.size() - 1;

        while (left <= right)
        {
            int mid = left + (right - left) / 2;

            if (nums[mid] == target)
                return mid;

            if (nums[left] <= nums[mid])
            {
                if (nums[left] <= target && target <= nums[mid])
                    right = mid - 1;
                else
                    left = mid + 1;
            }
            else
            {
                if (nums[right] >= target && target >= nums[mid])
                    left = mid + 1;
                else
                    right = mid - 1;
            }
        }

        return -1;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase
{
    string name;      // 用例名称
    vector<int> nums; // 输入：旋转过的升序数组（元素互不相同）
    int target;       // 要查找的目标值
    int expected;     // 期望结果（不存在为 -1）
};

// 把数组格式化成 [4,5,6,7,0,1,2]，过长时截断
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
        actual = sol.search(nums, c.target);
    }
    catch (const exception &e)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << e.what() << endl;
        return false;
    }

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  n = " << c.nums.size() << ", target = " << c.target << "\n"
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

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1（target 在第二段）", {4, 5, 6, 7, 0, 1, 2}, 0, 4},
        {"示例 2（target 不存在）", {4, 5, 6, 7, 0, 1, 2}, 3, -1},
        {"示例 3（单元素）", {1}, 0, -1},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 长度 1 / 2
        {"单元素命中", {1}, 1, 0},
        {"两个元素（旋转后 target 在后）", {3, 1}, 1, 1},
        {"两个元素（旋转后 target 在前）", {3, 1}, 3, 0},

        // 没旋转（旋转 n 次）：退化成普通二分
        {"未旋转，target 在最左", {1, 2, 3, 4, 5}, 1, 0},
        {"未旋转，target 在最右", {1, 2, 3, 4, 5}, 5, 4},
        {"未旋转，target 不存在", {1, 2, 3, 4, 5}, 6, -1},

        // 旋转后各个关键位置
        {"target 是第一段最大值（紧挨断点）", {4, 5, 6, 7, 0, 1, 2}, 7, 3},
        {"target 在第二段最末尾", {4, 5, 6, 7, 0, 1, 2}, 2, 6},
        {"target 恰好是 nums[0]（抓区间下界漏等号）", {4, 5, 6, 7, 0, 1, 2}, 4, 0},
        {"三个元素组成的最小旋转", {2, 3, 1}, 1, 2},
        {"target 落在断点的空隙里", {6, 7, 8, 1, 2, 3}, 5, -1},

        // ★ 这两条抓的是"用「整个窗口是否有序 nums[lo] <= nums[hi]」来选边"的错法：
        //   此时窗口里确实有断崖(lo > hi 位置比较为假)，但断崖落在**左半边**，
        //   于是右半边才有应该是被检查的那一段。若代码在"窗口无序"时默认左半边有序，
        //   就会把 target 推向右半段并漏掉答案。补充前用例集恰好全部覆盖不到，实测漏判。
        {"target 在第一段、断崖落在左半边 [6,7,0,1,2,3]", {6, 7, 0, 1, 2, 3}, 6, 0},
        {"target 在第一段、断崖落在左半边 [8,9,1,2,3,4,5]", {8, 9, 1, 2, 3, 4, 5}, 9, 1},

        // 负数与跨零
        {"负数跨零旋转，找第二段起点", {10, 20, -5, -3, -1}, -5, 2},
        {"负数跨零旋转，找末尾", {10, 20, -5, -3, -1}, -1, 4},
        {"负数组未旋转", {-5, -3, -1, 0, 2}, -3, 1},

        // 题目范围边界 ±10^4
        {"边界值 10000 / -10000", {10000, -10000}, -10000, 1},
    };

    // 长数组压力用例：n = 5000（题目上界）
    //   二分只需约 13 轮，这里主要验证下标边界（mid 计算、lo/hi 收缩）不出错
    {
        TestCase c;
        c.name = "长数组 5000：未旋转，target 在末尾";
        for (int i = 0; i < 5000; ++i)
            c.nums.push_back(i);
        c.target = 4999;
        c.expected = 4999;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "长数组 5000：左旋 2500 次，target = 0（在右段起点）";
        // 升序 0..4999 左旋 2500 -> [2500..4999, 0..2499]
        for (int i = 2500; i < 5000; ++i)
            c.nums.push_back(i);
        for (int i = 0; i < 2500; ++i)
            c.nums.push_back(i);
        c.target = 0;
        c.expected = 2500;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "长数组 5000：同一数组，target = 2499（在末尾）";
        for (int i = 2500; i < 5000; ++i)
            c.nums.push_back(i);
        for (int i = 0; i < 2500; ++i)
            c.nums.push_back(i);
        c.target = 2499;
        c.expected = 4999;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "长数组 5000：未旋转，target 不存在";
        for (int i = 0; i < 5000; ++i)
            c.nums.push_back(i);
        c.target = -1;
        c.expected = -1;
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

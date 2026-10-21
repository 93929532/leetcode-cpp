#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 153. 寻找旋转排序数组中的最小值（Find Minimum in Rotated Sorted Array）· 二分查找 · Medium
//
// 进入「二分查找」主题后的第一道硬题。它不像 704 那样"有序数组找目标"那么直白，
// 而是要你在「**两段各自有序**」的数组里判断答案落在哪一半。
//
//   题目给的数组长这样（升序数组被"转"了一下）：
//       [4, 5, 6, 7, 0, 1, 2]
//        └─ 第一段 ─┘  └─ 第二段 ─┘
//   特点：整个数组由**两段递增**组成，且 **第二段的第一个元素就是全局最小值**。
//   （如果"旋转了 n 次"，等于没转，整个数组还是一段递增，此时最小值是 nums[0]。）
//
//   于是问题变成：**怎么用二分的眼光判断"最小值在 mid 的左边还是右边"？**
//
//   关键决定：拿 nums[mid] 跟 **nums[hi]（右端点）** 比。只有两种情况：
//
//     if (nums[mid] > nums[hi])   mid 还在第一段，最小值一定在 mid 的**右边**（不含 mid）
//                                 -> lo = mid + 1
//     else                        最小值在 mid 的**左边，或者就是 mid**
//                                 -> hi = mid
//
//   为什么这么判断？拿 [4,5,6,7,0,1,2] 走一遍：
//     mid 指到 7（7 > nums[hi]=2）→ mid 还在第一段，最小值只能更靠右 → lo = mid + 1
//     mid 指到 0（0 < nums[hi]=2）→ mid 已落到第二段，最小值要么是它、要么更靠左 → hi = mid
//   记一个不变量就够了：**区间 [lo, hi] 里始终包含最小值**。
//
//   完整骨架（闭区间写法）：
//     int lo = 0, hi = (int)nums.size() - 1;
//     while (lo < hi) {
//         int mid = lo + (hi - lo) / 2;
//         if (nums[mid] > nums[hi]) lo = mid + 1;
//         else                      hi = mid;
//     }
//     return nums[lo];       // 此时 lo == hi，就是最小值的下标
//
//   三个坑：
//     1. **`hi = mid` 不能写成 `hi = mid - 1`**：mid 本身可能就是最小值，
//        减一会把它排除掉 —— 示例 1（[3,4,5,1,2] 找 1）就会漏掉答案。
//     2. **循环条件必须是 `lo < hi`**：配 `hi = mid` 时若写成 `lo <= hi`，
//        lo == hi == mid 那一轮还会执行 `hi = mid`，区间不再缩小 → **死循环**。
//     3. **不要为"没旋转"特判**：示例 3（[11,13,15,17]）一路走 else 向左收缩，
//        自然停在下标 0。这一点正是这套模板最漂亮的地方，硬加特判反而容易错。
//
//   为什么是"跟右端点比"而不是"跟左端点比"？
//     试一个看起来对称的写法：if (nums[mid] < nums[lo]) hi = mid; else lo = mid + 1;
//     它在 [1,2,3]（没旋转）上会一路向右，最后返回 3 —— 错。
//     根因：nums[lo] 是"第一段的起点"，当 mid 落在第一段 / 第二段时它的含义并不一样；
//     而 nums[hi] 在递增的尾巴上，比较结果能直接告诉你"mid 在哪一段"。
//     所以跟右端点比是主流写法；若坚持跟左端点比，就必须额外特判"没旋转"的情形。
//   面试时**选跟右端点比**这一版，讲起来最干净。
//
//   复杂度：时间 O(log n)（每次砍掉一半），空间 O(1)。
//   注意比较结果只有"大于 / 不大于"两种 —— 本题保证元素**互不相同**，
//   所以不存在 nums[mid] == nums[hi] 的歧义；若有重复（154 题），相等时只能 hi--，最坏退化 O(n)。
//
//   面试追问：
//     · 数组里有重复元素怎么办？-> 154「寻找旋转排序数组中的最小值 II」，相等时 hi-- 逐个试
//     · 怎么说明是 O(log n)？-> 区间长度每次至少减半，最多 log2(n)+1 轮（n=5000 时约 13 轮）
//     · 旋转了 0 次（本来就升序）？-> 同一个模板直接答对，不用加分支
//     · 为什么写 `lo + (hi - lo) / 2` 而不是 `(lo + hi) / 2`？-> 防溢出。
//       本题 n ≤ 5000 无所谓，但这是写二分的好习惯
//
//   同族题：33 搜索旋转排序数组、81（33 的含重复版）、154（本题的含重复版）、
//           162 寻找峰值、852 山脉数组的峰顶索引、278 第一个错误的版本、875 爱吃香蕉的珂珂
class Solution
{
public:
    int findMin(vector<int> &nums)
    {
        // TODO: 在这里写你的解法

        int left = 0, right = nums.size() - 1;

        if(nums[left] <= nums[right])
            return nums[left];

        while (right - left > 1)
        {
            int mid = left + (right - left) / 2;

            if (nums[mid] < nums[right])
                right = mid;
            else if (nums[mid] > nums[left])
                left = mid;
            else
                return 0;
        }

        return nums[right];
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase
{
    string name;      // 用例名称
    vector<int> nums; // 输入：旋转过的升序数组（元素互不相同）
    int expected;     // 期望结果
};

// 把数组格式化成 [3,4,5,1,2]，过长时截断
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

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {3, 4, 5, 1, 2}, 1},
        {"示例 2", {4, 5, 6, 7, 0, 1, 2}, 0},
        {"示例 3（旋转 n 次 = 没旋转）", {11, 13, 15, 17}, 11},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 长度为 1 / 2 的极端情况
        {"单元素", {5}, 5},
        {"两个元素未旋转", {1, 2}, 1},
        {"两个元素旋转", {2, 1}, 1},

        // 旋转位置的极端：最小值在最末尾 / 最靠前
        {"旋转 1 次（最小值在末尾）", {2, 3, 4, 5, 1}, 1},
        {"旋转 n-1 次（最小值在下标 1）", {5, 1, 2, 3, 4}, 1},
        {"最小值恰好在下标 2", {7, 8, 9, 1, 2, 3}, 1},

        // 负数与边界数值（题目范围 -5000..5000）
        {"全是负数", {-3, -2, -1, -5, -4}, -5},
        {"跨零旋转", {5000, -5000, 0}, -5000},
        {"未旋转的含边界值数组", {-5000, 5000}, -5000},
        {"正负两端都到边", {4999, 5000, -5000, -4999}, -5000},

        // "第一段只剩一个元素" —— 这时 mid 很容易一直落在第二段，arm 容易写错
        {"第一段只剩一个元素", {9, 1, 2, 3, 4, 5, 6, 7, 8}, 1},
        // "第二段只剩一个元素" —— 上一情形的镜像
        {"第二段只剩一个元素", {2, 3, 4, 5, 6, 7, 8, 9, 1}, 1},
    };

    // 长数组压力用例：n = 5000（题目给的上界），三种旋转位置
    //   说明：这题 n 只有 5000，O(n) 也能过；但写了 O(log n) 的二分只需要约 13 轮比较，
    //   所以这里主要是验证**下标边界**在长数组上不出错（mid 溢出、lo/hi 收缩等）
    {
        TestCase c;
        c.name = "长数组 5000 个元素：未旋转（旋转 n 次）";
        for (int i = 0; i < 5000; ++i)
            c.nums.push_back(-2500 + i);
        c.expected = -2500;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "长数组 5000 个元素：最小值在正中间";
        // 升序 0..4999 左旋 2500 次 -> [2500..4999, 0..2499]
        for (int i = 2500; i < 5000; ++i)
            c.nums.push_back(i);
        for (int i = 0; i < 2500; ++i)
            c.nums.push_back(i);
        c.expected = 0;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "长数组 5000 个元素：旋转 1 次（最小值在末尾）";
        // 升序 0..4999 左旋 1 次 -> [1..4999, 0]
        for (int i = 1; i < 5000; ++i)
            c.nums.push_back(i);
        c.nums.push_back(0);
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

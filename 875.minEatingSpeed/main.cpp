#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // max_element：求二分的上界

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    int minEatingSpeed(vector<int> &piles, int h)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 35 分钟以上再回头看）：
        //
        //   ⭐ 从这题开始，二分的**对象变了** —— 这是整个主题的分水岭。
        //
        //   704 是「在数组下标里二分，找 nums[mid] == target」。
        //   875 是「在**答案的取值范围**里二分，找最小的可行值」—— 这叫**二分答案**。
        //
        //   ① 第一步 · 想清楚二分什么
        //      要最小化的是**速度 k**。k 的取值范围是多少？
        //          · 下界 lo = 1                 （速度至少是 1）
        //          · 上界 hi = max(piles)        （k 再大也没意义：每小时最多吃完一堆）
        //      题目保证 piles.length <= h，所以 k = max(piles) 一定可行（每小时一堆）
        //      —— **答案必然存在**，不用担心无解。
        //
        //   ② 第二步 · 写一个「判断函数」
        //          「速度为 k 时，能不能在 h 小时内吃完？」
        //          hours = Σ ceil(piles[i] / k)
        //          返回 hours <= h
        //      这题的一切难点都在这个函数里：
        //          · 向上取整用 `(p + k - 1) / k`（整数运算，不用浮点）
        //          · 累加**必须用 long long**！实测：10^4 堆 × 10^9、k=1 时
        //            真实值是 10^13，用 int 累加会得到 1316134912 —— 完全错误。
        //            也可以在 sum > h 时提前 return false（既快又不会溢出）。
        //
        //   ③ 第三步 · 「找最小可行值」的模板（**和 704 不一样！**）
        //          int lo = 1, hi = max(piles);
        //          while (lo < hi) {
        //              int mid = lo + (hi - lo) / 2;
        //              if (canFinish(mid)) hi = mid;     // ← mid 可行，但它可能就是答案，不能排除
        //              else                lo = mid + 1;  // ← mid 不可行，跳过
        //          }
        //          return lo;                            // lo == hi 就是最小的可行值
        //
        //      ⚠️ **注意 `hi = mid`，不是 `hi = mid - 1`** —— 这是本题最容易错的地方。
        //         为什么？因为 mid **自己可能是答案**（它是可行的），所以只能把区间缩到 mid，
        //         不能把它排除掉（写成 `mid - 1` 可能会丢掉正确答案）。
        //         对比 704：那里的三分支在 `nums[mid] == target` 时**直接返回**了，
        //         所以两边都能放心地 `mid ± 1`。**两题的区别就在这里。**
        //
        //      另一种等价写法（路线图第六节给的那种，记录答案）：
        //          int ans = hi;
        //          while (lo <= hi) {
        //              int mid = ...;
        //              if (canFinish(mid)) { ans = mid; hi = mid - 1; }
        //              else lo = mid + 1;
        //          }
        //          return ans;
        //      两种我都对拍过（各 25 万组，0 差异）—— **关键是内部自洽**：
        //      `while (lo < hi)` 配 `hi = mid`；`while (lo <= hi)` 配 `hi = mid - 1` + 记录答案。
        //
        //   ④ 三个坑
        //     1. **`hi = mid` 写成 `hi = mid - 1`** → 可能跳过正确答案（这是本题头号坑）。
        //     2. **累加用 int** → 溢出后判断函数给出错误的 true/false。
        //     3. **判断函数写成了"计算速度"或"比较pile大小"** →
        //        它必须是「给定 k，算出总耗时，和 h 比」，不能只看单堆。
        //
        //   复杂度：二分 O(log(max(piles))) 次，每次判断 O(n) → O(n log(max(piles)))。

        // 二分答案：二分的不是下标，而是速度 [1, max(piles)]
        int low = 1, fast = 1;

        for (int p : piles)
            fast = max(fast, p);                    // 速度上界：每小时最多吃完一堆，再快没意义

        while (low < fast)
        {
            int mid = low + (fast - low) / 2;       // ① 试一个速度

            int time = 0;                           // ② 算这个速度要几小时
            for (int p : piles)
                time += (p + mid - 1) / mid;        //    整数向上取整（不能用 ceil）

            if (time <= h)                          // ③ 可行？
                fast = mid;                         //    可行 -> 答案 ≤ mid（mid 自己也可能）
            else
                low = mid + 1;                      //    不可行 -> 答案一定更大
        }                                           // ④ 收敛后 low == fast == 最小可行速度

        return low;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [3,6,7,11] 便于打印；过长的数组只打印首 6 末 3
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
    string name;       // 用例名称
    vector<int> piles; // 输入：每堆香蕉数
    int h;             // 输入：限时（小时）
    int expected;      // 期望的最小速度 k
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<int> piles = c.piles; // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    int actual = sol.minEatingSpeed(piles, c.h);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       piles = " << vectorToString(c.piles)
         << "   h = " << c.h << "\n"
         << "       期望: " << c.expected << "   实际: " << actual << endl;
    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {3, 6, 7, 11}, 8, 4},
        {"示例 2", {30, 11, 23, 4, 20}, 5, 30},
        {"示例 3", {30, 11, 23, 4, 20}, 6, 23},
    };

    // 额外的边界与补充用例
    // 下面所有 expected 都满足「最小性证书」：
    //     速度 = expected 能吃完，且 速度 = expected - 1 吃不完
    vector<TestCase> extraCases = {
        {"只有一堆·必须一次吃完", {5}, 1, 5},
        {"只有一堆·可以慢到 1", {5}, 5, 1},
        {"单堆到上限 10^9·h=1", {1000000000}, 1, 1000000000},
        {"单堆是 1·h=1", {1}, 1, 1},
        {"三堆相同 100·h=3", {100, 100, 100}, 3, 100},
        {"h 恰好等于堆数（每小时一堆）", {3, 6, 7, 11}, 4, 11},
        {"h 很大（可以慢到 1）", {3, 6, 7, 11}, 100, 1},
        {"两堆一大一小", {1, 1000000000}, 2, 1000000000},
        {"向上取整的边界", {7, 7, 7}, 3, 7},
        {"全是 1·h=n", {1, 1, 1}, 3, 1},
        {"四堆都是 4·h=5", {4, 4, 4, 4}, 5, 4},
        {"两堆·h 比堆数大一点", {5, 5}, 3, 5},
    };

    // 最大规模用例：n = 10^4 堆、每堆都是 10^9、h = 10^9
    //   答案 k = 10000。这条同时压两件事：
    //     · 判断函数的累加会不会溢出（真实总耗时是 10^13 量级）
    //     · 二分次数够不够（上界 10^9，约 30 次判断 × 10^4 堆）
    {
        const int n = 10000;
        vector<int> piles(n, 1000000000);
        extraCases.push_back({"n=10^4 全 10^9·h=10^9", piles, 1000000000, 10000});
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

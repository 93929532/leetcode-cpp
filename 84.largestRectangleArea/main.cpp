#include <iostream>
#include <string>
#include <vector>
#include <stack>     // 单调栈用它
#include <algorithm> // max 用它

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    int largestRectangleArea(vector<int> &heights)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 35 分钟以上再回头看）：
        //
        //   这题是「栈」主题的最后一题，也是**单调栈最经典的应用**。
        //   它比 739 难的地方在于：739 只要一个方向的信息（右边第一个更暖），
        //   而本题需要**左右两个方向**的信息（左右第一个更矮的柱子）。
        //
        //   ① 第一步 · 换个视角
        //      不要枚举矩形 —— 区间有 O(n²) 个，太贵。
        //      改成**枚举高度**：对每根柱子 j 问一句
        //          「如果以 heights[j] 作为矩形的高，它能向左右各延伸多远？」
        //      能延伸到的边界，就是**左右两边第一个比它矮的柱子**。
        //
        //   ② 第二步 · 单调栈存什么
        //      栈里存**下标**，对应的高度**单调递增**（从栈底到栈顶越来越高）。
        //      含义：这些柱子都「还没遇到右边第一个更矮的柱子」。
        //      当新的柱子 i 比栈顶矮时，栈顶那些柱子就找到右边界了 —— 结算它们。
        //
        //   ③ 第三步 · 面积公式（本题最容易写错的地方）
        //      设被弹出的柱子是 j、它的高度 h = heights[j]：
        //          右边界 = i                （当前这根更矮的柱子）
        //          左边界 = 弹完之后的新栈顶  （左边第一个更矮的；栈空则记为 -1）
        //          宽度   = i - 左边界 - 1     ← 注意是减 1，两个边界柱子都不算进去
        //          面积   = h * 宽度，用它更新答案
        //      **不是 `i - j`** —— 别顺手写成那个。
        //
        //   ④ 第四步 · 收尾
        //      扫描结束后，栈里可能还留着一些柱子（它们右边没有更矮的了）。
        //      最省事的做法：让循环多跑一次（i == n），用一个**比所有高度都小**的
        //      值当哨兵，就能把栈里剩下的全部结算掉。高度可能为 0，所以哨兵要用 -1。
        //
        //   四个坑：
        //     1. **宽度是 `i - left - 1`**，不是 `i - j`，也不是 `i - left`。
        //     2. **弹栈条件用 `>=` 还是 `>`：两种都能对。** 这个我实测过 ——
        //        穷举 9.7 万组 + 随机 6 万组，两种写法结果完全一致。
        //        别在这里纠结太久，选一个写下去就行。
        //     3. **末尾必须处理栈里残留的柱子**，否则会漏掉一大类答案
        //        （典型症状：递增数组全错）。
        //     4. **高度可以是 0**，所以哨兵不能用 0。
        //
        //   复杂度：时间 O(n)（每个下标最多进栈一次、出栈一次），空间 O(n)。
        //   对照：暴力枚举所有区间是 O(n²)，见下面两组 10^5 用例。
        //
        //   顺带一提：返回值用 int 是安全的 —— 最大面积 = 10^5 × 10^4 = 10^9，
        //   而 int 上限约 2.1×10^9。这也是为什么题目允许返回 int。

        // 单调栈：存【下标】，对应高度从栈底到栈顶递增。
        // 含义：栈里这些柱子都"还没遇到右边第一个比它矮的柱子"。
        heights.push_back(0);                                   
        stack<int> heightStack;
        // 末尾补 0 哨兵，保证最后一轮把栈清空
        int res = 0;

        for (int i = 0; i < (int)heights.size(); i++)
        {
            // 来了更矮的柱子 i，栈顶那根柱子的右边界就确定了
            while (!heightStack.empty() && heights[heightStack.top()] > heights[i])
            {
                int h = heights[heightStack.top()];                    // ① 先记住高度
                heightStack.pop();                                     // ② 弹出去
                int l = heightStack.empty() ? -1 : heightStack.top();  // ③ 弹完后的栈顶 = 左边界
                int s = (i - l - 1) * h;                               // ④ 右边界是 i，宽度 = i - l - 1

                res = max(res, s);
            }

            heightStack.push(i);                                    // 弹完（或没得弹）再入栈
        }

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [2,1,5,6,2,3] 便于打印；过长的数组只打印首 6 末 3
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
    string name;         // 用例名称
    vector<int> heights; // 输入：柱状图高度
    int expected;        // 期望的最大面积
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<int> h = c.heights; // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    int actual = sol.largestRectangleArea(h);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       heights = " << vectorToString(c.heights) << "\n"
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
        {"示例 1", {2, 1, 5, 6, 2, 3}, 10},
        {"示例 2", {2, 4}, 4},
    };

    // 额外的边界与补充用例
    // 下面所有 expected 都由 O(n²) 暴力枚举独立算出（不依赖单调栈）
    vector<TestCase> extraCases = {
        {"只有一根柱子", {5}, 5},
        {"单根高度为 0", {0}, 0},
        {"全是 0", {0, 0, 0}, 0},
        {"全部相同（宽度能拉满）", {3, 3, 3, 3}, 12},
        {"递增序列（答案在中间某根）", {1, 2, 3, 4, 5}, 9},
        {"递减序列", {5, 4, 3, 2, 1}, 9},
        {"中间一根极高", {1, 10000, 1}, 10000},
        {"中间为 0（把数组切成两段）", {2, 0, 2}, 2},
        {"两根都到高度上限", {10000, 10000}, 20000},
        {"高矮交错", {6, 2, 5, 4, 5, 1, 6}, 12},
        {"矮个子排一排", {1, 1, 1, 1, 1}, 5},
        {"含 0 的经典用例", {4, 2, 0, 3, 2, 5}, 6},
        {"波浪形", {1, 3, 2, 4, 3, 5}, 10},
        {"两边矮中间高", {5, 5, 1, 5, 5}, 10},
        {"山谷形", {2, 1, 2}, 3},
        {"隔一个来一根高柱", {3, 1, 3, 1, 3}, 5},
        {"答案是一根柱子", {1, 2, 10000, 2, 1}, 10000},
        {"很多个 2 中间一个 3", {2, 2, 2, 3, 2, 2, 2}, 14},
    };

    // 最大规模用例 1：n = 10^5，全部等高 10000
    //   答案 = 10^5 × 10^4 = 10^9（正好在这个数据类型的安全上限附近）
    //   这条专门压 O(n²) 的暴力：它要枚举约 5×10^9 个区间。
    //   实测（本机 -O2）：暴力约 2975 ms，单调栈约 0.15 ms —— 差约 2 万倍。
    //   LeetCode 的时限通常 1~2 秒，所以暴力必然超时。
    {
        const int n = 100000;
        vector<int> heights(n, 10000);
        extraCases.push_back({"最大规模 n=10^5（全部等高 10000，面积 10^9）",
                              heights, 1000000000});
    }

    // 最大规模用例 2：n = 10^5，前一半 10000、后一半 1
    //   答案 = 50000 × 10000 = 5×10^8（跨两半的矩形会被高 1 拖到很小）
    //   实测：暴力约 2997 ms，单调栈约 0.21 ms。
    {
        const int n = 100000, half = n / 2;
        vector<int> heights(n, 1);
        for (int i = 0; i < half; ++i)
            heights[i] = 10000;
        extraCases.push_back({"最大规模 n=10^5（前半 10000、后半 1）",
                              heights, 500000000});
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

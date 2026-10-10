#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // max：解法与测试骨架都要用

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    int trap(vector<int> &height)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   先把「某一列能接多少水」这个公式想清楚，全题就通了：
        //       第 i 列的接水量 = min(左边最高, 右边最高) - height[i]
        //   为什么？水面高度由**两侧较矮的那道墙**决定，再高就漏出去了；
        //   而这一列自身占掉的高度要减掉，剩下的才是水。
        //   剩下的问题只有一个：怎么在不重复扫数组的前提下，拿到每列的左右最高。
        //
        //   思路一 · 暴力（O(n²)，先说出来的保底解）
        //     对每个 i，分别向左、向右扫一遍求 maxLeft / maxRight，再套公式。
        //     能过小数据，但面试官会立刻问下一步。
        //
        //   思路二 · 前缀最大值数组 + 后缀最大值数组（O(n) 时间 / O(n) 空间）
        //     preMax[i]  = max(height[0..i])，sufMax[i] = max(height[i..n-1])，
        //     然后 water += min(preMax[i], sufMax[i]) - height[i]。
        //     ★ 这和你已经做过的 238「除自身以外数组的乘积」是同一套骨架
        //       （左右各扫一遍、把一侧的信息留下来），可以对照着看。
        //
        //   思路三 · 单调栈（O(n)，按"行"算水）
        //     维护一个**高度递减**的下标栈。遇到比栈顶高的柱子时，说明栈顶位置
        //     形成了一个凹槽，可以结算一层水：
        //         bottom = 栈顶（pop 掉）；若栈空则结束
        //         高 = min(height[栈顶], height[i]) - height[bottom]
        //         宽 = i - 栈顶 - 1                        ← 别忘了乘宽度
        //         water += 高 * 宽
        //     思路四（下面这个）是"按列"算，所以宽度恒为 1；单调栈是"按行"算，
        //     必须乘宽度 —— 这是两种写法最容易混淆的地方。
        //
        //   思路四 · 对撞双指针（O(n) 时间 / O(1) 空间，本题最优，也是它被归到
        //            "双指针"主题的原因）
        //     left = 0、right = n-1，一路维护 leftMax / rightMax：
        //         while (left < right) {
        //             if (height[left] < height[right]) {
        //                 leftMax = max(leftMax, height[left]);     // ★ 先更新 max
        //                 water  += leftMax - height[left];         //   再算水量
        //                 ++left;
        //             } else {
        //                 rightMax = max(rightMax, height[right]);
        //                 water += rightMax - height[right];
        //                 --right;
        //             }
        //         }
        //     **为什么这样对**（本题最漂亮的一步）：当 height[left] < height[right] 时，
        //     left 这一列的右边**一定**存在一根比它高的柱子（就是 height[right] 那侧
        //     的某根，至少 height[right] 本身），所以
        //         min(leftMax, rightMax) == leftMax
        //     —— 右侧的墙不可能成为短板，于是 left 列的水量只由 leftMax 决定，
        //     不需要知道 rightMax 的精确值。这就是它能省掉两个数组的原因。
        //
        //   六个坑：
        //     1. **必须先更新 max、再算水量**。顺序反了 water += leftMax - height[left]
        //        会算出**负数**（初始 leftMax = 0 时直接减成负的），答案瞬间崩。
        //        和 238 里"先乘右积再更新 right"是同一类顺序坑。
        //     2. **按列 vs 按行的宽度**：前缀/双指针版是按列，宽度恒为 1；
        //        单调栈版是按行，必须乘 (i - 栈顶 - 1)。漏乘会得到偏小的答案。
        //     3. **n = 0 或 1**：没有凹槽，返回 0（约束是 n ≥ 1，但 n=1 时循环不执行也天然返回 0）。
        //     4. **水量不会是负的**：因为 maxLeft ≥ height[i] 且 maxRight ≥ height[i]，
        //        所以 min(...) ≥ height[i]。如果你算出负数，一定是坑 1 的顺序错了。
        //     5. **数值范围**：n ≤ 2×10⁴、height[i] ≤ 10⁵，最坏总水量约 10⁹ 量级，
        //        int 装得下（上限 2.147×10⁹）；但贴在边界上，用 long long 累加更稳。
        //     6. **别和 11 盛最多水的容器搞混**：那题是"选两根柱子，容量 = 较矮的那根 × 间距"，
        //        求最大值；这题是"所有柱子之间能积多少水"，是求和。双指针的形状像，
        //        要回答的问题完全不同。
        //
        //   进阶：
        //     · 单调栈族的同门：739 每日温度、84 柱状图中最大的矩形（你计划里序号 26）
        //     · 二维版是 407 接雨水 II（换个方向：从边界往里做优先队列/最短路）
        //     · 这题的"左右最高"和 238 的"左右乘积"、135 分发糖果的"左右各扫一遍"，
        //       本质是同一个套路：**一次单向扫描收集一侧的信息，再反向合并**

        if (height.size() == 0 || height.size() == 1)
            return 0;

        int left = 0, right = height.size() - 1;
        int leftMax = height[left], rightMax = height[right];
        int water = 0;

        while (left < right)
        {
            if (height[left] < height[right])
            {
                leftMax = max(leftMax, height[left]);
                water += leftMax - height[left];
                ++left;
            }
            else
            {
                rightMax = max(rightMax, height[right]);
                water += rightMax - height[right];
                --right;
            }
        }

        return water;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [0,1,0,2,1] 便于打印；过长的数组只打印首 6 末 3
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
    string name;        // 用例名称
    vector<int> height; // 输入柱高
    int expected;       // 期望接水量
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<int> height = c.height; // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    int actual = sol.trap(height);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  height = " << vectorToString(c.height)
         << "  期望: " << c.expected
         << "  实际: " << actual << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1}, 6},
        {"示例 2", {4, 2, 0, 3, 2, 5}, 9},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"空数组", {}, 0},
        {"单元素", {5}, 0},
        {"两个元素（无右墙）", {5, 4}, 0},
        {"两端高中间低", {3, 0, 3}, 3},
        {"单调上升（接不到水）", {1, 2, 3, 4, 5}, 0},
        {"单调下降（接不到水）", {5, 4, 3, 2, 1}, 0},
        {"全平", {2, 2, 2}, 0},
        {"全零", {0, 0, 0}, 0},
        {"深坑宽度 3", {5, 0, 0, 0, 5}, 15},
        {"小凹槽只有一个单位", {4, 2, 3}, 1},
        {"两个凹槽", {2, 0, 2, 0, 2}, 4},
        {"极值桶", {100000, 0, 100000}, 100000},
        {"左墙比右墙矮", {2, 0, 5}, 2},
        {"右墙比左墙矮", {5, 0, 2}, 2},
    };

    // 最大规模用例：n = 2×10^4（题目约束上限），100000 与 0 交替
    //   每根"0 柱"左右都是 100000，接水量 100000；但**最后一根 0 柱在最右端、
    //   右侧没有墙**，它接不到水，所以有水的只有 9999 根：
    //       9999 × 100000 = 999,900,000（≈10^9，约为 int 上限的一半）
    //   这条用例同时压 O(n²) 的实现，并压住大数值累加
    {
        vector<int> h;
        h.reserve(20000);
        for (int i = 0; i < 20000; ++i)
            h.push_back(i % 2 == 0 ? 100000 : 0);
        extraCases.push_back({"最大规模 n=2×10^4（总水量 999,900,000）", h, 999900000});
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

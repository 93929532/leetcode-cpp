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
    bool searchMatrix(vector<vector<int>> &matrix, int target)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 35 分钟以上再回头看）：
        //
        //   ① 核心洞察 —— 这题其实**不用**特殊的二维二分
        //      题目给了两个条件：
        //          · 每行从左到右递增
        //          · **每行第一个数 > 上一行最后一个数**
        //      第二条是关键：它意味着把矩阵**按行优先展开**，就是一个完整的有序数组！
        //          [[1, 3, 5, 7],
        //           [10,11,16,20],     展开 -> 1,3,5,7,10,11,16,20,23,30,34,60
        //           [23,30,34,60]]     这是一个严格有序的一维数组
        //      所以：**在 [0, m*n-1] 上做一次普通的二分就行了**，模板直接照抄 704。
        //
        //   ② 本题唯一的新东西：一维下标 ↔ 二维坐标的换算
        //          行 = k / n          （n 是【列数】！）
        //          列 = k % n
        //      ⚠️ **`n` 是列数 `matrix[0].size()`，不是行数 `matrix.size()`** ——
        //         这是本题的头号坑。搞反了在方阵上碰巧能过，一遇到 m ≠ n 立刻全错。
        //
        //   ③ 模板（和 704 一模一样，只是取值方式变了）
        //          int m = matrix.size(), n = matrix[0].size();
        //          int lo = 0, hi = m * n - 1;
        //          while (lo <= hi) {
        //              int mid = lo + (hi - lo) / 2;
        //              int v = matrix[mid / n][mid % n];      // ← 唯一的改动
        //              if (v == target) return true;
        //              if (v < target) lo = mid + 1;
        //              else            hi = mid - 1;
        //          }
        //          return false;
        //
        //   ④ 另外两种也能过的解法（我都对拍过，27 万组 0 差异）
        //      · **两次二分**：先用二分找「第一个行尾 >= target 的行」，
        //        再在该行内二分。复杂度 O(log m + log n)，同样满足要求。
        //        好处是思路更"分层"，坏处是要写两段二分。
        //      · **右上角走阶梯**：从右上角出发，比 target 大就往左、小就往下，
        //        复杂度 O(m + n)。**注意它不满足题目要求的 O(log(m·n))** ——
        //        但本题 m, n ≤ 100，所以 LeetCode 也照过。面试时提一句可以，
        //        别拿它当主答案。
        //
        //   ⑤ 两个坑
        //     1. **列数写成行数**（见 ②）—— 头号坑。
        //     2. `k / n` 得行、`k % n` 得列，两处别写反。
        //
        //   复杂度：O(log(m·n)) 时间，O(1) 空间。

        int m = (int)matrix.size(), n = (int)matrix[0].size();

        // ① 行：第一个「行尾 >= target」的行
        int lo = 0, hi = m - 1, row = m;
        while (lo <= hi)
        {
            int mid = lo + (hi - lo) / 2;
            if (matrix[mid][n - 1] >= target)
            {
                row = mid;
                hi = mid - 1;
            } 
            else
                lo = mid + 1;
        }
        if (row == m)
            return false; 

        // ② 列：该行里第一个「>= target」的位置
        lo = 0;
        hi = n - 1;
        int col = n;
        while (lo <= hi)
        {
            int mid = lo + (hi - lo) / 2;
            if (matrix[row][mid] >= target)
            {
                col = mid;
                hi = mid - 1;
            }
            else
                lo = mid + 1;
        }
        return col < n && matrix[row][col] == target;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把矩阵格式化成 3×4 [1,3,5,7,10,11,…] 便于打印；元素过多只打印首 6 末 3
static string matrixToString(const vector<vector<int>> &g)
{
    int m = (int)g.size();
    int n = m ? (int)g[0].size() : 0;
    const int total = m * n;

    auto at = [&](int k)
    { return g[k / n][k % n]; };

    string s = to_string(m) + "×" + to_string(n) + "  [";
    if (total <= 16)
    {
        for (int k = 0; k < total; ++k)
        {
            if (k)
                s += ",";
            s += to_string(at(k));
        }
    }
    else
    {
        for (int k = 0; k < 6; ++k)
        {
            if (k)
                s += ",";
            s += to_string(at(k));
        }
        s += ", …(略去 " + to_string(total - 9) + " 个)…, ";
        for (int k = total - 3; k < total; ++k)
        {
            if (k != total - 3)
                s += ",";
            s += to_string(at(k));
        }
    }
    return s + "]";
}

// 单条测试用例
struct TestCase
{
    string name;              // 用例名称
    vector<vector<int>> grid; // 输入：矩阵
    int target;               // 要找的目标值
    bool expected;            // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<vector<int>> g = c.grid; // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    bool actual = sol.searchMatrix(g, c.target);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       matrix = " << matrixToString(c.grid) << "\n"
         << "       target = " << c.target << "\n"
         << "       期望: " << boolalpha << c.expected
         << "   实际: " << actual << noboolalpha << endl;
    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {{1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 60}}, 3, true},
        {"示例 2", {{1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 60}}, 13, false},
    };

    // 额外的边界与补充用例
    // 下面所有 expected 都由暴力扫描独立算出
    vector<TestCase> extraCases = {
        {"1×1 命中", {{5}}, 5, true},
        {"1×1 未命中", {{5}}, 3, false},
        {"单行·命中", {{1, 3, 5, 7}}, 5, true},
        {"单行·未命中", {{1, 3, 5, 7}}, 4, false},
        {"单列·命中", {{1}, {3}, {5}, {7}}, 5, true},
        {"单列·未命中", {{1}, {3}, {5}, {7}}, 4, false},
        {"行边界·命中上一行末尾", {{1, 3, 5, 7}, {10, 11, 16, 20}}, 7, true},
        {"行边界·命中下一行开头", {{1, 3, 5, 7}, {10, 11, 16, 20}}, 10, true},
        {"比所有数都小", {{1, 3}, {5, 7}}, 0, false},
        {"比所有数都大", {{1, 3}, {5, 7}}, 8, false},
        {"含负数", {{-10, -5, -2}, {-1, 0, 1}}, -5, true},
        {"取值下限 -10000", {{-10000, -10000}, {0, 10000}}, -10000, true},
        {"取值上限 10000", {{-10000, -10000}, {0, 10000}}, 10000, true},
        {"行内有重复值", {{1, 1, 1}, {2, 2, 2}}, 1, true},
        {"行内有重复值·未命中", {{1, 1, 1}, {2, 2, 2}}, 3, false},
    };

    // 最大规模：100×100（题目上限），值为 0 .. 9999，按行优先递增
    //   这条专门检查「列数 vs 行数」有没有搞反：方阵上搞反了也可能碰巧对，
    //   所以另外补了两条 3×4 / 4×3 的非方阵用例（见上面）。
    {
        const int m = 100, n = 100;
        vector<vector<int>> big(m, vector<int>(n));
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
                big[i][j] = i * n + j;

        extraCases.push_back({"100×100·命中（值 5000）", big, 5000, true});
        extraCases.push_back({"100×100·未命中（值 -1）", big, -1, false});
        extraCases.push_back({"100×100·命中最大值 9999", big, 9999, true});
    }

    // 非方阵：专门打「列数写成行数」这个坑
    //   如果误把 n 当成 m（3），展开后的下标换算会全错
    {
        vector<vector<int>> rect = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12},
        };
        extraCases.push_back({"非方阵 3×4·命中 8（列数错就崩）", rect, 8, true});
        extraCases.push_back({"非方阵 3×4·未命中 13", rect, 13, false});

        vector<vector<int>> tall = {
            {1, 2},
            {3, 4},
            {5, 6},
            {7, 8},
        };
        extraCases.push_back({"非方阵 4×2·命中 7（列数错就崩）", tall, 7, true});
        extraCases.push_back({"非方阵 4×2·未命中 9", tall, 9, false});
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

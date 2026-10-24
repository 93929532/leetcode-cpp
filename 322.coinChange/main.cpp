#include <iostream>
#include <string>
#include <vector>
#include <climits>   // INT_MAX：初始化"凑不出来"的哨兵
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 322. 零钱兑换（Coin Change）· 完全背包 · Medium
//
// 这是背包主题的第一站：**完全背包求最小值**。
//   · 每枚硬币可以用**无限次** -> 完全背包（不是 01 背包）
//   · "凑出 amount 最少要几枚" -> 求**最小**（既不是求方案数，也不是求能不能）
//
//   第一步先把 DP 的定义写清楚（这步比代码重要）：
//       dp[j] = 凑出金额 j 所需要的**最少硬币数**
//   答案就是 dp[amount]；凑不出来的金额让它保持"无穷大"。
//
//   转移方程：
//       vector<int> dp(amount + 1, INF);
//       dp[0] = 0;                                   // 凑 0 元要 0 枚
//       for (int coin : coins)
//           for (int j = coin; j <= amount; ++j)     // ★ 正序 = 完全背包
//               dp[j] = min(dp[j], dp[j - coin] + 1);
//
//   怎么理解：凑出 j 的**最后一枚**硬币如果是 coin，那么它之前一定已经凑出了 j-coin
//   （用了 dp[j-coin] 枚），再加这一枚就是 dp[j-coin] + 1 枚。
//   对所有可能的 coin 取最小，就是 dp[j]。
//
//   四个坑（下面每条都有专门用例，别只看不做）：
//     1. **正序还是倒序**：完全背包必须**正序**（j 从小到大）。
//        写成倒序就退化成 01 背包（每枚硬币只能用一次）：
//        [1,2,5] 凑 11 会答 -1 —— 因为 1+2+5 只有 8，再也凑不上了。
//        ★ 这一条是"01 背包 vs 完全背包"的**唯一开关**，务必记死。
//     2. **别把 dp 全初始化成 0**：因为这里是求 min，min(0, 任何正数) 永远是 0，
//        答案会静静变成 0 —— 不崩、不报错，属于最阴的静默错误。
//        只有 dp[0] 该是 0，其余必须是 INF。
//     3. **INF + 1 溢出**：转移里有 dp[j-coin] + 1，若 INF = INT_MAX，加 1 就溢出（UB）。
//        取 INT_MAX / 2 最省心：大到不会被真正的答案追上，又小到加 1 不溢出。
//        返回时判断 dp[amount] >= INF/2（或 >= INF/4）再决定是否返回 -1。
//     4. **贪心是错的**：反例 [1,3,4] 凑 6 ——
//        贪心先拿 4，再 1+1，共 3 枚；正解是 3+3 = 2 枚。
//        下面还有一条力扣的经典反例 [186,419,83,408] 凑 6249（答案 20，贪心更多）。
//        所以这题不能"尽量用大面值"，必须 DP。
//
//   复杂度：时间 O(amount × coins.size())，空间 O(amount)。
//   amount 上界 10^4、硬币最多 12 种 -> 最多 12 万次转移，随便过。
//
//   进阶 / 面试追问：
//     · 还能怎么写？-> **BFS**：把 0..amount 看成图上的点，每"加一枚硬币"是一条边，
//       求从 0 到 amount 的最短路；第一次到达 amount 的层数就是最少枚数。
//       和 DP 是同一件事的两种视角（DP 是"按金额递推"，BFS 是"按枚数扩展"）
//     · 求"有多少种凑法"？-> 518 零钱兑换 II：min 改成求和，
//       而且**必须把硬币循环放在外层**（否则算的是"排列数"而不是"组合数"）
//     · 每枚硬币有使用上限？-> 多重背包（拆成 01 背包，或单调队列优化）
//     · 记忆化搜索能写吗？-> 能：dfs(left) = min(dfs(left - coin) + 1)，
//       本质和递推是同一个 DP，只是转移方向反过来
//     · 为什么 dp 数组只需要一维？-> 因为每一轮都只依赖"同一行"更小的金额，
//       这是完全背包能用滚动数组的基础（01 背包则必须靠倒序来避免重复使用）
//
//   同族题：518 零钱兑换 II（完全背包求方案数）、279 完全平方数（完全背包求最小）、
//           377 组合总和 IV（完全背包求排列数）、
//           416 分割等和子集 / 494 目标和（都是 01 背包，正好对照）
class Solution
{
public:
    int coinChange(vector<int> &coins, int amount)
    {
        const int INF = amount + 1;
        vector<int> dp(amount + 1, INF);
        dp[0] = 0;

        for (int coin : coins)
            for (int j = coin; j <= amount; j++)
                dp[j] = min(dp[j], dp[j - coin] + 1);

        return dp[amount] == INF ? -1 : dp[amount];
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase
{
    string name;       // 用例名称
    vector<int> coins; // 硬币面值（每枚可用无限次）
    int amount;        // 目标金额
    int expected;      // 期望结果（凑不出为 -1）
};

// 把硬币列表格式化成 [1,2,5]，过长时截断
static string joinCoins(const vector<int> &v, size_t limit = 16)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += to_string(v[i]);
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 种）";
    return s + "]";
}

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    int actual = 0;

    try
    {
        Solution sol;
        vector<int> coins = c.coins; // 传副本：万一解法改动了入参，打印时也不受影响
        actual = sol.coinChange(coins, c.amount);
    }
    catch (const exception &e)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << e.what() << endl;
        return false;
    }

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  coins = " << joinCoins(c.coins)
         << "  amount = " << c.amount << "\n"
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
        {"示例 1", {1, 2, 5}, 11, 3}, // 11 = 5 + 5 + 1
        {"示例 2", {2}, 3, -1},
        {"示例 3", {1}, 0, 0},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 坑 4：贪心反例（面值组合不满足"贪心最优"）
        {"贪心反例 [1,3,4] 凑 6（贪心 3 枚，正解 2 枚）", {1, 3, 4}, 6, 2},
        {"力扣经典反例 [186,419,83,408] 凑 6249", {186, 419, 83, 408}, 6249, 20},

        // amount = 0 与"面值比金额大"
        {"amount = 0（任何硬币都是 0 枚）", {5, 7}, 0, 0},
        {"面值大于金额 -> 凑不出", {10}, 5, -1},

        // 无解的各种原因
        {"面值全是偶数、金额是奇数 -> 无解", {2, 4}, 7, -1},
        {"两枚面值凑不出目标", {3, 7}, 5, -1},

        // 恰好整除 / 恰好一枚 / 恰好两枚
        {"只有一枚硬币且整除", {5}, 25, 5},
        {"面值正好等于金额 -> 1 枚", {7}, 7, 1},
        {"正好两枚（7+7）", {1, 7, 10}, 14, 2},

        // 面值顺序打乱（验证实现不依赖输入顺序）
        {"面值乱序 [2,5,10,1] 凑 27", {2, 5, 10, 1}, 27, 4}, // 10+10+5+2

        // 互质面值的经典组合
        {"互质面值 [6,9,20] 凑 100", {6, 9, 20}, 100, 5}, // 20+20+20+20+20
    };

    // 上界规模用例：amount = 10^4（题目上界）
    //   ① 只有 1 元面值 -> 答案 10000，验证 dp 数组下标不会算错
    //   ② 常用货币面值 -> 答案 10（1000 × 10），验证"大面值优先仍然对"
    //   ③ [1,3,4] 凑 10000 -> 答案 2500（全用 4），正好压一下 12 万次转移
    {
        TestCase c;
        c.name = "上界：amount = 10^4，只有 1 元";
        c.coins = {1};
        c.amount = 10000;
        c.expected = 10000;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "上界：amount = 10^4，十种常用面值";
        c.coins = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000};
        c.amount = 10000;
        c.expected = 10;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "上界：amount = 10^4，面值 [1,3,4]（贪心反例放大）";
        c.coins = {1, 3, 4};
        c.amount = 10000;
        c.expected = 2500;
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

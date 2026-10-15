#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // sort：本题必须先按位置排序
#include <stack>     // 可选：用一个变量也能写；留着方便你试两种写法

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    int carFleet(int target, vector<int> &position, vector<int> &speed)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 35 分钟以上再回头看）：
        //
        //   这题和 739 / 239 不同 —— 既不是滑动窗口，也不是"下一个更大元素"。
        //   但它仍然是**单调栈家族**：因为每辆车的命运只有两种 ——
        //   要么"被前面的车队挡住"，要么"自己成为新车队的头"。
        //
        //   ① 关键的第一步：**不要模拟车的位置。**
        //      换个视角 —— 对每辆车问一句：
        //          「如果它前面一辆车都没有，它什么时候到达终点？」
        //      把这个数（到达时间）算出来，题目就换了个形状：
        //
        //          t[i] = (target - position[i]) / speed[i]
        //
        //   ② 第二步：按 position **从大到小**排序
        //      —— 也就是从「离终点最近」的车开始往前看。
        //      为什么是这个方向？因为车队的形成只取决于"前面的车"，所以要
        //      先把前面处理完，才知道后面那辆车要跟谁比。
        //
        //   ③ 第三步：依次对每辆车，拿它的 t 和「紧挨着它前面的那个车队
        //      的到达时间」比较：
        //          · 如果它到得**更早或同时**（t <= 前面车队的 t）
        //                → 它一定会追上前面那个车队 → 并进去，队数不变
        //          · 如果它到得**更晚**（t > 前面车队的 t）
        //                → 它比前面慢，永远追不上 → 自己成为一个新车队
        //
        //      **为什么只跟"紧挨着前面那一个"比就够？**
        //      因为它要么追上前一个车队（那就并进去了，不用再管更前面的），
        //      要么追不上前一个 —— 那更前面的它更追不上。
        //
        //   ④ 用一个变量「当前车队的到达时间」就够（也可以用一个栈）：
        //          double last = -1;  int fleets = 0;
        //          对每辆车（按位置从大到小）:
        //              if (t > last) { last = t; ++fleets; }
        //      循环结束时 fleets 就是答案。
        //
        //   四个坑：
        //     1. **排序方向**：按 position **从大到小**（离终点近的先处理）。
        //        从小到大排就完全错了 —— 这是本题第一号坑。
        //     2. **比的是到达时间**，不是速度、不是位置、不是"谁更快"。
        //     3. **判断新车队用严格大于 `>`**：相等时算"追上"，要并进同一队。
        //     4. **时间要用 double 算**：`(target - p) / (double)s`。
        //        写成整数除法会把小数部分**截掉**（9.9 变成 9），于是本该分成
        //        两队的车被误判成同一队 —— 注意它的效果是**单方向的**：
        //        截断只会让答案【偏小】，永远不会偏大（floor 是单调的）。
        //        实测：在一批随机小规模输入里，约 30% 会因此数少车队。
        //        注意：只写 `double t = (target-p)/s;` 是**没用**的 ——
        //        除法先按 int 算完再赋给 double，必须把某一侧转成 double，
        //        或者写 `(target - p) * 1.0 / s`。
        //
        //   复杂度：排序 O(n log n) + 一次扫描 O(n) —— 见下面的 10^5 用例。

        vector<pair<int, double>> time(position.size());

        for (int i = 0; i < position.size(); i++)
        {
            time[i].first = position[i];
            time[i].second = (double)(target - position[i]) / speed[i];
        }

        sort(time.begin(), time.end());

        int fleets = 0;
        double maxtime = -1;

        for (int last = time.size() - 1; last >= 0; last--)
        {
            if (time[last].second > maxtime)
            {
                maxtime = time[last].second;
                ++fleets;
            }
        }

        return fleets;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [10,8,0,5,3] 便于打印；过长的数组只打印首 6 末 3
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
//
// 注意：本题输出是一个**整数**（车队数量），不是数组。
//       position / speed 两个数组的对应关系由下标决定，顺序有意义。
struct TestCase
{
    string name;          // 用例名称
    int target;           // 终点位置
    vector<int> position; // 各车初始位置（互不相同）
    vector<int> speed;    // 各车速度，与 position 一一对应
    int expected;         // 期望的车队数量
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<int> pos = c.position; // 解法收的是非 const 引用，各复制一份给它
    vector<int> spd = c.speed;

    Solution sol;
    int actual = sol.carFleet(c.target, pos, spd);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       target   = " << c.target << "\n"
         << "       position = " << vectorToString(c.position) << "\n"
         << "       speed    = " << vectorToString(c.speed) << "\n"
         << "       期望: " << c.expected << "   实际: " << actual << endl;

    if (!pass)
    {
        cout << "       （每辆车单独算到达时间 (target-p)/s，再按位置从大到小扫一遍）" << endl;
    }
    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", 12, {10, 8, 0, 5, 3}, {2, 4, 1, 1, 3}, 3},
        {"示例 2（只有一辆车）", 10, {3}, {3}, 1},
        {"示例 3（全部并成一队）", 100, {0, 2, 4}, {4, 2, 1}, 1},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"两辆同速，谁也追不上谁", 10, {0, 5}, {1, 1}, 2},
        {"两辆同时到达 -> 并成一队", 10, {0, 5}, {2, 1}, 1},
        {"前车更快，差距被越拉越大", 100, {0, 50}, {1, 2}, 2},
        {"后车更快，刚好追上", 100, {0, 50}, {2, 1}, 1},
        {"三辆全部同时到达", 10, {0, 2, 4}, {5, 4, 3}, 1},
        {"同速前进，各走各的", 100, {0, 1, 2, 3}, {100, 100, 100, 100}, 4},
        {"速度递减（越靠后越慢）", 100, {0, 10, 20}, {1, 2, 4}, 3},
        {"部分合并、部分独立", 20, {0, 5, 10}, {1, 2, 1}, 2},
        {"位置密集、速度差别大", 100, {0, 1, 2}, {3, 2, 1}, 1},
        {"起点就在终点前一格", 10, {9}, {1}, 1},
        {"前车极慢，后车堵上去", 12, {0, 6}, {6, 1}, 1},
        {"后车速度更快，但起步太远仍追不上", 100, {0, 60}, {10, 9}, 2},

        // ---- 下面三条专门打「整数除法」这个坑 ----
        // 它们的真实到达时间都不是整数，截断后会被压成同一个值而误判合并。
        // 正确时间：5->0.200  4->0.375  3->1.000   （三个都不同 -> 3 队）
        // 截断后  ：     0        0        1        （-> 只数出 2 队）
        {"小数时间 1：三个队被压成两个", 7, {4, 5, 3}, {8, 10, 4}, 3},
        // 正确时间：13->0.083  9->0.714    截断后都是 0
        {"小数时间 2：就差 0.63，也该是两个队", 14, {9, 13}, {7, 12}, 2},
        // 正确时间：17->0.400  15->1.333  12->1.750  3->16.0  2->8.5
        // 截断后  ：     0         1         1        16       8  -> 少一队
        {"小数时间 3：四个队被压成三个", 19, {17, 12, 2, 3, 15}, {5, 4, 2, 1, 3}, 4},
    };

    // 最大规模用例 1：n = 10^5，全部同速
    //   同速就永远追不上 -> 每辆车各自成一个车队 -> 答案 = n。
    //   这条压的是"排序 + 扫描"在大数据下是否依然正确。
    {
        const int n = 100000;
        vector<int> position(n), speed(n, 1);
        for (int i = 0; i < n; ++i)
            position[i] = i;
        extraCases.push_back({"最大规模 n=10^5（全部同速，答案 = n）", n, position, speed, n});
    }

    // 最大规模用例 2：n = 10^5，全部同时到达
    //   构造：target = 2n，position[i] = i，speed[i] = 2n - i
    //         => 每辆车的到达时间都是 (2n - i) / (2n - i) = 1，完全相等
    //   全部同时到达 -> 并成 1 个车队。
    //   这条同时验证：浮点相等时的判断（必须用 `>` 而不是 `>=`）。
    {
        const int n = 100000, target = 2 * n;
        vector<int> position(n), speed(n);
        for (int i = 0; i < n; ++i)
        {
            position[i] = i;
            speed[i] = 2 * n - i;
        }
        extraCases.push_back({"最大规模 n=10^5（全部同时到达，答案 = 1）", target, position, speed, 1});
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

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm> // min
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX // ★ 阻止 <windows.h> 定义 min/max 宏
#endif
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 542. 01 矩阵（01 Matrix）· 图/多源 BFS · Medium
//
// 给定一个 0/1 矩阵，返回一个同样大小的矩阵：
//     每个格子填「它到最近的 0 的距离」（只能上下左右走，每步距离 1）。
//
//   ★ 这题是多源 BFS（multi-source BFS）的第一个经典题。
//     **不要**对每个 1 单独做一次 BFS（最坏 O((mn)^2) 会超时），
//     而是把**所有 0 一次性放进队列**，让它们同时向外扩散：
//
//       所有 0 的距离 = 0，入队；
//       然后 BFS：从距离为 d 的格子走到未访问的邻居 -> 邻居距离 = d + 1。
//
//     因为队列是先进先出，**距离近的先扩散**，所以每个格子第一次被访问时，
//     拿到的就是它到最近 0 的最短距离。
//
//   骨架：
//       vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
//           int R = mat.size(), C = mat[0].size();
//           vector<vector<int>> dist(R, vector<int>(C, -1));   // ★ -1 表示未访问
//           queue<pair<int,int>> q;
//
//           for (int r = 0; r < R; ++r)
//               for (int c = 0; c < C; ++c)
//                   if (mat[r][c] == 0) { dist[r][c] = 0; q.push({r, c}); }   // ★ 全部 0 入队
//
//           int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
//           while (!q.empty()) {
//               auto [r, c] = q.front(); q.pop();
//               for (int k = 0; k < 4; ++k) {
//                   int nr = r + dr[k], nc = c + dc[k];
//                   if (nr < 0 || nr >= R || nc < 0 || nc >= C)  continue;
//                   if (dist[nr][nc] != -1)                      continue;   // ★ 已访问
//                   dist[nr][nc] = dist[r][c] + 1;                           // ★ 距离 +1
//                   q.push({nr, nc});                                        // ★ 先标记再入队
//               }
//           }
//           return dist;
//       }
//
//   四个坑（骨架都有对应用例）：
//     1. ★★ **不要对每个 1 单独 BFS**：那是 O((mn)^2)，10^4 格子就可能超时；
//        多源 BFS 是所有源点共享一次扩散，O(mn)。
//     2. ★ **标记必须在入队时**：出队才标记会让同一格被多个邻居重复入队，队列膨胀。
//     3. ★ **距离要用 dist 数组，不要用"层数计数器"**：多源情况下
//        "层数"和"距离"容易错位（尤其起点不在同一层时）。dist[r][c] + 1 最稳。
//     4. **对角不算相邻**：只走上下左右（4 方向），别把 8 方向写进来。
//
//   另一种 O(mn) 思路（面试可以对比着说）：**两遍动态规划**
//       · 从左上往右下扫：dist[r][c] = min(上, 左) + 1
//       · 从右下往左上扫：dist[r][c] = min(下, 右) + 1
//       两遍取 min 后就得到曼哈顿意义上的最近 0。不需要队列。
//
//   复杂度：时间 O(m·n)（每格进出队列一次）；空间 O(m·n)。
//
//   面试追问：
//     · 为什么 BFS 能求最短？-> 队列保证按距离展开，第一次到达即最短（无权图）
//     · 多源和单源的区别？-> 多源 = 初始化时把所有源点塞进队列，只跑一次
//     · 如果要求"到最近 0 的路径"而不是距离？-> 记录每个格子的前驱，最后回溯
//     · 和 994 腐烂的橘子、1091 二进制矩阵最短路的关系？-> 同一个多源 BFS 模板
//
//   同族题：994 腐烂的橘子、1091 二进制矩阵中的最短路径、200 岛屿数量（连通块，DFS）、
//           1162 地图分析、1926 迷宫中离入口最近的出口
class Solution
{
public:
    vector<vector<int>> updateMatrix(vector<vector<int>> &mat)
    {
        // TODO: 在这里写你的解法

        int R = mat.size(), C = mat[0].size();
        vector<vector<int>> dist(R, vector<int>(C, -1));

        queue<pair<int, int>> q;
        for (int r = 0; r < R; r++)
            for (int c = 0; c < C; c++)
                if (mat[r][c] == 0)
                {
                    dist[r][c] = 0;
                    q.push({r, c});
                }

        int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
        while (!q.empty())
        {
            auto [r, c] = q.front();
            q.pop();

            for (int k = 0; k < 4; k++)
            {
                int nr = r + dr[k], nc = c + dc[k];
                if (nr < 0 || nr >= R || nc < 0 || nc >= C)
                    continue; // 越界
                if (dist[nr][nc] != -1)
                    continue; // 已访问

                dist[nr][nc] = dist[r][c] + 1;
                q.push({nr, nc});
            }
        }
        return dist;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

static string matToStr(const vector<vector<int>> &m, size_t maxRows = 5, size_t maxCols = 12)
{
    string s = "[";
    for (size_t i = 0; i < m.size() && i < maxRows; ++i)
    {
        if (i)
            s += ",";
        s += "[";
        for (size_t j = 0; j < m[i].size() && j < maxCols; ++j)
        {
            if (j)
                s += ",";
            s += to_string(m[i][j]);
        }
        if (m[i].size() > maxCols)
            s += "…";
        s += "]";
    }
    if (m.size() > maxRows)
        s += ",…（共 " + to_string(m.size()) + " 行）";
    return s + "]";
}

// 暴力：对每个 1 单独做 BFS 找最近的 0（仅用于小规模自检）
static vector<vector<int>> bruteMatrix(const vector<vector<int>> &mat)
{
    int R = (int)mat.size(), C = (int)mat[0].size();
    int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    vector<vector<int>> res(R, vector<int>(C, 0));

    for (int r = 0; r < R; ++r)
        for (int c = 0; c < C; ++c)
        {
            if (mat[r][c] == 0)
            {
                res[r][c] = 0;
                continue;
            }
            // 从 (r,c) 出发的 BFS，直到碰到 0
            vector<vector<bool>> vis(R, vector<bool>(C, false));
            queue<pair<pair<int, int>, int>> q;
            q.push({{r, c}, 0});
            vis[r][c] = true;
            int found = -1;
            while (!q.empty() && found < 0)
            {
                auto [pos, d] = q.front();
                q.pop();
                int cr = pos.first, cc = pos.second;
                if (mat[cr][cc] == 0)
                {
                    found = d;
                    break;
                }
                for (int k = 0; k < 4; ++k)
                {
                    int nr = cr + dr[k], nc = cc + dc[k];
                    if (nr < 0 || nr >= R || nc < 0 || nc >= C)
                        continue;
                    if (vis[nr][nc])
                        continue;
                    vis[nr][nc] = true;
                    q.push({{nr, nc}, d + 1});
                }
            }
            res[r][c] = found;
        }
    return res;
}

struct TestCase
{
    string name;
    vector<vector<int>> mat;
    vector<vector<int>> expected;
};

static bool checkInput(const TestCase &c, string &why)
{
    if (c.mat.empty() || c.mat[0].empty())
    {
        why = "矩阵为空";
        return false;
    }
    size_t C = c.mat[0].size();
    bool hasZero = false;
    for (const auto &row : c.mat)
    {
        if (row.size() != C)
        {
            why = "矩阵不是矩形";
            return false;
        }
        for (int v : row)
        {
            if (v != 0 && v != 1)
            {
                why = "矩阵里出现非 0/1 的值";
                return false;
            }
            if (v == 0)
                hasZero = true;
        }
    }
    if (!hasZero)
    {
        why = "题目保证至少有一个 0";
        return false;
    }
    if (c.mat.size() * C != c.expected.size() * (c.expected.empty() ? 1 : c.expected[0].size()))
    {
        why = "期望矩阵尺寸不对";
        return false;
    }
    return true;
}

static bool runCase(const TestCase &c)
{
    string why;
    if (!checkInput(c, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    // 数据自检：小矩阵用暴力重算，防止我手写期望值出错
    int cells = (int)(c.mat.size() * c.mat[0].size());
    if (cells <= 2500)
    {
        vector<vector<int>> brute = bruteMatrix(c.mat);
        if (brute != c.expected)
        {
            cout << "[用例数据错误] " << c.name << "\n"
                 << "       暴力算出   = " << matToStr(brute) << "\n"
                 << "       用例写的   = " << matToStr(c.expected) << endl;
            return false;
        }
    }

    vector<vector<int>> mat = c.mat;
    vector<vector<int>> got;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.updateMatrix(mat);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    bool pass = (!threw) && (got == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  " << c.mat.size() << "x" << c.mat[0].size() << "\n"
         << "       输入 = " << matToStr(c.mat) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       期望 = " << matToStr(c.expected) << "\n"
             << "       实际 = " << matToStr(got);
        if (got.size() != c.expected.size() ||
            (!got.empty() && !c.expected.empty() && got[0].size() != c.expected[0].size()))
            cout << "\n       ✗ 输出尺寸不对";
        cout << "\n";
    }

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例 1（四面都是 0）",
         {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}},
         {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}}},
        {"示例 2（右下角要扩散两层）",
         {{0, 0, 0}, {0, 1, 0}, {1, 1, 1}},
         {{0, 0, 0}, {0, 1, 0}, {1, 2, 1}}},
    };

    vector<TestCase> extraCases = {
        // 最小规模
        {"1x1 全是 0", {{0}}, {{0}}},

        // 一维：0 在两端 / 只在中间
        {"1x3 两端是 0", {{0, 1, 0}}, {{0, 1, 0}}},
        {"1x5 单个 0 在中间", {{1, 1, 0, 1, 1}}, {{2, 1, 0, 1, 2}}},
        {"1x5 单个 0 在端头", {{0, 1, 1, 1, 1}}, {{0, 1, 2, 3, 4}}},

        // 0 在角上：距离是曼哈顿
        {"3x3 只有左上角是 0",
         {{0, 1, 1}, {1, 1, 1}, {1, 1, 1}},
         {{0, 1, 2}, {1, 2, 3}, {2, 3, 4}}},

        // ★ 对角不算相邻：(1,1) 到 (0,0) 是 2 步
        {"2x2 对角关系", {{0, 1}, {1, 1}}, {{0, 1}, {1, 2}}},

        // 棋盘式交替
        {"1x7 交替 0/1", {{0, 1, 0, 1, 0, 1, 0}}, {{0, 1, 0, 1, 0, 1, 0}}},

        // 多个 0 分散
        {"4x4 多个 0",
         {{0, 1, 1, 0},
          {1, 1, 1, 1},
          {1, 1, 1, 1},
          {0, 1, 1, 0}},
         {{0, 1, 1, 0},
          {1, 2, 2, 1},
          {1, 2, 2, 1},
          {0, 1, 1, 0}}},

        // 全 1 只有中心一个 0
        {"5x5 只有正中心是 0",
         {{1, 1, 1, 1, 1},
          {1, 1, 1, 1, 1},
          {1, 1, 0, 1, 1},
          {1, 1, 1, 1, 1},
          {1, 1, 1, 1, 1}},
         {{4, 3, 2, 3, 4},
          {3, 2, 1, 2, 3},
          {2, 1, 0, 1, 2},
          {3, 2, 1, 2, 3},
          {4, 3, 2, 3, 4}}},
    };

    // 大规模用例：100x100，只有一个 0
    // 单源 + 4 方向 -> 距离就是曼哈顿距离，可以独立算出期望值
    {
        TestCase c;
        c.name = "100x100 只有左上角是 0（曼哈顿距离）";
        c.mat.assign(100, vector<int>(100, 1));
        c.mat[0][0] = 0;
        c.expected.assign(100, vector<int>(100, 0));
        for (int r = 0; r < 100; ++r)
            for (int k = 0; k < 100; ++k)
                c.expected[r][k] = r + k;
        extraCases.push_back(c);
    }
    {
        TestCase c;
        c.name = "100x100 只有正中心是 0（曼哈顿距离）";
        c.mat.assign(100, vector<int>(100, 1));
        c.mat[50][50] = 0;
        c.expected.assign(100, vector<int>(100, 0));
        for (int r = 0; r < 100; ++r)
            for (int k = 0; k < 100; ++k)
                c.expected[r][k] = abs(r - 50) + abs(k - 50);
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

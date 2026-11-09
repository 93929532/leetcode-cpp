#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set> // 环检测 + 释放去重
#include <algorithm>     // max
#include <climits>       // INT_MIN
#include <stdexcept>     // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// 二叉树节点（与 LeetCode 定义一致）
struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *l, TreeNode *r) : val(x), left(l), right(r) {}
};

// ===================== 答题区域 =====================
// 1448. 统计二叉树中好节点的数目（Count Good Nodes in Binary Tree）· 树/DFS · Medium
//
// "好节点"的定义：从**根**到这个节点的路径上，**没有**比它更大的值。
// 换句话说：这个节点的值 ≥ 路径上所有祖先（含自己）的最大值。
//
//   ★★ 关键：判断依据是"**整条路径**的最大值"，不是"父节点"！
//      反例：
//           5
//          /
//         3
//        /
//       4
//      节点 4 比父节点 3 大，但路径 5→3→4 上有 5 > 4，所以 4 **不是**好节点。
//      （骨架里专门放了这条用例，抓"只比较父节点"的错。）
//
//   做法：DFS 往下走时**带着路径上的最大值** maxSoFar：
//       int dfs(TreeNode* n, int maxSoFar) {
//           if (n == nullptr) return 0;
//           int good = (n->val >= maxSoFar) ? 1 : 0;     // 自己是不是好节点
//           int newMax = max(maxSoFar, n->val);          // ★ 更新后传给两个孩子
//           return good + dfs(n->left,  newMax)
//                       + dfs(n->right, newMax);
//       }
//       int goodNodes(TreeNode* root) { return dfs(root, INT_MIN); }
//
//   三个坑（骨架都有对应用例）：
//     1. ★ **初值必须用 `INT_MIN`，不能用 0**：节点值可以是负数，
//        用 0 当初始最大值会让所有负数节点都判成"不是好节点"（连根都错）。
//     2. ★ **更新最大值必须在递归孩子之前**，且要把 `newMax` 传下去；
//        传 `maxSoFar`（旧值）会让兄弟子树互相影响、结果错。
//     3. **不能只比较父节点**（见上面的反例）。
//
//   迭代写法（队列 / 栈，把 maxSoFar 一起打包）：
//       queue<pair<TreeNode*, int>> q;
//       q.push({root, INT_MIN});
//       while (!q.empty()) {
//           auto [n, mx] = q.front(); q.pop();
//           if (n->val >= mx) ++ans;
//           int nm = max(mx, n->val);
//           if (n->left)  q.push({n->left,  nm});
//           if (n->right) q.push({n->right, nm});
//       }
//     ★ 本题节点数上限 **10^5**，链状树递归深度可能到 10^5 层 ——
//       递归版在深链上**可能爆栈**（骨架外验证过），要绝对稳妥就写迭代。
//
//   复杂度：时间 O(n)（每个节点访问一次）；空间 O(h)（递归栈或显式栈）。
//
//   面试追问：
//     · 如果要求"路径上所有节点都不小于它"？-> 那就变成"路径最小值"，只需换个极值
//     · 和 257 二叉树的所有路径、112 路径总和有什么关系？-> 都是"边递归边带状态"
//       （带的是 path / 累加和 / 路径最大值），是这个套路的不同实例
//     · 能不能不传参，用回溯（进入时改、退出时还原）？-> 可以，
//       用一个成员变量 maxSoFar，注意退出时要恢复
//
//   同族题：257 二叉树的所有路径、112 路径总和、113 路径总和 II、
//           129 求根节点到叶节点数字之和、124 二叉树中的最大路径和
class Solution
{
private:
    int sum;
    void countGoodNODE(TreeNode *root, int best)
    {
        if (root == nullptr)
            return;

        if (root->val >= best)
        {
            sum++;
            best = root->val;
        }

        countGoodNODE(root->left, best);
        countGoodNODE(root->right, best);
    }

public:
    int goodNodes(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        sum = 0;
        countGoodNODE(root, INT_MIN);

        return sum;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域是 -10^4..10^4，用 100000 当 null
static const int NULLNODE = 100000;

static TreeNode *buildTree(const vector<int> &a)
{
    if (a.empty() || a[0] == NULLNODE)
        return nullptr;
    TreeNode *root = new TreeNode(a[0]);
    queue<TreeNode *> q;
    q.push(root);
    size_t i = 1;
    while (!q.empty() && i < a.size())
    {
        TreeNode *cur = q.front();
        q.pop();
        if (i < a.size())
        {
            if (a[i] != NULLNODE)
            {
                cur->left = new TreeNode(a[i]);
                q.push(cur->left);
            }
            ++i;
        }
        if (i < a.size())
        {
            if (a[i] != NULLNODE)
            {
                cur->right = new TreeNode(a[i]);
                q.push(cur->right);
            }
            ++i;
        }
    }
    return root;
}

static void serialize(TreeNode *root, vector<int> &out)
{
    out.clear();
    queue<TreeNode *> q;
    q.push(root);
    while (!q.empty())
    {
        TreeNode *cur = q.front();
        q.pop();
        if (cur == nullptr)
        {
            out.push_back(NULLNODE);
            continue;
        }
        out.push_back(cur->val);
        q.push(cur->left);
        q.push(cur->right);
    }
    while (!out.empty() && out.back() == NULLNODE)
        out.pop_back();
}

static string vecToStr(const vector<int> &v, size_t limit = 18)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += (v[i] == NULLNODE) ? "null" : to_string(v[i]);
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 项）";
    return s + "]";
}

static vector<TreeNode *> collectNodes(TreeNode *root, int cap)
{
    vector<TreeNode *> v;
    unordered_set<TreeNode *> seen;
    queue<TreeNode *> q;
    if (root)
        q.push(root);
    while (!q.empty())
    {
        TreeNode *cur = q.front();
        q.pop();
        if (!seen.insert(cur).second)
            break;
        v.push_back(cur);
        if ((int)v.size() > cap)
            break;
        if (cur->left)
            q.push(cur->left);
        if (cur->right)
            q.push(cur->right);
    }
    return v;
}

// 数据自检：值必须在 [-10^4, 10^4]，且不能撞 null 哨兵
static bool checkInput(const vector<int> &a, string &why)
{
    for (int v : a)
    {
        if (v == NULLNODE)
            continue;
        if (v < -10000 || v > 10000)
        {
            why = "值 " + to_string(v) + " 超出题目值域 [-10000,10000]";
            return false;
        }
    }
    return true;
}

struct TestCase
{
    string name;
    vector<int> tree;
    int expected;
};

static bool runCase(const TestCase &c)
{
    string why;
    if (!checkInput(c.tree, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    TreeNode *root = buildTree(c.tree);
    const int cap = (int)c.tree.size() * 2 + 10;

    vector<int> before;
    serialize(root, before);

    int got = -1;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.goodNodes(root);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    vector<int> after;
    serialize(root, after);
    bool intact = (before == after);

    bool pass = (!threw) && (got == c.expected) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       输入 = " << vecToStr(c.tree) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       期望 = " << c.expected << "   实际 = " << got;
        if (!intact)
            cout << "   ✗ 树结构被改动了";
        cout << "\n";
    }

    for (TreeNode *p : collectNodes(root, cap))
        delete p;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {3, 1, 4, 3, NULLNODE, 1, 5}, 4},
        {"示例 2", {3, 3, NULLNODE, 4, 2}, 3},
        {"示例 3（单节点）", {1}, 1},
    };

    vector<TestCase> extraCases = {
        // 全递增 / 全递减
        {"只有根的孩子更小 [3,2,1]", {3, 2, 1}, 1},
        {"全都更大 [1,2,3]", {1, 2, 3}, 3},

        // 值全相同：每个节点都等于路径最大值 -> 全是好节点
        {"值全相同（5 节点，全好）", {5, 5, 5, 5, 5}, 5},

        // ★ 关键反例：比父节点大，但比祖先小 -> 不是好节点
        {"★父小祖大（[5,3,null,4]）", {5, 3, NULLNODE, 4}, 1},
        // 10 -> 9 -> {8, 11}：8 和 9 都比祖先 10 小（不好），只有 11 好
        {"★父小祖大（[10,9,null,8,11]）", {10, 9, NULLNODE, 8, 11}, 2},

        // 负数：初值用 0 会全错
        {"全负数递减 [-1,-2,-3]", {-1, -2, -3}, 1},
        {"负数上升 [-3,-2,-1]", {-3, -2, -1}, 3},
        {"负数链 [0,-1,null,-2]", {0, -1, NULLNODE, -2}, 1},
        {"单节点负数", {-5}, 1},

        // 完全二叉树
        {"完全树 3 层（7 节点，值递增）", {1, 2, 3, 4, 5, 6, 7}, 7},
        {"完全树 3 层（值全 1）", {1, 1, 1, 1, 1, 1, 1}, 7},

        // 题目数值边界
        {"取值上下限", {-10000, 10000, 10000}, 3},
        {"根是最大值（只有根是好节点）", {10000, 9999, 9998, 9997}, 1},
    };

    // 深链用例（题目节点上限 10^5，这里放 5000 层压一压递归栈）
    {
        auto makeChain = [](int n, bool increasing)
        {
            vector<int> a;
            for (int i = 0; i < n; ++i)
            {
                // 必须真正单调：用 i - n/2（递增）/ n/2 - i（递减），不要用取模
                int v = increasing ? (i - n / 2) : (n / 2 - i);
                a.push_back(v);
                if (i + 1 < n)
                    a.push_back(NULLNODE); // 右链
            }
            return a;
        };
        extraCases.push_back({"右链 5000 层（递增 -> 全是好节点）", makeChain(5000, true), 5000});
        extraCases.push_back({"右链 5000 层（递减 -> 只有根）", makeChain(5000, false), 1});
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

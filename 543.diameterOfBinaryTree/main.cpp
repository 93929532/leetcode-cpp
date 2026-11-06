#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set> // 环检测 + 释放去重
#include <algorithm>     // max
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
// 543. 二叉树的直径（Diameter of Binary Tree）· 树 · Easy
//
// 直径 = 树中**任意两个节点之间最长路径的边数**。这条路径**不一定经过根**。
//
//   ★ 核心观察：对任意一个节点 x，**经过 x** 的最长路径长度 =
//         x 左子树的深度 + x 右子树的深度
//     （这里的"深度"是向下走到最深叶子的长度；见下面"边数/节点数"的坑）
//   而整棵树的直径 = 所有节点里，这个值的**最大值**。
//
//   所以是一次后序遍历：递归返回"以该节点为根的深度"，同时顺手更新全局最大直径。
//
//   骨架（depth 用"节点个数"度量：空 = 0，叶子 = 1）：
//       int best = 0;
//
//       int depth(TreeNode* node) {
//           if (node == nullptr) return 0;
//           int L = depth(node->left);
//           int R = depth(node->right);
//           best = max(best, L + R);        // ★ 这里不加 1：L、R 已经是"边数"
//           return 1 + max(L, R);           // 返回给父节点的是"节点个数深度"
//       }
//
//       int diameterOfBinaryTree(TreeNode* root) {
//           best = 0;                       // ★ 多组用例时记得重置成员变量
//           depth(root);
//           return best;
//       }
//
//   四个坑（骨架都有对应用例）：
//     1. ★ **边数 vs 节点数，差 1**：本题直径数的是**边**。
//        用"节点个数"做深度时（空=0、叶子=1），经过 x 的边数正好是 `L + R`，
//        **不能再 +1**；写成 `L + R + 1` 会每条用例都多 1。
//     2. ★ **不能只在根处算**：最长路径可能完全落在某个子树内部，
//        所以要在**每个节点**都更新一次 best，而不是只在 root 上算 L + R。
//        用例 `[1,2,null,3,4,5,6,7,8]` 就是"直径不经过根"的情形（答案 4，根处只有 3）。
//     3. **空树 / 单节点**：直径都是 0（没有边）。
//     4. **best 要初始化/重置**：如果写成成员变量，多组用例调用前要清零，
//        否则上一组的结果会污染下一组。
//
//   复杂度：时间 O(n)（每个节点访问一次）；空间 O(h)（递归栈）。
//
//   面试追问：
//     · 为什么不能"两次 DFS"？-> 那种做法是求"树的直径（任意图）"的经典两趟 BFS，
//       但它只适用于**无负权的一般树/图**，而且要找到端点；这里的"经过每个节点算 L+R"
//       一趟就够了
//     · 和 104 最大深度的关系？-> 543 就是"在 104 的递归过程中顺手记录答案"
//     · 如果要求返回路径而不是长度？-> 递归时记录"最深叶子"的路径，在更新 best 时保存
//
//   同族题：104 二叉树的最大深度（本题的子过程）、110 平衡二叉树（同样是"边递归边返回高度"）、
//           124 二叉树中的最大路径和（Hard，本题的加权版）、
//           687 最长同值路径（本题的"同值约束"版）
class Solution
{
private:
    int maxDiameter;

    int calcHeight(TreeNode *root)
    {
        if (root == nullptr)
            return 0;

        int leftDepth = calcHeight(root->left);
        int rightDepth = calcHeight(root->right);

        maxDiameter = max(maxDiameter, leftDepth + rightDepth);

        return 1 + max(leftDepth, rightDepth);
    }

public:
    int diameterOfBinaryTree(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        maxDiameter = 0;
        calcHeight(root);

        return maxDiameter;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域 -100..100，用 1000 当 null
static const int NULLNODE = 1000;

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

static string vecToStr(const vector<int> &v, size_t limit = 16)
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

// 输入自检：值必须落在 [-100,100]，且不能撞上 NULLNODE 哨兵
static bool checkInput(const vector<int> &a, string &why)
{
    for (int v : a)
    {
        if (v == NULLNODE)
            continue;
        if (v < -100 || v > 100)
        {
            why = "值 " + to_string(v) + " 超出题目值域 [-100,100]";
            return false;
        }
    }
    return true;
}

struct TestCase
{
    string name;
    vector<int> input; // 层序数组（NULLNODE = 空）
    int expected;      // 期望直径（边数）
};

static bool runCase(const TestCase &c)
{
    string why;
    if (!checkInput(c.input, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    TreeNode *root = buildTree(c.input);
    const int cap = (int)c.input.size() * 2 + 10;

    vector<int> before;
    serialize(root, before);

    int got = -1;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.diameterOfBinaryTree(root);
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
         << "       输入 = " << vecToStr(c.input) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       期望 = " << c.expected << "   实际 = " << got;
        if (!intact)
            cout << "   ✗ 树结构被改动了";
        cout << "\n";
    }

    vector<TreeNode *> nodes = collectNodes(root, cap);
    for (TreeNode *p : nodes)
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
        {"示例 1（路径 4-2-1-3）", {1, 2, 3, 4, 5}, 3},
        {"示例 2（两个节点）", {1, 2}, 1},
    };

    vector<TestCase> extraCases = {
        // 边界：没有边 -> 0
        {"空树", {}, 0},
        {"单节点", {1}, 0},

        // 链状：直径就是链长（边数 = 节点数 - 1）
        {"左链 3 层", {1, 2, NULLNODE, 3}, 2},
        {"右链 3 层", {1, NULLNODE, 2, NULLNODE, 3}, 2},
        {"左链 5 层", {1, 2, NULLNODE, 3, NULLNODE, 4, NULLNODE, 5}, 4},

        // 完全树：直径穿过根
        {"完全树 3 层（7 节点）", {1, 2, 3, 4, 5, 6, 7}, 4},
        {"完全树 4 层（15 节点）",
         {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
         6},

        // ★ 直径不经过根：最长路径完全在左子树内部（根处只有 3，答案 4）
        {"直径不经过根", {1, 2, NULLNODE, 3, 4, 5, 6, 7, 8}, 4},

        // 两棵子树一深一浅：最长路径是 4-2-1-3-6，共 4 条边（不是 3）
        {"左深右浅", {1, 2, 3, 4, 5, NULLNODE, 6}, 4},

        // 值重复不影响结果
        {"值全相同", {7, 7, 7, 7, 7}, 3},

        // 题目数值边界
        {"取值上下限", {-100, 100, 0}, 2},
    };

    // 深链：5000 层 -> 直径 4999（压递归栈）
    {
        vector<int> a;
        const int N = 5000;
        for (int i = 0; i < N; ++i)
        {
            a.push_back((i % 201) - 100);
            if (i + 1 < N)
                a.push_back(NULLNODE);
        }
        extraCases.push_back({"右链 5000 层", a, N - 1});
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

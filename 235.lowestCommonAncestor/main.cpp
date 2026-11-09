#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set> // 环检测 + 释放去重
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
// 235. 二叉搜索树的最近公共祖先
//      （Lowest Common Ancestor of a Binary Search Tree）· 树/BST · Medium
//
// ★ 这题的关键是**标题里那两个字：搜索树**。
//   普通二叉树求 LCA 要遍历整棵树 O(n)（那是 236 题，Hard 的入门版），
//   但 BST 有**有序性**，可以从根往下"走"到答案，只要 O(h)。
//
//   核心观察：从根开始往下走，p 和 q 的**第一个分叉点**就是最近公共祖先。
//     对于当前节点 root，只有三种情况：
//       ① p、q 都 <  root  -> LCA 一定在**左**子树，往左走
//       ② p、q 都 >  root  -> LCA 一定在**右**子树，往右走
//       ③ 否则（一个在左一个在右，或者其中一个就是 root）
//                          -> **当前节点就是 LCA**
//
//   迭代写法（最简洁，也最不容易写错）：
//       TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
//           while (root != nullptr) {
//               if (p->val < root->val && q->val < root->val)      root = root->left;
//               else if (p->val > root->val && q->val > root->val) root = root->right;
//               else return root;                                  // ★ 分叉点
//           }
//           return nullptr;   // 题目保证 p、q 都在树里，走不到这里
//       }
//
//   递归写法（同样 O(h)，代码更短）：
//       if (p->val < root->val && q->val < root->val)
//           return lowestCommonAncestor(root->left, p, q);
//       if (p->val > root->val && q->val > root->val)
//           return lowestCommonAncestor(root->right, p, q);
//       return root;
//
//   三个坑（骨架都有对应用例）：
//     1. ★ **别用普通二叉树的 LCA 做法**（后序遍历、左右都非空则返回 root）。
//        那样虽然答案对，但是 O(n)、完全没用上 BST 性质 ——
//        面试官一定会追问"能不能更快"，答不上就是减分项。
//     2. ★ **边界必须用严格的 `<` / `>`**：如果写成 `<=` / `>=`，
//        当 p 或 q **正好等于**某个祖先节点时会错误地往子树里走。
//        例如 p 就是 root、q 在左子树：用 `<=` 会走左子树，答案错。
//     3. **p、q 的先后顺序无所谓**（谁大谁小都行），
//        所以三个判断里不用区分谁是 p 谁是 q。
//
//   复杂度：时间 O(h)（h 为树高，平衡时 O(log n)，最坏链状 O(n)）；
//           空间 O(1)（迭代）或 O(h)（递归栈）。
//
//   面试追问：
//     · 如果不是 BST 怎么办？-> 236 题：后序遍历，左右子树各返回一个"找到的节点"，
//       两边都非空则当前节点是 LCA，O(n)
//     · 如果要多次查询？-> 可以先预处理（如欧拉序 + RMQ），或者用 BST 性质每次 O(h)
//     · p、q 可能不在树里吗？-> 本题保证存在；若可能不存在，要先各查一次再判断
//
//   同族题：236 二叉树的最近公共祖先（Hard，非 BST 版）、
//           1123 最深叶节点的最近公共祖先、1650 二叉树的最近公共祖先 III、
//           98 验证二叉搜索树（同样吃 BST 有序性）
class Solution
{
public:
    TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q)
    {
        // TODO: 在这里写你的解法

        if (p->val < root->val && q->val < root->val)
            return lowestCommonAncestor(root->left, p, q);

        if (p->val > root->val && q->val > root->val)
            return lowestCommonAncestor(root->right, p, q);

        return root;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域 -10^9..10^9（LeetCode 原题），测试数据用小值，用 1000000 当 null
static const int NULLNODE = 1000000;

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

// 收集所有节点（带环检测 + 数量上限）
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

// 按值找节点（题目保证值在 BST 里唯一）
static TreeNode *findByVal(TreeNode *root, int val)
{
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
        if (cur->val == val)
            return cur;
        if (cur->left)
            q.push(cur->left);
        if (cur->right)
            q.push(cur->right);
    }
    return nullptr;
}

// 数据自检：必须是合法 BST（左子树全部 < 节点 < 右子树全部），且不含重复值、
// 不撞 NULLNODE 哨兵。数据不合法时提前报错。
static bool checkBST(TreeNode *root, long long lo, long long hi, string &why)
{
    if (!root)
        return true;
    if (root->val == NULLNODE)
    {
        why = "出现了和 null 哨兵 (1000000) 相同的值";
        return false;
    }
    if (root->val <= lo || root->val >= hi)
    {
        why = "值 " + to_string(root->val) + " 破坏了 BST 的有序性";
        return false;
    }
    if (!checkBST(root->left, lo, root->val, why))
        return false;
    return checkBST(root->right, root->val, hi, why);
}

struct TestCase
{
    string name;
    vector<int> tree; // 层序数组（NULLNODE = 空），必须是合法 BST
    int p;
    int q;
    int expected; // 期望 LCA 的节点值
};

static bool runCase(const TestCase &c)
{
    TreeNode *root = buildTree(c.tree);
    const int cap = (int)c.tree.size() * 2 + 10;

    string why;
    if (!checkBST(root, -1000000000LL, 1000000000LL, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        for (TreeNode *x : collectNodes(root, cap))
            delete x;
        return false;
    }

    TreeNode *pn = findByVal(root, c.p);
    TreeNode *qn = findByVal(root, c.q);
    TreeNode *want = findByVal(root, c.expected);
    if (pn == nullptr || qn == nullptr || want == nullptr)
    {
        cout << "[用例数据错误] " << c.name << "  p/q/期望值 不在树中" << endl;
        for (TreeNode *x : collectNodes(root, cap))
            delete x;
        return false;
    }

    vector<int> before;
    serialize(root, before);

    TreeNode *got = nullptr;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.lowestCommonAncestor(root, pn, qn);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    vector<int> after;
    serialize(root, after);
    bool intact = (before == after);

    bool pass = (!threw) && (got == want) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       树     = " << vecToStr(c.tree) << "\n"
         << "       p, q   = " << c.p << ", " << c.q << "\n";
    if (threw)
        cout << "       异常   = " << err << "\n";
    else
    {
        cout << "       期望   = " << c.expected
             << "   实际 = " << (got ? to_string(got->val) : "nullptr");
        if (got != nullptr && want != nullptr && got->val == want->val && got != want)
            cout << "   ✗ 值对但节点不对";
        if (!intact)
            cout << "   ✗ 树结构被改动了";
        cout << "\n";
    }

    for (TreeNode *x : collectNodes(root, cap))
        delete x;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<int> T1 = {6, 2, 8, 0, 4, 7, 9, NULLNODE, NULLNODE, 3, 5};
    const vector<TestCase> sampleCases = {
        {"示例 1（p=2, q=8，分居两侧）", T1, 2, 8, 6},
        {"示例 2（p=2, q=4，都在左子树）", T1, 2, 4, 2},
        {"示例 3（p=2, q=1）", {2, 1}, 2, 1, 2},
    };

    vector<TestCase> extraCases = {
        // 只有一个孩子
        {"p 是根、q 是它的左孩子", {2, 1}, 1, 2, 2},
        {"三个节点，p、q 分居两侧", {2, 1, 3}, 1, 3, 2},
        {"三个节点，p 是根", {2, 1, 3}, 2, 3, 2},

        // ★ 边界：p 或 q 正好等于某个祖先节点（抓 <= / >= 写错）
        {"p 是根、q 在右子树深处", T1, 6, 5, 6},
        {"q 是根、p 在左子树深处", T1, 0, 6, 6},
        {"p 是祖先、q 在它右子树", T1, 2, 5, 2},

        // 全左链 / 全右链
        {"全左链：p、q 都在最深处", {5, 4, NULLNODE, 3, NULLNODE, 2}, 2, 3, 3},
        {"全右链：p、q 相邻", {1, NULLNODE, 2, NULLNODE, 3}, 2, 3, 2},

        // 同一棵子树内部的 LCA
        {"都在左子树内部（3 和 5）", T1, 3, 5, 4},
        {"都是叶子且分居两棵子树（0 和 5）", T1, 0, 5, 2},

        // 完美 BST 15 节点
        {"完美 BST：1 和 7", {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15}, 1, 7, 4},
        {"完美 BST：1 和 15", {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15}, 1, 15, 8},
        {"完美 BST：5 和 7", {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15}, 5, 7, 6},

        // 负数 / 非连续值
        {"含负数的 BST", {0, -5, 5, -10, -1, 1, 10}, -10, 10, 0},
        {"含负数的 BST：都在左侧", {0, -5, 5, -10, -1, 1, 10}, -10, -1, -5},
    };

    // 深链：1000 层的右链 BST（1->2->...->1000），压递归/迭代深度
    {
        vector<int> chain;
        const int N = 1000;
        for (int i = 1; i <= N; ++i)
        {
            chain.push_back(i);
            if (i + 1 <= N)
                chain.push_back(NULLNODE);
        }
        extraCases.push_back({"右链 BST 1000 层：1 和 1000", chain, 1, 1000, 1});
        extraCases.push_back({"右链 BST 1000 层：999 和 1000", chain, 999, 1000, 999});
        extraCases.push_back({"右链 BST 1000 层：500 和 501", chain, 500, 501, 500});
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

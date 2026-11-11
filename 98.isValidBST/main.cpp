#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set> // 环检测 + 释放去重
#include <climits>       // INT_MIN / INT_MAX / LLONG_MIN / LLONG_MAX
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
// 98. 验证二叉搜索树（Validate Binary Search Tree）· 树/BST · Medium
//
// 判断一棵树是不是合法 BST：
//   每个节点 x 都必须满足：**整棵左子树**的值都 < x->val < **整棵右子树**的值。
//   注意是"整棵子树"，不是"左右孩子"！
//
//   ── 写法一：上下界递归（推荐，最直观）────────────────────
//       bool check(TreeNode* n, long long lo, long long hi) {
//           if (n == nullptr) return true;
//           if (n->val <= lo || n->val >= hi) return false;   // ★ 严格区间 (lo,hi)
//           return check(n->left,  lo, n->val)
//               && check(n->right, n->val, hi);
//       }
//       bool isValidBST(TreeNode* root) {
//           return check(root, LLONG_MIN, LLONG_MAX);         // ★ 用 long long
//       }
//
//   ── 写法二：中序遍历，必须严格递增（也很常见）──────────────
//       中序序列严格递增 <=> 是合法 BST。用一个 prev 指针记住前一个节点：
//       bool inorder(TreeNode* n, TreeNode*& prev) {
//           if (n == nullptr) return true;
//           if (!inorder(n->left, prev)) return false;
//           if (prev != nullptr && prev->val >= n->val) return false;  // ★ >= 不是 >
//           prev = n;
//           return inorder(n->right, prev);
//       }
//
//   ★★ 两个大坑（骨架都有对应用例）：
//     1. **只比较父子节点**：
//            [5,4,6,null,null,3,7]
//            5 的左孩子 4 ✓、右孩子 6 ✓、6 的左孩子 3 ✓（3 < 6）——
//            看起来处处合法，但 3 在 5 的右子树里，3 < 5，**不是合法 BST**。
//        必用上下界（或中序）才能发现。
//     2. **用 `INT_MIN` / `INT_MAX` 当初末边界**：
//        本题值域是**完整的 int 范围** [-2^31, 2^31-1]。若写
//            check(root, INT_MIN, INT_MAX)
//        那么节点值恰好等于 INT_MIN 时，`n->val <= lo` 成立 -> 被误判成非法。
//        ✅ 解决：边界用 `long long`（LLONG_MIN/LLONG_MAX），或者用中序写法（不需要边界）。
//
//   其它细节：
//     · **BST 是严格不等**：出现重复值就非法（`>=` / `<=` 要写对）。
//     · **空树算合法 BST**（返回 true）。
//     · 递归深度 = 树高，本题节点上限 10^4，链状树递归 10^4 层勉强够用
//       （骨架里放了 5000 层链的用例；要绝对稳可改成中序迭代 + 显式栈）。
//
//   复杂度：时间 O(n)；空间 O(h) 递归栈。
//
//   面试追问：
//     · 为什么不能只比较父子？-> 反例见坑 1；BST 的定义是"整棵子树"的约束
//     · 中序写法的 `prev` 为什么要传引用？-> 要跨递归调用保留"上一个访问的节点"
//     · 235 求 LCA 为什么能用 O(h)？-> 同样吃 BST 的有序性（本题是"验证"，235 是"利用"）
//     · 题 99（恢复二叉搜索树）是本题的进阶：找出中序里逆序的两个节点并交换
//
//   同族题：99 恢复二叉搜索树（Hard）、230 BST 第 K 小、235 BST 的 LCA、
//           700 BST 中的搜索、108 有序数组转 BST、173 BST 迭代器
class Solution
{
private:
    bool DFS(TreeNode *root, long long low, long long high)
    {
        if (root == nullptr)
            return true;

        int value = root->val;

        if (value <= low || value >= high)
            return false;

        return DFS(root->left, low, value) && DFS(root->right, value, high);
    }

public:
    bool isValidBST(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        return DFS(root, LLONG_MIN, LLONG_MAX);
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 本题值域是完整 int 范围，所以：
//   · 测试数据用 long long 存，避免哨兵和真实值撞车
//   · null 哨兵取 1e18（远超 int 范围）
static const long long NULLNODE = 1000000000000000000LL;

static TreeNode *buildTree(const vector<long long> &a)
{
    if (a.empty() || a[0] == NULLNODE)
        return nullptr;
    TreeNode *root = new TreeNode((int)a[0]);
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
                cur->left = new TreeNode((int)a[i]);
                q.push(cur->left);
            }
            ++i;
        }
        if (i < a.size())
        {
            if (a[i] != NULLNODE)
            {
                cur->right = new TreeNode((int)a[i]);
                q.push(cur->right);
            }
            ++i;
        }
    }
    return root;
}

static void serialize(TreeNode *root, vector<long long> &out)
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

static string vecToStr(const vector<long long> &v, size_t limit = 18)
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

// 数据自检：值必须在完整 int 范围内，且不能撞 null 哨兵
static bool checkInput(const vector<long long> &a, string &why)
{
    for (long long v : a)
    {
        if (v == NULLNODE)
            continue;
        if (v < (long long)INT_MIN || v > (long long)INT_MAX)
        {
            why = "值 " + to_string(v) + " 超出 int 范围";
            return false;
        }
    }
    return true;
}

struct TestCase
{
    string name;
    vector<long long> tree;
    bool expected;
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

    vector<long long> before;
    serialize(root, before);

    bool got = !c.expected; // 故意取反
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.isValidBST(root);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    vector<long long> after;
    serialize(root, after);
    bool intact = (before == after);

    bool pass = (!threw) && (got == c.expected) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       输入 = " << vecToStr(c.tree) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       期望 = " << (c.expected ? "true" : "false")
             << "   实际 = " << (got ? "true" : "false");
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
        {"示例 1（[2,1,3]）", {2, 1, 3}, true},
        {"示例 2（[5,1,4,null,null,3,6]）", {5, 1, 4, NULLNODE, NULLNODE, 3, 6}, false},
    };

    vector<TestCase> extraCases = {
        {"空树", {}, true},
        {"单节点", {1}, true},

        // ★★ 关键反例：每个父子关系都合法，但 3 落在 5 的右子树里
        {"★只比父子会漏（[5,4,6,null,null,3,7]）",
         {5, 4, 6, NULLNODE, NULLNODE, 3, 7},
         false},
        {"类似形态（[10,5,15,null,null,6,20]）",
         {10, 5, 15, NULLNODE, NULLNODE, 6, 20},
         false},

        // ★ 重复值：BST 要求严格不等
        {"重复值（[2,2,2]）", {2, 2, 2}, false},
        {"重复值（[1,1]）", {1, 1}, false},
        {"重复值（[5,3,8,3,4]）", {5, 3, 8, 3, 4}, false},

        // 链状
        {"左链递减（合法）", {3, 2, NULLNODE, 1}, true},
        {"右链递增（合法）", {1, NULLNODE, 2, NULLNODE, 3}, true},
        {"左链不递减（非法）", {3, 2, NULLNODE, 4}, false},

        // 完全树
        {"完全树（合法）", {4, 2, 6, 1, 3, 5, 7}, true},
        {"完全树但一处放错（非法）", {4, 2, 6, 1, 3, 7, 5}, false},

        // ★★ INT 边界：用 INT_MIN 当初始下界会误判
        {"值为 INT_MIN 的单节点", {(long long)INT_MIN}, true},
        {"值为 INT_MAX 的单节点", {(long long)INT_MAX}, true},
        {"根 INT_MIN，右孩子 0", {(long long)INT_MIN, NULLNODE, 0}, true},
        {"根 INT_MAX，左孩子 0", {(long long)INT_MAX, 0}, true},
        {"根 0，左右是 int 两端", {0, (long long)INT_MIN, (long long)INT_MAX}, true},
        {"根 INT_MIN，右孩子 INT_MIN+1（合法）",
         {(long long)INT_MIN, NULLNODE, (long long)INT_MIN + 1},
         true},
        {"根 INT_MIN，左孩子 0（非法）", {(long long)INT_MIN, 0}, false},
    };

    // 深链：5000 层（合法 BST：1 -> 2 -> ... -> 5000 的右链）
    {
        vector<long long> a;
        const int N = 5000;
        for (int i = 1; i <= N; ++i)
        {
            a.push_back(i);
            if (i < N)
                a.push_back(NULLNODE);
        }
        extraCases.push_back({"右链 5000 层（合法）", a, true});

        // 同一条链，把最后一个节点的值改成破坏顺序的值
        vector<long long> c = a;
        c[c.size() - 1] = 2; // 末尾值塌陷：父节点是 4999，2 太小 -> 非法
        extraCases.push_back({"右链 5000 层，末尾值塌陷（非法）", c, false});
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

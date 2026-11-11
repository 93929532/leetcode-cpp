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
// 230. 二叉搜索树中第 K 小的元素（Kth Smallest Element in a BST）· 树/BST · Medium
//
// ★ 这题几乎是 98 题中序写法的直接复用：
//     **BST 的中序遍历 = 严格递增序列**，所以"中序第 k 个"就是"第 k 小"。
//
//   ── 写法一：中序递归 + 计数器（最直观）──────────────
//       int cnt = 0, ans = 0;
//       void inorder(TreeNode* n, int k) {
//           if (n == nullptr || cnt >= k) return;      // ★ 提前剪枝
//           inorder(n->left, k);
//           if (++cnt == k) { ans = n->val; return; }
//           inorder(n->right, k);
//       }
//       int kthSmallest(TreeNode* root, int k) {
//           cnt = 0; ans = 0; inorder(root, k); return ans;
//       }
//
//   ── 写法二：中序迭代（显式栈，可干净地提前退出，面试推荐）────
//       int kthSmallest(TreeNode* root, int k) {
//           vector<TreeNode*> st;
//           TreeNode* cur = root;
//           while (cur != nullptr || !st.empty()) {
//               while (cur != nullptr) { st.push_back(cur); cur = cur->left; }
//               cur = st.back(); st.pop_back();
//               if (--k == 0) return cur->val;        // ★ 数到第 k 个就返回
//               cur = cur->right;
//           }
//           return -1;   // 题目保证 k 合法，走不到这里
//       }
//
//   三个坑（骨架都有对应用例）：
//     1. ★ **k 从 1 开始**（不是 0）。用 `--k == 0` 比 `++cnt == k` 少一个变量，
//        但务必写对起点。
//     2. ★ **要提前终止**：如果老老实实把整棵树中序遍历完再取第 k 个，
//        复杂度是 O(n)；提前返回只需要 O(h + k)。
//        递归版靠 `cnt >= k` 剪枝，迭代版直接 `return`。
//     3. **递归版状态要重置**：`cnt`/`ans` 如果是成员变量，每次调用前必须清零
//        （否则多组用例之间会串）。
//
//   复杂度：时间 O(h + k)（最多走到第 k 个就停）；空间 O(h) 栈。
//   最坏（k = n 且树退化成链）是 O(n)。
//
//   面试追问：
//     · 如果这棵树会被**频繁修改**，要求每次查询都很快怎么办？
//       -> 在节点里维护"左子树节点数 size"：比较 k 与 size 后决定走左边、取根、还是
//          走右边（并把 k 减去 size+1），单次查询 O(h)。这是 173/230 的进阶考法。
//     · 第 k 大怎么做？-> 中序倒过来（右-根-左），或求第 (n-k+1) 小
//     · 能不能不用中序？-> 可以分治（数 size），但本质还是利用 BST 有序性
//
//   同族题：98 验证 BST（同一个中序套路）、173 BST 迭代器（中序的惰性版本）、
//           235 BST 的 LCA、700 BST 中的搜索
class Solution
{
private:
    int res;
    void DFS(TreeNode *root, int &k)
    {
        if (root == nullptr || k == 0)
            return;

        DFS(root->left, k);
        if (k == 0) 
            return;  

        if (--k == 0)
        {
            res = root->val;
            return;
        }
        
        DFS(root->right, k);
    }

public:
    int kthSmallest(TreeNode *root, int k)
    {
        // TODO: 在这里写你的解法
        res = 0;
        DFS(root, k);

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域 0..10^4，用 100000 当 null
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

// 中序遍历，收集序列（用于自检：必须是严格递增的合法 BST）
static void inorderSeq(TreeNode *n, vector<int> &out)
{
    if (n == nullptr)
        return;
    inorderSeq(n->left, out);
    out.push_back(n->val);
    inorderSeq(n->right, out);
}

struct TestCase
{
    string name;
    vector<int> tree;
    int k;
    int expected;
};

static bool runCase(const TestCase &c)
{
    TreeNode *root = buildTree(c.tree);
    const int cap = (int)c.tree.size() * 2 + 10;

    // 数据自检：必须是合法 BST（中序严格递增），且 k 在 [1, n] 内
    vector<int> seq;
    inorderSeq(root, seq);
    string why;
    for (size_t i = 1; i < seq.size(); ++i)
        if (seq[i - 1] >= seq[i])
        {
            why = "不是合法 BST（中序在 " + to_string(seq[i - 1]) + "," + to_string(seq[i]) + " 处不递增）";
            break;
        }
    if (why.empty() && (c.k < 1 || c.k > (int)seq.size()))
        why = "k=" + to_string(c.k) + " 超出 [1," + to_string(seq.size()) + "]";
    if (!why.empty())
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        for (TreeNode *p : collectNodes(root, cap))
            delete p;
        return false;
    }

    vector<int> before;
    serialize(root, before);

    int got = -1;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.kthSmallest(root, c.k);
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
         << "       输入 = " << vecToStr(c.tree) << "   k = " << c.k << "\n"
         << "       中序 = " << vecToStr(seq, 24) << "\n";
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
        {"示例 1（k=1）", {3, 1, 4, NULLNODE, 2}, 1, 1},
        {"示例 2（k=3）", {5, 3, 6, 2, 4, NULLNODE, NULLNODE, 1}, 3, 3},
    };

    vector<TestCase> extraCases = {
        {"单节点 k=1", {1}, 1, 1},

        // k = 1 / k = n 两个边界
        {"完全树 k=1（最小）", {4, 2, 6, 1, 3, 5, 7}, 1, 1},
        {"完全树 k=4（根）", {4, 2, 6, 1, 3, 5, 7}, 4, 4},
        {"完全树 k=7（最大）", {4, 2, 6, 1, 3, 5, 7}, 7, 7},

        // 链状
        {"左链 k=1", {3, 2, NULLNODE, 1}, 1, 1},
        {"左链 k=2", {3, 2, NULLNODE, 1}, 2, 2},
        {"左链 k=3", {3, 2, NULLNODE, 1}, 3, 3},
        {"右链 k=1", {1, NULLNODE, 2, NULLNODE, 3}, 1, 1},
        {"右链 k=3", {1, NULLNODE, 2, NULLNODE, 3}, 3, 3},

        // 示例 2 的树，k 取遍
        {"示例 2 的树 k=1", {5, 3, 6, 2, 4, NULLNODE, NULLNODE, 1}, 1, 1},
        {"示例 2 的树 k=6", {5, 3, 6, 2, 4, NULLNODE, NULLNODE, 1}, 6, 6},

        // 值域边界：最小 0、较大值
        {"值含 0", {2, 0, 3}, 1, 0},
        {"值在 10^4 附近", {10000, 9999, NULLNODE}, 2, 10000}, // 中序 [9999,10000]

        // 不对称树
        {"左深右浅", {5, 3, 6, 2, 4, NULLNODE, 7}, 4, 5}, // 中序 [2,3,4,5,6,7]
    };

    // 深链：5000 层右链（1 -> 2 -> ... -> 5000），k 取两端
    {
        auto makeChain = [](int n)
        {
            vector<int> a;
            for (int i = 1; i <= n; ++i)
            {
                a.push_back(i);
                if (i < n)
                    a.push_back(NULLNODE);
            }
            return a;
        };
        extraCases.push_back({"右链 5000 层 k=1", makeChain(5000), 1, 1});
        extraCases.push_back({"右链 5000 层 k=2500", makeChain(5000), 2500, 2500});
        extraCases.push_back({"右链 5000 层 k=5000", makeChain(5000), 5000, 5000});
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

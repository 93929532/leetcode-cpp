#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set> // 环检测 + 释放去重
#include <algorithm>     // max, abs
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
// 110. 平衡二叉树（Balanced Binary Tree）· 树 · Easy
//
// 判断整棵树是否"高度平衡"：**每个**节点的左右子树高度差都 ≤ 1。
// 注意是"每个节点"，不是只看根 —— 这是第一个坑。
//
//   ── 写法一：朴素 O(n²)（好想，但慢）────────────────────────────
//       int height(TreeNode* n) {                 // 求高度（节点数，空=0）
//           if (!n) return 0;
//           return 1 + max(height(n->left), height(n->right));
//       }
//       bool isBalanced(TreeNode* root) {
//           if (!root) return true;
//           if (abs(height(root->left) - height(root->right)) > 1) return false;
//           return isBalanced(root->left) && isBalanced(root->right);
//       }
//   为什么慢：每个节点都要**重新遍历**它整棵子树求高度，代价 = Σ(每个子树的高度)。
//   实测（完美二叉树）：65535 个节点时朴素版访问 91.7 万次、剪枝版 6.5 万次，差 14 倍
//   （约等于 log2(n)），也就是 O(n log n) 量级。教科书常写的 O(n²) 是较松的上界 ——
//   链状树反而会在根节点就判出不平衡、直接返回，不会退化。
//
//   ── 写法二：后序 + 剪枝 O(n)（★本题正解）──────────────────────
//   一次遍历，让 dfs **返回高度**；一旦发现某棵子树不平衡，就返回哨兵 -1，
//   父节点看到 -1 立刻也返回 -1（不再往下算）——这一步就是"剪枝"。
//
//       int dfs(TreeNode* n) {
//           if (!n) return 0;
//           int L = dfs(n->left);
//           if (L == -1) return -1;               // ★ 左子树已不平衡，直接上报
//           int R = dfs(n->right);
//           if (R == -1) return -1;               // ★ 右子树同理
//           if (abs(L - R) > 1) return -1;        // ★ 自己不平衡
//           return 1 + max(L, R);                 // 平衡：返回真实高度
//       }
//       bool isBalanced(TreeNode* root) { return dfs(root) != -1; }
//
//   四个坑（骨架都有对应用例）：
//     1. **只看根不算**：要每个节点都检查，所以要靠递归的返回值往下传。
//     2. **`> 1` 不是 `>= 1`**：差 1 是**平衡**的，差 2 才不平衡。
//     3. **-1 必须向上传播**：写完 `int L = dfs(left)` 就忘了判 `L == -1`，
//        会把 -1 当高度参与 `max` 和差值计算 —— 结果看着"差不多"但错。
//     4. **空树是平衡的**（返回 true），别写成 false。
//
//   复杂度：朴素 ≈ O(n log n)（松上界 O(n²)）/ 剪枝 O(n)；空间都是 O(h) 递归栈。
//   ★ 这题是"剪枝"的典型示范：不剪每个节点被反复遍历，剪了每个节点只访问一次。
//
//   面试追问：
//     · 为什么剪枝后是 O(n)？-> 每个节点最多被访问一次，遇到 -1 立刻短路返回
//     · 用 pair<bool,int> 或全局 flag 行不行？-> 都行，本质是"带状态返回"
//     · 高度用"节点数"还是"边数"？-> 只要两侧口径一致，**差值**是一样的，答案也一样
//
//   同族题：104 最大深度（求高度的子过程）、543 二叉树的直径（同样的后序套路）、
//           124 二叉树中的最大路径和（Hard，剪枝思路类似）
class Solution
{
private:
    int dfsTree(TreeNode *root)
    {
        if (root == nullptr)
            return 0;

        int leftChild = dfsTree(root->left);
        if (leftChild == -1)
            return -1;

        int rightChild = dfsTree(root->right);
        if (rightChild == -1)
            return -1;

        if (abs(leftChild - rightChild) > 1)
            return -1;

        return 1 + max(leftChild, rightChild);
    }

public:
    bool isBalanced(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        return dfsTree(root) != -1;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域是 -10^4..10^4，所以用 100000 当 null（不会撞真实值）
static const int NULLNODE = 100000;
static const int VAL_MIN = -10000;
static const int VAL_MAX = 10000;

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

// 输入自检：值必须落在 [-10^4, 10^4]，且不能撞上 NULLNODE 哨兵
static bool checkInput(const vector<int> &a, string &why)
{
    for (int v : a)
    {
        if (v == NULLNODE)
            continue;
        if (v < VAL_MIN || v > VAL_MAX)
        {
            why = "值 " + to_string(v) + " 超出题目值域 [" +
                  to_string(VAL_MIN) + "," + to_string(VAL_MAX) + "]";
            return false;
        }
    }
    return true;
}

struct TestCase
{
    string name;
    vector<int> input; // 层序数组（NULLNODE = 空）
    bool expected;     // 是否平衡
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

    bool got = !c.expected; // 故意取反，用来发现"没被调用/没赋值"的实现
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.isBalanced(root);
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
        cout << "       期望 = " << (c.expected ? "true" : "false")
             << "   实际 = " << (got ? "true" : "false");
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

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1（3,9,20,null,null,15,7）", {3, 9, 20, NULLNODE, NULLNODE, 15, 7}, true},
        {"示例 2（左子树比右子树高 2）",
         {1, 2, 2, 3, 3, NULLNODE, NULLNODE, 4, 4},
         false},
        {"示例 3（空树）", {}, true},
    };

    vector<TestCase> extraCases = {
        {"单节点", {1}, true},
        {"两个节点（差 1，平衡）", {1, 2}, true},

        // 差 1 vs 差 2 的边界（最容易把 >1 写成 >=1）
        {"左右差 1（平衡）", {1, 2, 3, 4}, true},
        {"全左 3 层（差 2，不平衡）", {1, 2, NULLNODE, 3}, false},
        {"全右 3 层（差 2，不平衡）", {1, NULLNODE, 2, NULLNODE, 3}, false},

        // 完全树：都平衡
        {"完全树 3 层（7 节点）", {1, 2, 3, 4, 5, 6, 7}, true},
        {"完全树 4 层（15 节点）",
         {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
         true},

        // ★ 根处平衡（左高 3、右高 2，差 1），但左子树内部不平衡（节点 2 的左右是 2 和 0）
        //    只检查根的写法会误判成 true
        {"根平衡但左子树内部不平衡", {1, 2, 20, 3, NULLNODE, 21, NULLNODE, 4}, false},

        // 值重复不影响结构判断
        {"值全相同（平衡）", {7, 7, 7}, true},

        // 题目数值边界 -10^4 .. 10^4
        {"取值上下限", {-10000, 10000, 0}, true},
    };

    // 链状用例：不平衡，同时压一压递归栈
    {
        auto makeChain = [](int n)
        {
            vector<int> a;
            for (int i = 0; i < n; ++i)
            {
                a.push_back((i % 201) - 100);
                if (i + 1 < n)
                    a.push_back(NULLNODE);
            }
            return a;
        };
        extraCases.push_back({"右链 1000 层（不平衡）", makeChain(1000), false});
        extraCases.push_back({"右链 5000 层（不平衡，压递归栈）", makeChain(5000), false});
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

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
// 124. 二叉树中的最大路径和（Binary Tree Maximum Path Sum）· 树 · Hard
//
// 路径 = 树上一条**不自交**的节点序列（每个节点最多出现一次），
// 路径和 = 序列上所有节点值之和。求所有路径里的最大值。
// **路径不必经过根**，可以只在某棵子树内部。
//
//   ★★ 这题有两个必须分清的东西（是本题唯一的难点）：
//       ① **递归的返回值**：只能是一条"**向下的单边链**"的最大增益
//          （因为父节点只能从自己往一个方向延伸，不能分叉）
//       ② **全局答案 best**：可以**分叉**（左 + 自己 + 右），因为它是一个完整的路径
//
//   公式（后序遍历）：
//       gain(node) = node->val + max(0, max(gain(left), gain(right)))
//                    ↑ 只往一边走；某边增益为负就不要它（取 0）
//
//       best = max(best, node->val + max(0, gain(left)) + max(0, gain(right)))
//                        ↑ 这里两边都要，因为路径可以在 node 处分叉
//
//   代码骨架：
//       int best = INT_MIN;
//       int gain(TreeNode* node) {
//           if (node == nullptr) return 0;
//           int L = max(0, gain(node->left));      // ★ 负增益直接丢弃
//           int R = max(0, gain(node->right));
//           best = max(best, node->val + L + R);   // ★ 分叉：作为完整路径
//           return node->val + max(L, R);          // ★ 只返回一边：作为向上延伸的链
//       }
//       int maxPathSum(TreeNode* root) { best = INT_MIN; gain(root); return best; }
//
//   三个坑（骨架都有对应用例）：
//     1. ★★ **把返回值当成答案**：`return node->val + L + R;` 是错的 ——
//        返回给父节点时只能带**一条边**（否则路径会分叉两次）。
//        答案要单独用全局 best 记录"分叉版"。
//     2. ★ **负增益必须丢弃**（`max(0, gain)`）：某个子树增益是负的，
//        走它只会让路径和变小，不如从当前节点重新开始。
//     3. ★ **初值必须是 `INT_MIN` 而不是 0**：如果所有节点都是负数，
//        答案是最小的那个负数（比如全是 -1 时答案是 -1，不是 0）。
//
//   复杂度：时间 O(n)（每个节点访问一次）；空间 O(h) 递归栈。
//
//   面试追问：
//     · 和 543 二叉树的直径有什么关系？-> 543 是"边数版"，本题是"点权版"，
//       递推结构一模一样：都是"子树信息 + 全局答案"
//     · 如果要返回具体路径怎么办？-> 递归时记录"最大增益来自哪边"，
//       在更新 best 时保存路径
//     · 节点数上限 3*10^4，链状树递归深度 3*10^4 —— 可能接近栈上限，
//       要绝对稳可以改成显式栈的后序迭代
//
//   同族题：543 二叉树的直径（边数版）、687 最长同值路径（带约束版）、
//           124 的最大路径和（本题）、剑指 Offer II 051 节点之和最大的路径
class Solution
{
private:
    int maxPath;
    int findMaxPath(TreeNode *root)
    {
        if (root == nullptr)
            return 0;

        int L, R, v;

        L = findMaxPath(root->left);
        R = findMaxPath(root->right);
        v = root->val;

        maxPath = max(maxPath, max({v, v + L, v + R, v + L + R})); 
        return max({v, v + L, v + R});                             
    }

public:
    int maxPathSum(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        maxPath = INT_MIN;
        findMaxPath(root);

        return maxPath;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域 -1000..1000，用 100000 当 null
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

// 数据自检：值必须在 [-1000, 1000] 内，且不能撞 null 哨兵
static bool checkInput(const vector<int> &a, string &why)
{
    for (int v : a)
    {
        if (v == NULLNODE)
            continue;
        if (v < -1000 || v > 1000)
        {
            why = "值 " + to_string(v) + " 超出题目值域 [-1000,1000]";
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

    int got = INT_MIN;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.maxPathSum(root);
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

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "  节点数 = "
         << (count_if(c.tree.begin(), c.tree.end(), [](int v)
                      { return v != NULLNODE; }))
         << "\n"
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
        {"示例 1（[1,2,3] -> 2-1-3）", {1, 2, 3}, 6},
        {"示例 2（[-10,9,20,null,null,15,7] -> 15-20-7）",
         {-10, 9, 20, NULLNODE, NULLNODE, 15, 7},
         42},
    };

    vector<TestCase> extraCases = {
        // 单节点：正 / 负
        {"单节点正数", {1}, 1},
        {"单节点负数", {-3}, -3},

        // ★ 全负数：初值用 0 会错
        {"全负数（根最大）", {-1, -2, -3}, -1},
        {"全负数（中间最大）", {-5, -1, -8}, -1},
        {"全负数（根最小）", {-10, -2, -3}, -2},

        // 负增益必须丢弃
        {"一边负一边正", {2, -1}, 2},
        {"两边一负一正", {1, -2, 3}, 4},
        {"中间是负、两边大", {-5, 10, 10}, 15},

        // 全正：整棵树都可能被吃满
        {"全正完美树 7 节点", {1, 2, 3, 4, 5, 6, 7}, 18},
        {"全正三节点", {1, 1, 1}, 3},

        // 链状：路径就是整条链
        {"左链 3 层", {1, 2, NULLNODE, 3}, 6},
        {"右链 3 层", {1, NULLNODE, 2, NULLNODE, 3}, 6},
        {"左链含负数", {1, -2, NULLNODE, 3}, 3},

        // ★ 最优路径完全不经过根（根很负）
        {"最优路径不经过根", {-100, 20, 20, 15, 15, 15, 15}, 50},

        // 题目数值边界
        {"全 1000", {1000, 1000, 1000}, 3000},
        {"全 -1000", {-1000, -1000, -1000}, -1000},

        // 深一点的树：验证不经过根
        {"左深右浅", {5, 4, 8, 11, NULLNODE, 13, 4, 7, 2, NULLNODE, NULLNODE, NULLNODE, 1}, 48},
    };

    // 大链：30000 层（题目上限 3*10^4），全 1 -> 路径和 = 30000
    {
        vector<int> a;
        const int N = 30000;
        for (int i = 0; i < N; ++i)
        {
            a.push_back(1);
            if (i + 1 < N)
                a.push_back(NULLNODE);
        }
        extraCases.push_back({"右链 30000 层（全 1，压递归栈）", a, N});
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

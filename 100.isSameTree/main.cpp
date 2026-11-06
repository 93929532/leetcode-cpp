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
// 100. 相同的树（Same Tree）· 树 · Easy
//
// 判断两棵树是否**完全相同**：结构一样 + 对应位置的节点值一样。
//
//   ★ 这是第一道"同时递归两棵树"的题。套路只有三步，且**三种情况必须分清**：
//       ① 两个都空      -> 相同（true）
//       ② 只有一个空    -> 不同（false）      ← 必须单独判，否则会解引用空指针
//       ③ 两个都非空    -> 值相等 && 左子树相同 && 右子树相同
//
//   骨架：
//       bool isSameTree(TreeNode* p, TreeNode* q) {
//           if (p == nullptr && q == nullptr) return true;    // ①
//           if (p == nullptr || q == nullptr) return false;   // ②
//           return p->val == q->val                           // ③
//               && isSameTree(p->left,  q->left)
//               && isSameTree(p->right, q->right);
//       }
//
//   四个坑（骨架都有对应用例）：
//     1. ★ **① 和 ② 的顺序不能反**：写成 `if (p || q) return false;` 放在前面，
//        会把"两个都空"也判成 false；而先判"两个都空"再判"一个空"才严密。
//     2. ★ **少了 ② 就会崩**：只写 `if (!p && !q) return true;` 然后直接
//        `p->val`，遇到"一边空一边不空"立刻段错误。
//     3. **只比值不判结构**：把两棵树按某个序遍历成值列表再比较是**错的** ——
//        `[1,2]` 和 `[1,null,2]` 的前序值序列都是 `[1,2]`，但结构不同。
//     4. **忘了比 val**：结构相等就返回 true，值不同却被判成相同。
//
//   迭代写法（面试可对比）：
//       用队列同时存两棵树的节点对 `(p, q)`，每次弹出比较：
//       都空继续；一个空返回 false；值不同返回 false；然后左右孩子成对入队。
//
//   复杂度：时间 O(min(n, m))（不同时尽快短路）；空间 O(min(h1, h2)) 递归栈。
//
//   面试追问：
//     · 和 101 对称二叉树的关系？-> 101 是"p 的左边对 q 的右边"，
//       本题是"p 的左边对 q 的左边"，只差一行
//     · 和 572 另一棵树的子树的关系？-> 572 需要在 s 里枚举每个节点，
//       逐个调用本题的函数
//     · 能不能只用一个指针遍历？-> 不能，结构信息必须两边同步比较
//
//   同族题：101 对称二叉树（镜像版）、572 另一棵树的子树、226 翻转二叉树、
//           617 合并二叉树
class Solution
{
public:
    bool isSameTree(TreeNode *p, TreeNode *q)
    {
        // TODO: 在这里写你的解法

        if (p == nullptr && q == nullptr)
            return true; // 都空
        if (p == nullptr || q == nullptr)
            return false; // 一个空

        if (p->val != q->val)
            return false; // 值不同
            
        return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域是 -10^4..10^4，所以用 100000 当 null
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

// 单条测试用例：两棵树 + 期望结果
struct TestCase
{
    string name;
    vector<int> p;
    vector<int> q;
    bool expected;
};

static bool runCase(const TestCase &c)
{
    string why;
    if (!checkInput(c.p, why) || !checkInput(c.q, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    TreeNode *tp = buildTree(c.p);
    TreeNode *tq = buildTree(c.q);
    const int cap = ((int)c.p.size() + (int)c.q.size()) * 2 + 10;

    vector<int> beforeP, beforeQ;
    serialize(tp, beforeP);
    serialize(tq, beforeQ);

    bool got = !c.expected; // 故意取反：没被赋值也能看出来
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.isSameTree(tp, tq);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    vector<int> afterP, afterQ;
    serialize(tp, afterP);
    serialize(tq, afterQ);
    bool intact = (beforeP == afterP) && (beforeQ == afterQ);

    bool pass = (!threw) && (got == c.expected) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       p      = " << vecToStr(c.p) << "\n"
         << "       q      = " << vecToStr(c.q) << "\n";
    if (threw)
        cout << "       异常   = " << err << "\n";
    else
    {
        cout << "       期望   = " << (c.expected ? "true" : "false")
             << "   实际 = " << (got ? "true" : "false");
        if (!intact)
            cout << "   ✗ 树结构被改动了";
        cout << "\n";
    }

    for (TreeNode *x : collectNodes(tp, cap))
        delete x;
    for (TreeNode *x : collectNodes(tq, cap))
        delete x;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1（结构值都相同）", {1, 2, 3}, {1, 2, 3}, true},
        {"示例 2（一边有左孩子、一边有右孩子）", {1, 2}, {1, NULLNODE, 2}, false},
        {"示例 3（值互换）", {1, 2, 1}, {1, 1, 2}, false},
    };

    vector<TestCase> extraCases = {
        // 空树
        {"都是空树", {}, {}, true},
        {"一个空一个非空", {}, {0}, false},
        {"一个非空一个空", {0}, {}, false},

        // 单节点
        {"单节点相同", {1}, {1}, true},
        {"单节点值不同", {1}, {2}, false},

        // 结构相同、值不同
        {"结构同值不同（根）", {1, 2, 3}, {9, 2, 3}, false},
        {"结构同值不同（深处）", {1, 2, 3, 4}, {1, 2, 3, 5}, false},

        // ★ 值同、结构不同（抓"只比值不判结构"）
        {"左孩子 vs 右孩子", {1, 2}, {1, NULLNODE, 2}, false},
        {"完全树 vs 缺一个节点", {1, 2, 3}, {1, 2}, false},

        // 对称但不相同：左右互换
        {"左右互换（结构不同）", {1, 2, 3}, {1, 3, 2}, false},

        // 大一点的结构
        {"完全树 7 节点相同", {1, 2, 3, 4, 5, 6, 7}, {1, 2, 3, 4, 5, 6, 7}, true},
        {"完全树末尾差一个节点",
         {1, 2, 3, 4, 5, 6, 7},
         {1, 2, 3, 4, 5, 6},
         false},

        // 题目数值边界
        {"取值上下限相同", {-10000, 10000, 0}, {-10000, 10000, 0}, true},
        {"取值上下限差 1", {-10000, 10000, 0}, {-10000, 9999, 0}, false},
    };

    // 深链：两棵完全相同的 5000 层右链（压递归栈）
    {
        auto makeChain = [](int n, int tweakLast)
        {
            vector<int> a;
            for (int i = 0; i < n; ++i)
            {
                int v = (i % 201) - 100;
                if (i == n - 1)
                    v += tweakLast;
                a.push_back(v);
                if (i + 1 < n)
                    a.push_back(NULLNODE);
            }
            return a;
        };
        extraCases.push_back({"右链 5000 层：完全相同", makeChain(5000, 0), makeChain(5000, 0), true});
        extraCases.push_back({"右链 5000 层：只差最后一个节点",
                              makeChain(5000, 0), makeChain(5000, 1), false});
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

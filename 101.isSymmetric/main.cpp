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
// 101. 对称二叉树（Symmetric Tree）· 树 · Easy
//
// 判断整棵树是否**关于中轴线左右镜像对称**。
//
//   ★ 核心：对称不是"左右子树相同"，而是"**左子树与右子树的镜像相同**"。
//     做法是把根的两个子树当成两个指针 p、q，做**交叉**递归比较：
//         p->left  对 q->right
//         p->right 对 q->left
//
//   骨架：
//       bool check(TreeNode* p, TreeNode* q) {          // 判断两棵子树是否互为镜像
//           if (p == nullptr && q == nullptr) return true;   // 都空
//           if (p == nullptr || q == nullptr) return false;  // 一个空
//           return p->val == q->val
//               && check(p->left,  q->right)               // ★ 交叉
//               && check(p->right, q->left);               // ★ 交叉
//       }
//       bool isSymmetric(TreeNode* root) {
//           if (root == nullptr) return true;
//           return check(root->left, root->right);
//       }
//
//   三个坑（骨架都有对应用例）：
//     1. ★★ **对称 ≠ 左右子树相同**（本题最大的坑）：
//            `[1,2,2,null,3,null,3]`（示例 2）的左右子树**结构和值完全一样**，
//            但它们不是镜像 -> 答案 false。
//            如果写成 `return isSameTree(root->left, root->right)`，这条用例会误判成 true。
//            注意"交叉"两个字：p 的左边要和 q 的**右边**比。
//     2. **空树返回 true**（对称的定义对空树成立），单节点也是 true。
//     3. **忘了判"一个空"就会崩**：只写都空的判断，然后直接 `p->val`，
//        遇到"一边空一边不空"立刻段错误。
//
//   迭代写法（面试可对比）：队列里成对存 `(p, q)`，每次弹出比较，然后
//       **交叉入队**：`(p->left, q->right)` 和 `(p->right, q->left)`。
//
//   复杂度：时间 O(n)（每个节点访问一次）；空间 O(h) 递归栈。
//
//   面试追问：
//     · 和 100 相同的树的区别？-> 只差最后两行的左右配对（本题是交叉）
//     · 能不能用中序遍历判断？-> 不行！中序序列回文并不等价于镜像对称。
//       例如 `[1,2,3,3,null,2]` 之类能构造出"中序回文但不对称"的反例
//     · 能不能只比较每层的值是否回文？-> 也不行，某一层回文不代表结构镜像
//
//   同族题：100 相同的树（非镜像版）、572 另一棵树的子树、
//           226 翻转二叉树（镜像就是翻转后相同）、951 翻转等价二叉树
class Solution
{
    bool check(TreeNode *p, TreeNode *q)
    {
        if (p == nullptr && q == nullptr)
            return true;

        if (p == nullptr || q == nullptr)
            return false;

        if (p->val != q->val)
            return false;

        return check(p->left, q->right) && check(p->right, q->left);
    }

public:
    bool isSymmetric(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        if (root == nullptr)
            return true;

        return check(root->left, root->right);
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域是 -100..100，用 100000 当 null
static const int NULLNODE = 100000;
static const int VAL_MIN = -100;
static const int VAL_MAX = 100;

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

struct TestCase
{
    string name;
    vector<int> input;
    bool expected;
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

    bool got = !c.expected; // 故意取反，便于发现"没被赋值"
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.isSymmetric(root);
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
        {"示例 1（完全对称 7 节点）", {1, 2, 2, 3, 4, 4, 3}, true},
        {"示例 2（左右子树相同但不镜像）", {1, 2, 2, NULLNODE, 3, NULLNODE, 3}, false},
    };

    vector<TestCase> extraCases = {
        {"空树", {}, true},
        {"单节点", {1}, true},

        // 两个叶子 / 只有一个孩子（单边必然不对称）
        {"三个节点都相等", {1, 2, 2}, true},
        {"只有左孩子", {1, 2}, false},
        {"只有右孩子", {1, NULLNODE, 2}, false},
        {"值不同", {1, 2, 3}, false},

        // ★ 左右子树结构值完全相同，但配对方向不对 -> 不对称
        {"左右子树相同但配对错（都是左孩子）", {1, 2, 2, 3, NULLNODE, 3, NULLNODE}, false},
        // ★ 镜像配对正确 -> 对称
        {"左右孩子交叉配对（对称）", {1, 2, 2, NULLNODE, 3, 3, NULLNODE}, true},
        {"镜像配对（对称）", {1, 2, 2, 3, NULLNODE, NULLNODE, 3}, true},

        // 深的形状差异
        {"左深右浅", {1, 2, 2, 3, NULLNODE, NULLNODE, NULLNODE}, false},
        {"完全树但是值不对称", {1, 2, 2, 3, 4, 4, 5}, false},

        // 题目数值边界
        {"取值上下限（对称）", {-100, 100, 100}, true},
        {"取值上下限（不对称）", {-100, 100, 99}, false},
    };

    // 大规模对称树：完美二叉树 9 层（511 节点，题目上限 1000），
    // 每一层的值都按"回文"方式填（k 和 本层长度-1-k 取同一个值）->
    // 每一层都是回文，整棵树就镜像对称
    {
        vector<int> a;
        int levelLen = 1;
        for (int lvl = 0; lvl < 9; ++lvl)
        {
            for (int k = 0; k < levelLen; ++k)
            {
                int half = (k < levelLen - 1 - k) ? k : (levelLen - 1 - k);
                a.push_back((half % 2 == 0) ? 1 : 2);
            }
            levelLen *= 2;
        }
        extraCases.push_back({"完美二叉树 9 层（511 节点，对称）", a, true});

        // 同结构，但把最后一层的第一个值改掉 -> 该层不再是回文 -> 不对称
        vector<int> b = a;
        b[255] = (b[255] == 1) ? 2 : 1;
        extraCases.push_back({"完美二叉树 9 层，最后一层改一个值（不对称）", b, false});
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

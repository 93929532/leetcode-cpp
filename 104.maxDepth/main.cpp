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
// 104. 二叉树的最大深度（Maximum Depth of Binary Tree）· 树 · Easy
//
// 深度 = 从根到**最远叶子**路径上的**节点个数**。
//
//   递归思路（一句话）：
//       空树的深度是 0；否则深度 = 1 + max(左子树深度, 右子树深度)
//
//   骨架（一行也能写）：
//       int maxDepth(TreeNode* root) {
//           if (root == nullptr) return 0;                       // ① 空树 = 0
//           return 1 + max(maxDepth(root->left),
//                          maxDepth(root->right));               // ② 别忘了这个 1
//       }
//
//   四个坑：
//     1. **空树返回 0，不是 1**（示例里的空输入就是 0）。
//     2. **叶子返回 1 而不是 0**：叶子是 `1 + max(0, 0) = 1`，靠公式自然成立，
//        所以**不需要**给叶子单独写 if —— 这也是这题比 111 简单的地方。
//     3. **别把"深度"和"高度"搞混**：
//        深度（本题）数**节点个数**；有的题（如 110 里的树高）数的是**边数**，
//        两者差 1。写之前先看清题目问的是哪个。
//     4. **递归深度 = 树高**：链状树有 10^4 层时，递归栈可能吃紧
//        （Windows 默认栈约 1MB，每帧几十字节）。骨架里放了 5000 层的链状用例，
//        真要在 10^4 层上稳妥，就换迭代写法。
//
//   迭代写法（BFS 层序，面试可以对比着说）：
//       用队列一层一层地弹，每弹完一层 depth++。好处是没有递归栈；
//       坏处是要 O(宽度) 的队列空间，且要额外记"这一层有几个节点"。
//
//   复杂度：时间 O(n)（每个节点访问一次）；空间 O(h)（递归栈），最坏链状 O(n)。
//
//   面试追问：
//     · 能不能 O(1) 空间？-> 树题基本做不到，除非用 Morris 遍历（用叶子空指针当线索）
//     · 111 最小深度和这题有什么区别？-> ★ 大坑：最小深度里
//       "只有一个孩子的节点"不算叶子，不能直接 `1 + min(左, 右)`（空孩子那侧会返回 0）
//     · 543 二叉树的直径、110 平衡二叉树都是这题的变体（都需要子树高度）
//
//   同族题：111 最小深度、110 平衡二叉树、543 二叉树的直径、559 N 叉树的最大深度
class Solution
{
public:
    int maxDepth(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        if (root == nullptr)
            return 0;
        else
            return 1 + max(maxDepth(root->left), maxDepth(root->right));
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域 -100..100，用 1000 当 null
static const int NULLNODE = 1000;

// 按力扣的层序数组建树
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

// 把树序列化成层序数组（去掉尾部 null），用于校验"树没被改动"
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

// 输入自检：层序数组里每个值要么是 NULLNODE（表示空），要么落在题目值域 [-100, 100]。
// 乱填值会引出两类"假问题"：① 值和 NULLNODE 撞了（比如填了 1000），会被当成空节点，
// 树被悄悄截断；② 超值域的输入本身就不合法。提前报错，免得把"用例写错"当成"解法写错"。
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

// 单条测试用例
struct TestCase
{
    string name;
    vector<int> input; // 层序数组（NULLNODE = 空）
    int expected;      // 期望深度
};

// 运行一条用例，打印一行信息，返回该用例是否通过
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
        got = sol.maxDepth(root);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    // 树不应该被改动（本题只是"看"，不需要改结构）
    vector<int> after;
    serialize(root, after);
    bool intact = (before == after);

    bool pass = (!threw) && (got == c.expected) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  节点数 = " << (count_if(c.input.begin(), c.input.end(), [](int v)
                                       { return v != NULLNODE; }))
         << "\n"
         << "       输入 = " << vecToStr(c.input) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
        cout << "       期望 = " << c.expected << "   实际 = " << got;
    if (!intact)
        cout << "\n       ✗ 树结构被改动了（这题不需要改树）";
    cout << endl;

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
        {"示例 1（3 层）", {3, 9, 20, NULLNODE, NULLNODE, 15, 7}, 3},
        {"示例 2（右链 2 层）", {1, NULLNODE, 2}, 2},
        {"示例 3（空树）", {}, 0},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"单节点", {1}, 1},

        // 链状：验证"深度 = 节点数"而不是边数
        {"左链 3 层", {1, 2, NULLNODE, 3}, 3},
        {"右链 3 层", {1, NULLNODE, 2, NULLNODE, 3}, 3},

        // 完全二叉树
        {"完全树 3 层（7 节点）", {1, 2, 3, 4, 5, 6, 7}, 3},
        {"完全树 4 层（15 节点）",
         {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
         4},

        // 一边深一边浅：最大值要取"深的那边"
        {"左深右浅", {1, 2, 3, 4}, 3},
        {"右深左浅", {1, 2, 3, NULLNODE, NULLNODE, NULLNODE, 4}, 3},

        // 值重复不影响深度
        {"值全相同", {7, 7, 7, 7, 7}, 3},

        // 题目数值边界 -100 .. 100
        {"取值上下限", {-100, 100, 0}, 2},
    };

    // 深链用例：递归深度 = 树高，这里压一压递归栈
    {
        // 深链用例：值必须落在 [-100,100] 内，否则会撞上 NULLNODE 哨兵（1000）被误当成空节点
        auto makeChain = [](int n)
        {
            vector<int> a;
            for (int i = 0; i < n; ++i)
            {
                a.push_back((i % 201) - 100); // 值在 [-100,100] 内循环
                if (i + 1 < n)
                    a.push_back(NULLNODE); // 每层只有一个孩子 -> 右链
            }
            return a;
        };
        extraCases.push_back({"右链 1000 层（压递归栈）", makeChain(1000), 1000});
        extraCases.push_back({"右链 5000 层（递归栈接近上限）", makeChain(5000), 5000});
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

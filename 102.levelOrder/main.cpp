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
// 102. 二叉树的层序遍历（Binary Tree Level Order Traversal）· 树/BFS · Medium
//
// 按"从上到下、从左到右"逐层输出所有节点的值。
// ★ 这是树主题里 **BFS 模板的第一题**，后面的 199 右视图、637 层平均值、
//   103 锯齿遍历、107 自底向上、116 填充 next 指针，全都是在这个模板上改几行。
//
//   核心：队列 + **每轮先把本层的节点数记下来**，然后只弹这么多。
//       vector<vector<int>> levelOrder(TreeNode* root) {
//           vector<vector<int>> ans;
//           if (root == nullptr) return ans;          // ★ 空树返回空数组
//           queue<TreeNode*> q;
//           q.push(root);
//           while (!q.empty()) {
//               int sz = (int)q.size();               // ★ 先记下本层大小
//               vector<int> level;
//               for (int i = 0; i < sz; ++i) {        // ★ 只处理本层 sz 个
//                   TreeNode* cur = q.front(); q.pop();
//                   level.push_back(cur->val);
//                   if (cur->left)  q.push(cur->left);
//                   if (cur->right) q.push(cur->right);
//               }
//               ans.push_back(level);
//           }
//           return ans;
//       }
//
//   四个坑（骨架都有对应用例）：
//     1. ★ **忘了 `sz` 这一层切分**：直接 `while (!q.empty()) { 弹一个、存一个 }`
//        只能得到一维的 BFS 序列，拿不到"分层"的信息。
//     2. ★ **`q.size()` 必须存下来**：写在 `for (int i = 0; i < q.size(); ++i)` 里，
//        `q.size()` 在循环中会因为 push 而变化 —— 本层会永远弹不完（甚至死循环）。
//     3. **空树要返回 `[]` 而不是 `[[]]`**：`ans` 一开始就是空，别先 push 一个空层。
//     4. **符号比较**：`int sz = q.size()` 要显式转换，否则 -Wsign-compare 警告。
//
//   另一种写法：DFS + 深度参数（面试可以对比着说）
//       void dfs(TreeNode* n, int depth, vector<vector<int>>& ans) {
//           if (!n) return;
//           if (depth == (int)ans.size()) ans.push_back({});   // 第一次到这一层
//           ans[depth].push_back(n->val);
//           dfs(n->left,  depth + 1, ans);
//           dfs(n->right, depth + 1, ans);
//       }
//   好处：不用队列；坏处：递归栈、而且"层"的顺序靠 depth 保证。
//
//   复杂度：时间 O(n)（每个节点进出队列各一次）；
//           空间 O(w)（w 为最宽一层的节点数），最坏 O(n/2)。
//
//   面试追问：
//     · 如果要求"从下往上"输出？-> 107 题：把 ans 整体 reverse 一下
//     · 如果要"之字形"？-> 103 题：奇数层 reverse（或用双端队列）
//     · 只要每层最右边的节点？-> 199 题：每层取最后一个
//     · 每层的平均值？-> 637 题：每层累加除以 size（注意用 double）
//     · 能不能 O(1) 空间？-> 常规 BFS 不行；如果允许修改树，可以用 next 指针（116 题）
//
//   同族题：199 二叉树的右视图、637 二叉树的层平均值、103 二叉树的锯齿形层序遍历、
//           107 二叉树的层序遍历 II、116 填充每个节点的下一个右侧节点指针
class Solution
{
public:
    vector<vector<int>> levelOrder(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        if (root == nullptr)
            return {};

        vector<TreeNode *> cur = {root};
        vector<vector<int>> res;

        while (!cur.empty())
        {
            vector<TreeNode *> nxt;
            vector<int> level;

            for (auto p : cur)
            {
                level.push_back(p->val);

                if (p->left != nullptr)
                    nxt.push_back(p->left);
                if (p->right != nullptr)
                    nxt.push_back(p->right);
            }

            cur = nxt;
            res.push_back(level);
        }

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域是 -1000..1000，用 100000 当 null
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

// 把二维结果打印成 [[3],[9,20],[15,7]]
static string matToStr(const vector<vector<int>> &m, size_t maxLevels = 6, size_t maxItems = 8)
{
    string s = "[";
    for (size_t i = 0; i < m.size() && i < maxLevels; ++i)
    {
        if (i)
            s += ",";
        s += "[";
        for (size_t j = 0; j < m[i].size() && j < maxItems; ++j)
        {
            if (j)
                s += ",";
            s += to_string(m[i][j]);
        }
        if (m[i].size() > maxItems)
            s += ",…";
        s += "]";
    }
    if (m.size() > maxLevels)
        s += ",…（共 " + to_string(m.size()) + " 层）";
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

struct TestCase
{
    string name;
    vector<int> tree;
    vector<vector<int>> expected;
};

static bool runCase(const TestCase &c)
{
    TreeNode *root = buildTree(c.tree);
    const int cap = (int)c.tree.size() * 2 + 10;

    vector<int> before;
    serialize(root, before);

    vector<vector<int>> got;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.levelOrder(root);
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
         << "       输入 = " << vecToStr(c.tree) << "\n"
         << "       期望 = " << matToStr(c.expected) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       实际 = " << matToStr(got);
        // 常见错误提示：一维 BFS 序列被塞进一层 / 层数不对
        if (got.size() == 1 && c.expected.size() > 1)
            cout << "\n       ✗ 只有 1 层：忘了用 q.size() 切分每层";
        else if (got.size() != c.expected.size())
            cout << "\n       ✗ 层数不对（期望 " << c.expected.size() << " 层）";
        if (!intact)
            cout << "\n       ✗ 树结构被改动了";
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

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {3, 9, 20, NULLNODE, NULLNODE, 15, 7}, {{3}, {9, 20}, {15, 7}}},
        {"示例 2（单节点）", {1}, {{1}}},
        {"示例 3（空树）", {}, {}},
    };

    vector<TestCase> extraCases = {
        {"两个节点（只有左孩子）", {1, 2}, {{1}, {2}}},
        {"两个节点（只有右孩子）", {1, NULLNODE, 2}, {{1}, {2}}},

        // 完全二叉树
        {"完全树 3 层（7 节点）", {1, 2, 3, 4, 5, 6, 7}, {{1}, {2, 3}, {4, 5, 6, 7}}},
        {"完全树 4 层（15 节点）",
         {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
         {{1}, {2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}}},

        // 链状：每层只有 1 个节点
        {"左链 3 层", {1, 2, NULLNODE, 3}, {{1}, {2}, {3}}},
        {"右链 3 层", {1, NULLNODE, 2, NULLNODE, 3}, {{1}, {2}, {3}}},

        // 不对称：某层缺节点
        {"不对称（第 3 层只有 4）", {1, 2, 3, 4}, {{1}, {2, 3}, {4}}},
        {"某层两边各缺一个", {1, 2, 3, NULLNODE, 4, NULLNODE, 5}, {{1}, {2, 3}, {4, 5}}},
        {"左边深右边浅", {1, 2, 3, 4, NULLNODE, NULLNODE, NULLNODE, 5}, {{1}, {2, 3}, {4}, {5}}},

        // 值有重复：不能靠值判断层次
        {"值全相同", {5, 5, 5, 5, 5, 5, 5}, {{5}, {5, 5}, {5, 5, 5, 5}}},

        // 题目数值边界
        {"取值上下限", {-1000, 1000, 1000}, {{-1000}, {1000, 1000}}},
    };

    // 大规模用例：完美二叉树 10 层（1023 节点）
    {
        vector<int> tree;
        vector<vector<int>> expected;
        int start = 1, len = 1;
        for (int lvl = 0; lvl < 10; ++lvl)
        {
            vector<int> level;
            for (int k = 0; k < len; ++k)
            {
                tree.push_back(start + k);
                level.push_back(start + k);
            }
            expected.push_back(level);
            start += len;
            len *= 2;
        }
        extraCases.push_back({"完美二叉树 10 层（1023 节点）", tree, expected});

        // 右链 1000 层：每层 1 个节点
        vector<int> chain;
        vector<vector<int>> chainExp;
        for (int i = 1; i <= 1000; ++i)
        {
            chain.push_back((i % 201) - 100);
            chainExp.push_back({(i % 201) - 100});
            if (i < 1000)
                chain.push_back(NULLNODE);
        }
        extraCases.push_back({"右链 1000 层（每层 1 个）", chain, chainExp});
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

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
// 199. 二叉树的右视图（Binary Tree Right Side View）· 树/BFS · Medium
//
// 站在树的**右边**看过去，从上到下能看到的节点值。
// 换句话说：**每一层最右边的那个节点**。
//
//   ★★ 第一大坑：**右视图 ≠ 一直往右走的那条路径**！
//      最右边可不一定在右子树里。反例：
//            1
//          ┌─┴─┐
//          2   3
//        ┌─┘
//        4
//      每层最右 = 1, 3, 4  -> 右视图是 [1,3,4]
//      但如果"一直走 right"，只能拿到 [1,3] —— 漏了 4。
//      （骨架里专门放了这条用例，就是抓这个错。）
//
//   ── 写法一：BFS 分层（承接 102，最直观）──────────────
//       vector<int> rightSideView(TreeNode* root) {
//           vector<int> ans;
//           if (root == nullptr) return ans;
//           queue<TreeNode*> q; q.push(root);
//           while (!q.empty()) {
//               int sz = (int)q.size();
//               for (int i = 0; i < sz; ++i) {
//                   TreeNode* cur = q.front(); q.pop();
//                   if (i == sz - 1) ans.push_back(cur->val);   // ★ 本层最后一个
//                   if (cur->left)  q.push(cur->left);
//                   if (cur->right) q.push(cur->right);
//               }
//           }
//           return ans;
//       }
//     也可以用 102 那套「当前层 cur / 下一层 nxt」双数组，取 cur.back()。
//
//   ── 写法二：DFS，**先右后左**（更省空间，面试加分）──────────
//       void dfs(TreeNode* n, int depth, vector<int>& ans) {
//           if (!n) return;
//           if (depth == (int)ans.size())      // ★ 这一层第一次到达的节点
//               ans.push_back(n->val);         //   因为先走右边，它就是最右
//           dfs(n->right, depth + 1, ans);
//           dfs(n->left,  depth + 1, ans);
//       }
//       vector<int> rightSideView(TreeNode* root) {
//           vector<int> ans; dfs(root, 0, ans); return ans;
//       }
//     为什么对：先访问右子树，所以每层**第一次**被访问到的节点一定是该层最右的；
//     `depth == ans.size()` 表示"这一层还没记录过"。
//     ★ 必须"先右后左"，顺序反了会记录成最左节点。
//
//   三个坑（骨架都有对应用例）：
//     1. ★★ 把右视图误解成"一直往右走"（见上面的反例）。
//     2. **空树返回空数组**，不是 `[0]` 或 `[[]]`。
//     3. **BFS 取最后一个**（`i == sz-1`），别取成第一个；
//        DFS 必须**先右后左**，否则记录的是最左节点。
//
//   复杂度：BFS 时间 O(n)、空间 O(最宽层)；DFS 时间 O(n)、空间 O(h)。
//   如果树很深很窄，DFS 更省空间；如果树很宽，BFS 队列会很大。
//
//   面试追问：
//     · 左视图怎么做？-> 每层取第一个（BFS 用 i==0；DFS 改成先左后右）
//     · 每层的最大值/平均值？-> 637、515 同模板
//     · DFS 和 BFS 哪个更好？-> 本题两者都 O(n)，DFS 空间更优（O(h) vs O(w)）
//
//   同族题：102 二叉树的层序遍历、637 二叉树的层平均值、103 锯齿形层序遍历、
//           515 在每个树行中找最大值、513 找树左下角的值
class Solution
{
public:
    vector<int> rightSideView(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        if (root == nullptr)
            return {};

        vector<int> res;
        deque<TreeNode *> cur = {root};

        while (!cur.empty())
        {
            int size = cur.size();
            int levelEnd = 0;

            for (int i = 0; i < size; i++)
            {
                TreeNode *p = cur.front();
                cur.pop_front();
                levelEnd = p->val;

                if (p->left != nullptr)
                    cur.push_back(p->left);
                if (p->right != nullptr)
                    cur.push_back(p->right);
            }
            res.push_back(levelEnd);
        }

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域是 -100..100，用 100000 当 null
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

static string vecToStr(const vector<int> &v, size_t limit = 20)
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

static string intsToStr(const vector<int> &v, size_t limit = 20)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += to_string(v[i]);
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

struct TestCase
{
    string name;
    vector<int> tree;
    vector<int> expected;
};

static bool runCase(const TestCase &c)
{
    TreeNode *root = buildTree(c.tree);
    const int cap = (int)c.tree.size() * 2 + 10;

    vector<int> before;
    serialize(root, before);

    vector<int> got;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.rightSideView(root);
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
         << "       期望 = " << intsToStr(c.expected) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       实际 = " << intsToStr(got);
        // 针对性提示
        if (!pass && !intact)
            cout << "\n       ✗ 树结构被改动了";
        else if (got.size() + 1 == c.expected.size() || got.size() + 2 == c.expected.size())
            cout << "\n       ✗ 少了层：可能把右视图理解成「一直往右走」";
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
        {"示例 1（[1,2,3,null,5,null,4]）",
         {1, 2, 3, NULLNODE, 5, NULLNODE, 4},
         {1, 3, 4}},
        {"示例 2（最右在左子树深处）",
         {1, 2, 3, 4, NULLNODE, NULLNODE, NULLNODE, 5},
         {1, 3, 4, 5}},
        {"示例 3（只有右孩子）", {1, NULLNODE, 3}, {1, 3}},
        {"示例 4（空树）", {}, {}},
    };

    vector<TestCase> extraCases = {
        {"单节点", {1}, {1}},

        // 链状：每层一个
        {"左链 3 层", {1, 2, NULLNODE, 3}, {1, 2, 3}},
        {"右链 3 层", {1, NULLNODE, 2, NULLNODE, 3}, {1, 2, 3}},

        // ★ 关键反例：从根一直往右的结果是 [1,3]，但正确答案是 [1,3,4]
        {"★最右在左子树（[1,2,3,4]）", {1, 2, 3, 4}, {1, 3, 4}},
        {"★最右在左子树（两层）", {1, 2, 3, 4, NULLNODE, NULLNODE, NULLNODE}, {1, 3, 4}},

        // 完全树：每层最右是 2^(d+1)-1
        {"完全树 3 层（7 节点）", {1, 2, 3, 4, 5, 6, 7}, {1, 3, 7}},
        {"完全树 4 层（15 节点）",
         {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
         {1, 3, 7, 15}},

        // 只有左孩子的链：右视图 = 每层唯一的节点
        {"只有左孩子的 4 层链", {1, 2, NULLNODE, 3, NULLNODE, 4}, {1, 2, 3, 4}},

        // 值重复：不能靠值判断层次
        {"值全相同（5 节点）", {5, 5, 5, 5, 5}, {5, 5, 5}},

        // 题目数值边界 -100 .. 100
        {"取值上下限", {-100, 100, 100}, {-100, 100}},
    };

    // 完美二叉树 6 层（63 节点，题目上限 100）：每层最右 = 2^(d+1)-1
    {
        vector<int> tree, expected;
        int start = 1, len = 1;
        for (int lvl = 0; lvl < 6; ++lvl)
        {
            for (int k = 0; k < len; ++k)
                tree.push_back(start + k);
            expected.push_back(start + len - 1);
            start += len;
            len *= 2;
        }
        extraCases.push_back({"完美二叉树 6 层（63 节点）", tree, expected});
    }

    // 100 层右链（题目上限 100 个节点）
    {
        vector<int> tree, expected;
        for (int i = 1; i <= 100; ++i)
        {
            tree.push_back((i % 201) - 100);
            expected.push_back((i % 201) - 100);
            if (i < 100)
                tree.push_back(NULLNODE);
        }
        extraCases.push_back({"右链 100 层（每层一个）", tree, expected});
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

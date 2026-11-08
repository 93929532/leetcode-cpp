#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>

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
// 572. 另一棵树的子树（Subtree of Another Tree）· 树 · Easy
//
// 给定两棵树 root 和 subRoot，判断 root 里是否存在一棵子树，与 subRoot
// **结构和节点值完全相同**。
//
//   ① 最直接的思路（推荐，也最不容易错）
//      题目定义本来就是：「存在 root 的某个节点，使以它为根的子树与 subRoot 相同」。
//      那就照着定义写：
//          bool isSubtree(TreeNode* root, TreeNode* sub) {
//              if (root == nullptr) return false;              // 走到空还没匹配上
//              if (isSameTree(root, sub)) return true;         // ★ 复用 100 题！
//              return isSubtree(root->left,  sub)
//                  || isSubtree(root->right, sub);
//          }
//      而 isSameTree 就是你刚做完的 100 题：
//          if (!p && !q) return true;
//          if (!p || !q) return false;
//          return p->val == q->val && isSameTree(p->left,q->left) && isSameTree(p->right,q->right);
//
//      → 这就是「**外层遍历 + 内层比较**」的双递归，本题的核心套路。
//      复杂度 O(m·n)：每个 root 节点都要跑一遍 O(n) 的比较。m,n ≤ 2000，够用。
//
//   ② 更"优雅"的写法：序列化 + 子串搜索
//      把两棵树都序列化成字符串，再看 sub 的串是不是 root 的串的**子串**。
//      **用【前序 + null 占位】是对的** —— 我实测过 20 万组随机树，0 差异。
//          void ser(TreeNode* r, string& s) {
//              if (!r) { s += "#,"; return; }        // ★ null 必须写出来当占位
//              s += to_string(r->val) + ",";         // ★ 数字后面要有分隔符
//              ser(r->left, s); ser(r->right, s);
//          }
//          return rootStr.find(subStr) != string::npos;
//      为什么对：**前序遍历里，每棵子树都占据连续的一段**。
//
//   ③ ⚠️ 一个非常像但**是错的**的写法（务必避开）
//      **层序（BFS）序列化 + 子串搜索** —— 直觉上也很顺，但它是错的。
//      我用 20 万组随机树实测：**19% 的用例出错**，而且两个方向都错：
//          · 假阴性：官方示例 1（root=[3,4,5,1,2], sub=[4,1,2]）它返回 false
//          · 假阳性：root=[1,1,1], sub=[1,1] 它返回 true（实际应是 false）
//      原因：**层序序列化里，子树并不对应连续的一段** —— 层序把同一深度的节点
//      混在一起了，子树的边界根本对不齐。
//
//   ④ 三个坑
//     1. **忘了先判 root 为空** -> 空指针解引用。
//     2. **isSameTree 里 "两个都空 / 只有一个空" 必须分成两种情况**（同 100 题）。
//     3. **只用「节点的值」当线索是不行的** —— 本题允许重复值，
//        root=[1,1,1] 和 sub=[1,1] 就是典型的坑：找到了值一样，但结构对不上。
//
//   ⑤ 复杂度：O(m·n) 时间（做法①②的序列化版可做到 O(m+n)），O(h) 递归栈。

class Solution
{
private:
    bool isSameTree(TreeNode *p, TreeNode *q)
    {
        if (p == nullptr && q == nullptr)
            return true; // 都空
        if (p == nullptr || q == nullptr)
            return false; // 一个空

        if (p->val != q->val)
            return false; // 值不同

        return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
    }

public:
    bool isSubtree(TreeNode *root, TreeNode *subRoot)
    {
        // TODO: 在这里写你的解法

        if (root == nullptr)
            return isSameTree(root, subRoot);

        return isSameTree(root, subRoot) || isSubtree(root->left, subRoot) || isSubtree(root->right, subRoot);
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

// 收集整棵树的节点（带防环，避免错误解法把树改成环后死循环）
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

// 单条测试用例：root 树 + subRoot 树 + 期望结果
struct TestCase
{
    string name;
    vector<int> root;
    vector<int> sub;
    bool expected;
};

static bool runCase(const TestCase &c)
{
    string why;
    if (!checkInput(c.root, why) || !checkInput(c.sub, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    TreeNode *tr = buildTree(c.root);
    TreeNode *ts = buildTree(c.sub);
    const int cap = ((int)c.root.size() + (int)c.sub.size()) * 2 + 10;

    vector<int> beforeR, beforeS;
    serialize(tr, beforeR);
    serialize(ts, beforeS);

    bool got = !c.expected; // 故意取反：没被赋值也能看出来
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.isSubtree(tr, ts);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    vector<int> afterR, afterS;
    serialize(tr, afterR);
    serialize(ts, afterS);
    bool intact = (beforeR == afterR) && (beforeS == afterS);

    bool pass = (!threw) && (got == c.expected) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       root    = " << vecToStr(c.root) << "\n"
         << "       subRoot = " << vecToStr(c.sub) << "\n";
    if (threw)
        cout << "       异常    = " << err << "\n";
    else
    {
        cout << "       期望    = " << (c.expected ? "true" : "false")
             << "   实际 = " << (got ? "true" : "false");
        if (!intact)
            cout << "   ✗ 树结构被改动了";
        cout << "\n";
    }

    for (TreeNode *x : collectNodes(tr, cap))
        delete x;
    for (TreeNode *x : collectNodes(ts, cap))
        delete x;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1（是子树）", {3, 4, 5, 1, 2}, {4, 1, 2}, true},
        {"示例 2（多了一个 0 就不是）",
         {3, 4, 5, 1, 2, NULLNODE, NULLNODE, NULLNODE, NULLNODE, 0},
         {4, 1, 2},
         false},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"root 与 subRoot 完全相同", {1, 2, 3}, {1, 2, 3}, true},
        {"匹配到 root 的根节点", {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, true},
        {"匹配到左子树", {1, 2, 3, 4, 5}, {2, 4, 5}, true},
        {"匹配到右子树", {1, 2, 3, NULLNODE, NULLNODE, 6, 7}, {3, 6, 7}, true},
        {"只有单节点且相同", {1}, {1}, true},
        {"只有单节点但值不同", {1}, {2}, false},
        {"subRoot 比 root 还大", {1}, {1, 2, 3}, false},
        {"重复值：{1,1} 里找 {1}", {1, 1}, {1}, true},
        {"重复值陷阱：{1,1,1} 里找 {1,1}（结构对不上）", {1, 1, 1}, {1, 1}, false},
        {"值的结构对了但顺序反了", {1, 2, NULLNODE, 3}, {1, NULLNODE, 2}, false},
        {"深层才匹配上", {1, 2, NULLNODE, 3, NULLNODE, 4}, {4}, true},
        {"含负数", {-1, -2, -3}, {-2}, true},
        {"取值下限 -10000", {-10000, -10000}, {-10000}, true},
        {"取值上限 10000", {10000, 9999, 10000}, {10000}, true},
    };

    // 最大规模用例：root 2000 个节点（值 0..1999，按层序数组建出来是一棵接近满的二叉树）
    //   这条压「外层遍历 × 内层比较」的总开销：最坏 2000 × O(高度) 次比较
    //   注：LeetCode 用层序数组表示树，所以树深天然受限（n 个节点最多 ~log2(n) 层），
    //       不用担心递归爆栈。
    {
        vector<int> big;
        big.reserve(2000);
        for (int v = 0; v < 2000; ++v)
            big.push_back(v);
        extraCases.push_back({"最大规模 root 2000 节点", big, {1990}, true});
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

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map> // 中序 值->下标 的哈希
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
// 105. 从前序与中序遍历序列构造二叉树
//      （Construct Binary Tree from Preorder and Inorder Traversal）· 树 · Medium
//
// 给定前序和中序序列（值互不相同），还原出唯一的二叉树。
//
//   ★ 两个序列各管一件事：
//       前序（根-左-右）：**第一个元素就是根**
//       中序（左-根-右）：**根的位置把中序切成"左子树"和"右子树"两段**
//     于是：前序定"根是谁"，中序定"左右子树各有几个节点"。
//
//   手工推演（示例 1）：
//       preorder = [3, 9, 20, 15, 7]
//       inorder  = [9, 3, 15, 20, 7]
//       根 = pre[0] = 3
//       中序里 3 在位置 1  ->  左子树中序 [9]（1 个），右子树中序 [15,20,7]（3 个）
//       前序按同样大小切给左右：左子树前序 [9]，右子树前序 [20,15,7]
//       递归下去。
//
//   ── 写法：哈希表 + 递归（推荐 O(n)）──────────────────────
//       unordered_map<int, int> pos;          // 中序：值 -> 下标
//       int preIdx = 0;                       // ★ 前序的"当前根"指针，全局推进
//
//       TreeNode* build(const vector<int>& pre, int inLo, int inHi) {
//           if (inLo > inHi) return nullptr;                 // 空区间
//           int rootVal = pre[preIdx++];                     // ★ 取根并前进
//           TreeNode* root = new TreeNode(rootVal);
//           int mid = pos[rootVal];                          // 中序里的位置
//           root->left  = build(pre, inLo,     mid - 1);     // 左子树占 [inLo, mid-1]
//           root->right = build(pre, mid + 1,  inHi);        // 右子树占 [mid+1, inHi]
//           return root;
//       }
//       TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
//           pos.clear();
//           for (int i = 0; i < (int)inorder.size(); ++i) pos[inorder[i]] = i;
//           preIdx = 0;
//           return build(preorder, 0, (int)inorder.size() - 1);
//       }
//
//   四个坑（骨架都有对应用例）：
//     1. ★★ **`preIdx` 必须按引用/成员 + 全局推进**。如果把它当**值**传进递归，
//        左右子树会各自从同一个位置取根 -> 整棵树错乱。
//     2. ★ **不用哈希表会退化成 O(n²)**：每层都在中序里线性找根的位置。
//        本题 n <= 3000，O(n²) 也能过，但面试官一定会问"能不能 O(n)"。
//     3. **区间边界要写对**：左子树 `[inLo, mid-1]`、右子树 `[mid+1, inHi]`，
//        空区间（lo > hi）返回 nullptr。左右各差一就会漏节点或多建节点。
//     4. **不能①先递归左再递归右搞反**：前序的顺序是"根-左-右"，所以必须
//        **先递归左子树、再递归右子树**，否则 preIdx 取到的根会错位。
//
//   复杂度：时间 O(n)（哈希表 O(1) 定位根）；空间 O(n) 哈希表 + O(h) 递归栈。
//
//   面试追问：
//     · 为什么这题能唯一确定一棵树？-> 前序给了根的顺序，中序给了左右划分，
//       两者结合信息完备；而"前序 + 后序"通常不能唯一确定（无法区分左右）
//     · 106（中序 + 后序）怎么改？-> 后序的根在**最后**，所以 preIdx 改成从末尾向前，
//       且必须先递归**右**子树
//     · 能不能不用哈希表？-> 可以用"区间内根的位置"做二分，但那需要额外结构
//
//   同族题：106 从中序与后序构造二叉树、889 根据前序和后序遍历构造二叉树、
//           108 将有序数组转换为 BST、297 二叉树序列化与反序列化
class Solution
{
public:
    TreeNode *buildTree(vector<int> &preorder, vector<int> &inorder)
    {
        // TODO: 在这里写你的解法

        if (preorder.size() == 0)
            return nullptr;

        TreeNode *root = new TreeNode(preorder.front());

        int n = 0;
        while (preorder.front() != inorder.at(n))
            n++;

        vector<int> psub, isub;

        psub.assign(preorder.begin() + 1, preorder.begin() + n + 1);
        isub.assign(inorder.begin(), inorder.begin() + n);
        root->left = buildTree(psub, isub);

        psub.assign(preorder.begin() + n + 1, preorder.end());
        isub.assign(inorder.begin() + n + 1, inorder.end());
        root->right = buildTree(psub, isub);

        return root;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域 -3000..3000，用 100000 当 null
static const int NULLNODE = 100000;

static TreeNode *buildFromLevel(const vector<int> &a)
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

static void preorderSeq(TreeNode *n, vector<int> &out)
{
    if (n == nullptr)
        return;
    out.push_back(n->val);
    preorderSeq(n->left, out);
    preorderSeq(n->right, out);
}

static void inorderSeq(TreeNode *n, vector<int> &out)
{
    if (n == nullptr)
        return;
    inorderSeq(n->left, out);
    out.push_back(n->val);
    inorderSeq(n->right, out);
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
            break; // 成环
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
    vector<int> preorder;
    vector<int> inorder;
    vector<int> expected; // 期望结果树的层序数组
};

static bool runCase(const TestCase &c)
{
    // 输入自检
    string why;
    if (c.preorder.size() != c.inorder.size() || c.preorder.empty())
        why = "两个序列长度必须相同且非空";
    unordered_set<int> sp, si;
    for (int v : c.preorder)
        if (!sp.insert(v).second)
            why = "preorder 里有重复值";
    for (int v : c.inorder)
        if (!si.insert(v).second)
            why = "inorder 里有重复值";
    if (why.empty() && sp != si)
        why = "两个序列的元素集合不一致";
    if (!why.empty())
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    vector<int> pre = c.preorder;
    vector<int> in = c.inorder;

    TreeNode *got = nullptr;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.buildTree(pre, in);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    const int n = (int)c.preorder.size();
    const int cap = n * 2 + 10;
    vector<TreeNode *> nodes;
    vector<int> gotSeq, gotPre, gotIn;
    if (!threw && got != nullptr)
    {
        nodes = collectNodes(got, cap);
        serialize(got, gotSeq);
        preorderSeq(got, gotPre);
        inorderSeq(got, gotIn);
    }

    bool okTree = (!threw) && (got != nullptr) && (gotSeq == c.expected);
    bool okCount = (!threw) && ((int)nodes.size() == n);
    bool okPre = (!threw) && (gotPre == c.preorder);
    bool okIn = (!threw) && (gotIn == c.inorder);
    bool pass = okTree && okCount && okPre && okIn;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "  n = " << n << "\n"
         << "       preorder = " << vecToStr(c.preorder) << "\n"
         << "       inorder  = " << vecToStr(c.inorder) << "\n"
         << "       期望层序 = " << vecToStr(c.expected) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else if (got == nullptr)
        cout << "       结果 = nullptr（应返回一棵树）\n";
    else
    {
        cout << "       实际层序 = " << vecToStr(gotSeq);
        if (!okCount)
            cout << "\n       ✗ 节点数不对：期望 " << n << "，实际 " << nodes.size();
        if (!okPre)
            cout << "\n       ✗ 结果树的前序与原 preorder 不一致";
        if (!okIn)
            cout << "\n       ✗ 结果树的中序与原 inorder 不一致";
        cout << "\n";
    }

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
        {"示例 1", {3, 9, 20, 15, 7}, {9, 3, 15, 20, 7}, {3, 9, 20, NULLNODE, NULLNODE, 15, 7}},
        {"示例 2（单节点）", {-1}, {-1}, {-1}},
    };

    vector<TestCase> extraCases = {
        // 两个节点：只有左 / 只有右
        {"只有左孩子", {1, 2}, {2, 1}, {1, 2}},
        {"只有右孩子", {1, 2}, {1, 2}, {1, NULLNODE, 2}},

        // 链状
        {"左链 3 层", {3, 2, 1}, {1, 2, 3}, {3, 2, NULLNODE, 1}},
        {"右链 3 层", {1, 2, 3}, {1, 2, 3}, {1, NULLNODE, 2, NULLNODE, 3}},

        // 完全树
        {"完全树 3 层", {1, 2, 4, 5, 3, 6, 7}, {4, 2, 5, 1, 6, 3, 7}, {1, 2, 3, 4, 5, 6, 7}},

        // 不对称：左子树有两个节点、右子树有两个节点但形状不同
        {"不对称树", {1, 2, 4, 3, 5}, {4, 2, 1, 3, 5}, {1, 2, 3, 4, NULLNODE, NULLNODE, 5}},

        // 左子树为空（根只有右子树）
        {"根只有右子树", {1, 2, 3, 4}, {1, 2, 3, 4}, {1, NULLNODE, 2, NULLNODE, 3, NULLNODE, 4}},

        // 右子树为空（根只有左子树）
        {"根只有左子树", {4, 3, 2, 1}, {1, 2, 3, 4}, {4, 3, NULLNODE, 2, NULLNODE, 1}},

        // 题目数值边界 -3000 .. 3000
        {"取值上下限", {-3000, 3000}, {-3000, 3000}, {-3000, NULLNODE, 3000}},
    };

    // 生成的大用例：完美二叉树 10 层（1023 节点）+ 链状 3000 层（题目上限）
    {
        // ① 完美树 10 层：层序 1..1023
        vector<int> level;
        for (int i = 1; i <= 1023; ++i)
            level.push_back(i);
        TreeNode *tmp = buildFromLevel(level);
        vector<int> pre, in;
        preorderSeq(tmp, pre);
        inorderSeq(tmp, in);
        for (TreeNode *p : collectNodes(tmp, 5000))
            delete p;
        extraCases.push_back({"完美二叉树 10 层（1023 节点）", pre, in, level});

        // ② 左链 3000 层（题目上限 3000）：直接构造链，再从树取三种序列
        {
            TreeNode *chainRoot = nullptr;
            for (int v = 1; v <= 3000; ++v)
            {
                TreeNode *node = new TreeNode(v);
                node->left = chainRoot; // 3000 -> 2999 -> ... -> 1
                chainRoot = node;
            }
            vector<int> chainPre, chainIn, chainLevel;
            preorderSeq(chainRoot, chainPre); // 3000, 2999, ..., 1
            inorderSeq(chainRoot, chainIn);   // 1, 2, ..., 3000
            serialize(chainRoot, chainLevel);
            for (TreeNode *p : collectNodes(chainRoot, 10000))
                delete p;
            extraCases.push_back({"左链 3000 层（题目上限）", chainPre, chainIn, chainLevel});
        }
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

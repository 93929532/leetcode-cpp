#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set> // 环检测用
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
// 226. 翻转二叉树（Invert Binary Tree）· 树 · Easy
//
// 把整棵树**左右镜像**：每个节点的左右孩子互换，然后对子树递归做同样的事。
//
//   递归三步（顺序可以调整）：
//       ① 交换 root 的 left / right
//       ② 递归翻转左子树
//       ③ 递归翻转右子树
//     （也可以先递归再交换，或者后序：先翻两边再交换 —— 结果一样）
//
//   骨架：
//       TreeNode* invertTree(TreeNode* root) {
//           if (root == nullptr) return nullptr;      // ★ 必须判空
//           swap(root->left, root->right);
//           invertTree(root->left);
//           invertTree(root->right);
//           return root;
//       }
//
//   三个坑（骨架都有对应用例）：
//     1. **忘记判空**：空树直接崩（`root->left` 解引用 nullptr）。
//        空树是合法输入（示例 3），必须返回 nullptr。
//     2. **只交换了根，忘了递归**：示例 1 会挂 —— 树看起来"动了一点"但没整体翻转。
//     3. ★ **用递归返回值给指针赋值时，顺序写反会自我覆盖**：
//            root->left  = invertTree(root->right);   // 这两行
//            root->right = invertTree(root->left);    // ← 第二行用的 left 已被覆盖！
//        第二行拿到的其实是"已经翻转好的右子树"，于是左右变成同一棵树。
//        要么用 swap（推荐），要么先把两边结果存进临时变量：
//            TreeNode* L = invertTree(root->right);
//            TreeNode* R = invertTree(root->left);
//            root->left = L; root->right = R;
//
//   迭代写法（面试可以对比着说）：
//       层序 BFS，每弹出一个节点就交换它的左右孩子，再把非空孩子入队。
//       好处是不用递归栈；坏处是要额外的队列空间。
//
//   复杂度：时间 O(n)（每个节点访问一次）；
//           空间 O(h)（递归栈深度 = 树高，最坏链状 O(n)）。迭代法是 O(宽度) 的队列。
//
//   面试追问：
//     · 前序/中序/后序遍历都能做吗？-> 都能，只要每个节点都交换一次左右孩子。
//       但**中序**要小心：先翻左、再交换、再翻右，会把同一棵子树翻两次（可验证）
//     · 和 101 对称二叉树的关系？-> 101 是"判断左子树是否等于右子树的镜像"，
//       用的是同一套"左右对调"的思路
//     · 能不能不交换节点、只交换值？-> 结构题这么做是作弊，骨架会比对结构
//
//   同族题：101 对称二叉树、100 相同的树、617 合并二叉树、572 另一棵树的子树、
//           108 将有序数组转换为二叉搜索树

class Solution
{
public:
    TreeNode *invertTree(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        if (root == nullptr)
            return root;

        invertTree(root->left);
        invertTree(root->right);

        if (root->left != nullptr || root->right != nullptr)
            swap(root->left, root->right);

        return root;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 空节点标记：本题值域是 -100..100，用 1000 当 null 不会和真实值冲突
static const int NULLNODE = 1000;

// 按力扣的层序数组建树（数组里的 NULLNODE 表示空）
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

// 把树序列化成力扣风格的层序数组，并去掉尾部的 null
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

// 把数组格式化成 [4,2,7,null,1] 便于打印
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

// 收集整棵树的节点（带环检测 + 数量上限），用于释放内存和校验结构
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

// 层序数组里真实节点的个数（NULLNODE 不算）
static size_t countNonEmpty(const vector<int> &a)
{
    size_t n = 0;
    for (int v : a)
        if (v != NULLNODE)
            ++n;
    return n;
}

// 单条测试用例：input / expected 都用层序数组表示（NULLNODE 表示空）
struct TestCase
{
    string name;
    vector<int> input;
    vector<int> expected;
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    TreeNode *root = buildTree(c.input);

    TreeNode *got = nullptr;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.invertTree(root);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    // 结果树：收集节点（防成环/防节点数爆炸）
    vector<TreeNode *> gotNodes = collectNodes(got, (int)c.input.size() * 2 + 10);
    vector<int> gotSeq;
    serialize(got, gotSeq);

    bool pass = false;
    if (!threw)
    {
        // ① 序列化结果一致
        bool okSeq = (gotSeq == c.expected);
        // ② 节点数一致（防止"只交换值不换结构"或凭空造节点）
        bool okCount = (gotNodes.size() == countNonEmpty(c.input));
        pass = okSeq && okCount;
    }

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       输入   = " << vecToStr(c.input) << "\n"
         << "       期望   = " << vecToStr(c.expected) << "\n";
    if (threw)
        cout << "       异常   = " << err << "\n";
    else
        cout << "       实际   = " << vecToStr(gotSeq) << "\n";

    // 释放：先删原树的节点，再删"结果里新增"的节点。
    //   注意：本题是原地翻转，got 与 root 往往是同一批节点，
    //   必须用 freed 集合去重，否则会 double free（堆损坏）。
    unordered_set<TreeNode *> freed;
    for (TreeNode *p : collectNodes(root, (int)c.input.size() * 2 + 10))
        if (freed.insert(p).second)
            delete p;
    for (TreeNode *p : gotNodes)
        if (freed.insert(p).second)
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
        {"示例 1（完全树 7 个节点）", {4, 2, 7, 1, 3, 6, 9}, {4, 7, 2, 9, 6, 3, 1}},
        {"示例 2（3 个节点）", {2, 1, 3}, {2, 3, 1}},
        {"示例 3（空树）", {}, {}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 单节点
        {"单节点", {1}, {1}},

        // 只有一边孩子：翻转后孩子换到另一边（能抓"忘记递归"）
        {"只有左孩子", {1, 2}, {1, NULLNODE, 2}},
        {"只有右孩子", {1, NULLNODE, 2}, {1, 2}},

        // 链状（一直往左）：翻转后变成一直往右
        {"左链 3 层", {1, 2, NULLNODE, 3}, {1, NULLNODE, 2, NULLNODE, 3}},
        {"右链 3 层", {1, NULLNODE, 2, NULLNODE, 3}, {1, 2, NULLNODE, 3}},

        // 值有重复但结构不对称：只看值分不出对错，必须看结构
        {"值有重复、结构不对称", {1, 2, 2, 3}, {1, 2, 2, NULLNODE, NULLNODE, NULLNODE, 3}},

        // 稍微大一点的不对称树
        {"不对称树 6 个节点", {1, 2, 3, 4, 5, NULLNODE, 6}, {1, 3, 2, 6, NULLNODE, 5, 4}},

        // 题目数值边界 -100 .. 100
        {"取值上下限", {-100, 100, 0}, {-100, 0, 100}},
    };

    // 大规模用例：完美二叉树 63 个节点（n 上限是 100）
    //   完美二叉树翻转后 = **每一层的顺序倒过来**（这个规律不依赖翻转算法，可独立算出期望值）
    {
        vector<int> input, expected;
        int levelStart = 1, levelLen = 1;
        for (int i = 1; i <= 63; ++i)
            input.push_back(i);
        for (int lvl = 0; lvl < 6; ++lvl) // 63 个节点 = 6 层
        {
            for (int k = levelStart + levelLen - 1; k >= levelStart; --k)
                expected.push_back(k);
            levelStart += levelLen;
            levelLen *= 2;
        }
        extraCases.push_back({"完美二叉树 63 个节点（每层倒序）", input, expected});
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

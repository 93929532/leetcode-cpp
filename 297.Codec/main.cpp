#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <sstream>       // 序列化常用
#include <algorithm>     // count_if / max
#include <unordered_map> // 序列化时常用（值 -> 下标 / 缓存）
#include <unordered_set> // 环检测 + 指针去重
#include <stdexcept>     // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX // ★ 阻止 <windows.h> 定义 min/max 宏（否则 max({...}) 会被破坏）
#endif
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
// 297. 二叉树的序列化与反序列化（Serialize and Deserialize Binary Tree）· Hard
//
// 设计两个互逆的函数：
//     string    serialize(TreeNode* root)     把树变成字符串
//     TreeNode* deserialize(string data)      把字符串还原成树
// 要求 **deserialize(serialize(tree)) == tree**（结构 + 值都一致）。
//
//   ★★ 核心难点只有一个：**必须把 null 也编码进去**。
//      如果只存值（比如前序 "1,2,3"），[1,2] 和 [1,null,2] 会编码成同一个串 ——
//      结构信息丢了。所以一定要有"空节点标记"（常用 `#` 或 `null`）。
//
//   ── 方案 A：前序 + `#` 标记（最简单，推荐）────────────────────
//       serialize：DFS 前序，遇到空节点写 "#"，用 ',' 分隔
//           void enc(TreeNode* n, string& s) {
//               if (n == nullptr) { s += "#,"; return; }
//               s += to_string(n->val) + ",";
//               enc(n->left, s);
//               enc(n->right, s);
//           }
//           // 例如 [1,2,3,null,null,4,5] -> "1,2,#,#,3,4,#,#,5,#,#,"
//
//       deserialize：同样按前序递归读取
//           TreeNode* dec(istringstream& in) {
//               string tok;
//               getline(in, tok, ',');
//               if (tok == "#") return nullptr;
//               TreeNode* n = new TreeNode(stoi(tok));
//               n->left  = dec(in);
//               n->right = dec(in);
//               return n;
//           }
//
//   ── 方案 B：层序 BFS + null 标记（和力扣的数组表示一致）──────────
//       serialize 用队列层序输出，null 写 "#"；deserialize 用队列按层重建。
//       好处是字符串和题目里的 `[1,2,3,null,null,4,5]` 形式一一对应。
//
//   四个坑（骨架都有对应用例）：
//     1. ★★ **漏掉 null 标记** -> 结构丢失，`[1,2]` 与 `[1,null,2]` 混淆。
//     2. ★ **分隔符和标记要选对**：值里可能有负号（如 `-5`），所以标记别用 `-`；
//        解析时也**不能按单个字符切**（多位数、负数）。用 `,` 分隔 + `stoi`。
//     3. **空树**：`serialize(nullptr)` 要能产出合法串（如 `"#"`），
//        `deserialize` 要能还原成 nullptr。
//     4. **返回的必须是新树**：不能把原树指针存起来再还回去（骨架会检查两棵树的
//        节点集合没有交集）。
//
//   复杂度：时间 O(n)、空间 O(n)（字符串）+ O(h) 递归栈。
//
//   面试追问：
//     · 为什么 BST 可以只用前序不需要 null？-> 449 题：BST 大小关系决定了结构，
//       前序序列足以唯一还原（本质是"有序性"提供了结构信息）
//     · 能不能用后序/中序？-> 中序不行（不能唯一确定树）；前序/后序 + null 都行
//     · 数据量大时怎么省空间？-> 用二进制编码 / 变长整数，或者共享重复子树（652 题）
//     · 多线程/流式？-> 需要设计带长度前缀或自描述的分帧格式
//
//   同族题：449 序列化与反序列化 BST、428 序列化与反序列化 N 叉树、
//           652 寻找重复的子树、105 前序+中序构造二叉树（同样靠遍历还原结构）
class Codec
{
private:
    void encodeTree(TreeNode *root, string &data)
    {
        if (root == nullptr)
            data += "#,";
        else
        {
            data += to_string(root->val) + ",";
            encodeTree(root->left, data);
            encodeTree(root->right, data);
        }
    }

    TreeNode *decodeTree(string &data)
    {
        if (data.empty())
            return nullptr;

        size_t pos = data.find(',');
        string token = data.substr(0, pos);
        data.erase(0, pos + 1);

        if (token == "#")
            return nullptr;

        TreeNode *node = new TreeNode(stoi(token));
        node->left = decodeTree(data);
        node->right = decodeTree(data);
        return node;
    }

public:
    // 把树编码成字符串
    string serialize(TreeNode *root)
    {
        // TODO: 在这里写你的解法

        string data;
        encodeTree(root, data);
        return data;
    }

    // 把字符串解码成树
    TreeNode *deserialize(string data)
    {
        // TODO: 在这里写你的解法

        return decodeTree(data);
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

// 层序序列化（去掉尾部 null），用于和期望值比对
static void serializeLevel(TreeNode *root, vector<int> &out)
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
    vector<int> tree; // 原树的层序数组（NULLNODE = 空）
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
    const int n = (int)count_if(c.tree.begin(), c.tree.end(),
                                [](int v)
                                { return v != NULLNODE; });

    string data;
    TreeNode *got = nullptr;
    bool threw = false;
    string err;
    try
    {
        Codec codec;
        data = codec.serialize(root);
        got = codec.deserialize(data);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    vector<int> gotSeq, gotNodeVals;
    vector<TreeNode *> gotNodes;
    if (!threw && got != nullptr)
    {
        serializeLevel(got, gotSeq);
        gotNodes = collectNodes(got, cap);
    }

    // 校验
    // 空树还原成 nullptr 才是对的；非空树才要求结构与值一致
    bool okTree = (n == 0) ? ((!threw) && (got == nullptr))
                           : ((!threw) && (got != nullptr) && (gotSeq == c.tree));
    bool okCount = (!threw) && ((int)gotNodes.size() == n);
    bool okData = (n == 0) ? true : (!data.empty());

    // 关键：还原出的必须是"新树"，不能把原树指针还回来
    bool okIndependent = true;
    if (!threw && got != nullptr)
    {
        unordered_set<TreeNode *> orig;
        for (TreeNode *p : collectNodes(root, cap))
            orig.insert(p);
        for (TreeNode *p : gotNodes)
            if (orig.count(p))
            {
                okIndependent = false;
                break;
            }
    }

    bool pass = okTree && okCount && okData && okIndependent;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "  节点数 = " << n << "\n"
         << "       原树   = " << vecToStr(c.tree) << "\n"
         << "       编码串 = ";
    if (threw)
        cout << "(异常)";
    else if (data.size() > 60)
        cout << "\"" + data.substr(0, 60) + "…\"（长度 " + to_string(data.size()) + "）";
    else
        cout << "\"" + data + "\"";
    cout << "\n";

    if (threw)
        cout << "       异常 = " << err << "\n";
    else if (got == nullptr)
        cout << "       还原结果 = nullptr\n";
    else
    {
        cout << "       还原   = " << vecToStr(gotSeq);
        if (!okTree)
            cout << "\n       ✗ 还原出的树与原树不一致";
        if (!okCount)
            cout << "\n       ✗ 节点数不对：期望 " << n << "，实际 " << gotNodes.size();
        if (!okIndependent)
            cout << "\n       ✗ 返回的节点与原树是同一批指针（必须是新建的树）";
        if (!okData)
            cout << "\n       ✗ 非空树却序列化出了空串";
        cout << "\n";
    }

    // 释放：用 freed 集合去重 —— 若解法把原树指针还回来（错误实现），
    // 两个循环会删同一批节点，导致 double free（堆损坏）。
    unordered_set<TreeNode *> freed;
    for (TreeNode *p : collectNodes(root, cap))
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

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 2, 3, NULLNODE, NULLNODE, 4, 5}},
        {"示例 2（空树）", {}},
        {"示例 3（单节点）", {1}},
    };

    vector<TestCase> extraCases = {
        // ★ 结构必须能区分：只有左孩子 vs 只有右孩子
        {"只有左孩子", {1, 2}},
        {"只有右孩子", {1, NULLNODE, 2}},

        // 链状
        {"左链 3 层", {1, 2, NULLNODE, 3}},
        {"右链 3 层", {1, NULLNODE, 2, NULLNODE, 3}},
        {"左链 5 层", {5, 4, NULLNODE, 3, NULLNODE, 2, NULLNODE, 1}},

        // 完全树
        {"完全树 3 层", {1, 2, 3, 4, 5, 6, 7}},
        {"完全树 4 层",
         {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}},

        // 负数值：考验解析（不能用 '-' 当分隔符）
        {"负数", {-1, -2, -3}},
        {"负数与零混合", {0, -1000, 1000, NULLNODE, -1}},
        {"全是负数", {-5, NULLNODE, -3, NULLNODE, -1}},

        // 值全相同：只能靠结构区分
        {"值全相同 5 节点", {5, 5, 5, 5, 5}},
        {"值全相同（单侧）", {7, 7, NULLNODE, 7, NULLNODE, 7}},

        // 不对称的深树
        {"左深右浅", {5, 4, 8, 11, NULLNODE, 13, 4, 7, 2, NULLNODE, NULLNODE, NULLNODE, 1}},

        // 题目数值边界
        {"取值上下限", {-1000, 1000, 1000}},
    };

    // 大用例：完美二叉树 10 层（1023 节点）+ 深链 5000 层（题目上限 10^4）
    {
        vector<int> level;
        for (int i = 1; i <= 1023; ++i)
            level.push_back(i - 512); // 映射到 [-511,511]，不超出题目值域
        extraCases.push_back({"完美二叉树 10 层（1023 节点）", level});

        vector<int> chain;
        for (int i = 0; i < 5000; ++i)
        {
            chain.push_back((i % 201) - 100);
            if (i + 1 < 5000)
                chain.push_back(NULLNODE);
        }
        extraCases.push_back({"右链 5000 层（压递归栈）", chain});
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

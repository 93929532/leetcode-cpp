#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> // 原节点 -> 副本节点
#include <unordered_set> // 深拷贝校验：副本不能复用原节点
#include <stdexcept>     // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// 带随机指针的链表节点（与 LeetCode 定义一致）
class Node
{
public:
    int val;
    Node *next;
    Node *random;
    Node(int _val)
    {
        val = _val;
        next = nullptr;
        random = nullptr;
    }
};

// ===================== 答题区域 =====================
// 138. 复制带随机指针的链表（Copy List with Random Pointer）· 链表 + 哈希 · Medium
//
// 返回一条**完全新的**链表：结构（val / next / random 的指向关系）与原链表一模一样，
// 但所有节点都是新 new 出来的，**不能有任何指针指向原链表的节点**。
//
//   难点只有一句话：**random 可能指向链表里任意一个节点，包括还没创建出来的节点。**
//   所以"边走边复制"这种一遍扫描走不通 —— 走到 B 的时候，B->random 可能指向 Z，
//   而 Z 还没被创建。
//
//   思路一 · 哈希表两遍（★推荐）
//       第一遍：只管建节点、连 next，同时记下 map<原节点, 副本>
//       第二遍：补 random —— 副本->random = map[原->random]
//
//       骨架：
//           unordered_map<Node*, Node*> mp;
//           for (Node* p = head; p; p = p->next)
//               mp[p] = new Node(p->val);          // ★ 这里正是 operator[] 的语义：不存在就建
//           for (Node* p = head; p; p = p->next) {
//               mp[p]->next   = p->next   ? mp[p->next]   : nullptr;
//               mp[p]->random = p->random ? mp[p->random] : nullptr;   // ★ 必须判空
//           }
//           return head ? mp[head] : nullptr;
//
//       ★ 这里用 operator[] 是对的：第一遍之后每个 key 都一定存在，
//         它"不存在就建"的副作用正好用来建副本；换成 find 反而更啰嗦。
//       ★ 但**空指针不能当 key**：`mp[p->random]` 在 random 为 nullptr 时
//         会给 nullptr 建一个条目（无害但语义错），所以必须 `p->random ? ... : nullptr`。
//       ★ 返回时也要判 head 为空：`head ? mp[head] : nullptr`。
//
//   思路二 · 递归 + 记忆化
//       dfs(node)：若 node 为空返回空；若 map 里已有副本就返回它；
//       否则建副本、存 map、再递归连 next 和 random。
//       直观，但递归深度 = n（n ≤ 1000，不会爆栈）。
//
//   思路三 · 原地穿插（O(1) 空间，LeetCode 的 follow-up）
//       ① 把副本插到原节点后面：A → A' → B → B' → …
//       ② 补 random：A'.random = A.random ? A.random->next : nullptr
//          （因为 A.random 的 next 就是它的副本）
//       ③ 拆成两条链表，并**还原原链表**
//       三个坑：random 为空的判空、拆链时先把 next 记下来、别把原链表改坏。
//
//   两个坑（骨架都会检查）：
//     1. **浅拷贝**：直接 `return head` —— 骨架会核对"副本的节点是否都是新节点"
//     2. **random 指向原节点**：只复制了 val 和 next，random 直接复用原指针 ——
//        同样会被"副本的所有指针必须指向副本自己"这一条抓出来
//
//   复杂度：哈希法 O(n) 时间 / O(n) 空间；穿插法 O(n) 时间 / O(1) 空间。
//
//   面试追问：
//     · 能把 O(n) 空间降到 O(1) 吗？-> 思路三（穿插 + 拆分）
//     · 原链表要不要保持不变？-> 要（穿插法拆完必须还原，否则原链表被破坏）
//     · 和 133 克隆图什么关系？-> 一模一样的"深拷贝 + 哈希映射"思路
//
//   同族题：133 克隆图（同套路）、143 重排链表、2 两数相加、206 反转链表
class Solution
{
public:
    Node *copyRandomList(Node *head)
    {
        // TODO: 在这里写你的解法

        if (head == nullptr)
            return nullptr;

        unordered_map<Node *, Node *> mp;
        Node *newHead = new Node(head->val);

        Node *oldCur = head, *newCur = newHead;
        while (oldCur != nullptr)
        {
            if (oldCur->next != nullptr)
                newCur->next = new Node(oldCur->next->val);
            else
                newCur->next = nullptr;

            mp[oldCur] = newCur;

            oldCur = oldCur->next;
            newCur = newCur->next;
        }

        oldCur = head, newCur = newHead;
        while (oldCur != nullptr)
        {
            if (oldCur->random != nullptr)
                newCur->random = mp[oldCur->random];
            else
                newCur->random = nullptr;

            oldCur = oldCur->next;
            newCur = newCur->next;
        }

        return newHead;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 按 (vals, randomIdx) 构建带随机指针的链表；randomIdx[i] == -1 表示 null
// nodes 输出所有节点指针（按顺序），便于事后释放与身份校验
static Node *buildRandomList(const vector<int> &vals, const vector<int> &randomIdx, vector<Node *> &nodes)
{
    nodes.clear();
    for (int v : vals)
        nodes.push_back(new Node(v));
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        if (i + 1 < nodes.size())
            nodes[i]->next = nodes[i + 1];
        int r = (i < randomIdx.size()) ? randomIdx[i] : -1;
        nodes[i]->random = (r < 0) ? nullptr : nodes[r];
    }
    return nodes.empty() ? nullptr : nodes[0];
}

// 把链表收集成节点数组；超过上限视为异常（防成环把骨架卡死）
static vector<Node *> collectNodes(Node *head, int cap, bool &cycle, bool &overCap)
{
    vector<Node *> v;
    unordered_set<Node *> seen;
    cycle = false;
    overCap = false;
    for (Node *p = head; p != nullptr; p = p->next)
    {
        if (!seen.insert(p).second)
        {
            cycle = true;
            break;
        }
        v.push_back(p);
        if ((int)v.size() > cap)
        {
            overCap = true;
            break;
        }
    }
    return v;
}

// 打印成 [[7,null],[13,0],...] 的形式：random 用下标表示，null 表示空
static string dumpList(Node *head, size_t limit = 8)
{
    bool cycle = false, overCap = false;
    const int cap = 4000;
    vector<Node *> v = collectNodes(head, cap, cycle, overCap);

    unordered_map<Node *, int> pos;
    for (size_t i = 0; i < v.size(); ++i)
        pos[v[i]] = (int)i;

    string s = "[";
    size_t show = v.size() < limit ? v.size() : limit;
    for (size_t i = 0; i < show; ++i)
    {
        if (i)
            s += ",";
        s += "[" + to_string(v[i]->val) + ",";
        if (v[i]->random == nullptr)
            s += "null";
        else
        {
            auto it = pos.find(v[i]->random);
            s += (it == pos.end()) ? "?" : to_string(it->second);
        }
        s += "]";
    }
    if (v.size() > show)
        s += ",…（共 " + to_string(v.size()) + " 个节点）";
    s += "]";
    if (cycle)
        s += " ← 成环";
    else if (overCap)
        s += " ← 节点数超上限";
    return s;
}

// 单条测试用例：输入用 (val 数组, random 下标数组) 表示
struct TestCase
{
    string name;
    vector<int> vals;
    vector<int> randomIdx; // -1 = null
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<Node *> orig;
    Node *head = buildRandomList(c.vals, c.randomIdx, orig);
    const size_t n = orig.size();

    // 记录原链表的结构快照（用于事后确认"原链表没被改坏"）
    vector<int> origVals, origRand;
    {
        unordered_map<Node *, int> pos;
        for (size_t i = 0; i < n; ++i)
            pos[orig[i]] = (int)i;
        for (size_t i = 0; i < n; ++i)
        {
            origVals.push_back(orig[i]->val);
            origRand.push_back(orig[i]->random ? pos[orig[i]->random] : -1);
        }
    }
    const unordered_set<Node *> origSet(orig.begin(), orig.end());

    Node *copyHead = nullptr;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        copyHead = sol.copyRandomList(head);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    if (threw)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << err << endl;
        for (Node *p : orig)
            delete p;
        return false;
    }

    // ---------- 校验 ----------
    bool cycle = false, overCap = false;
    vector<Node *> cp = collectNodes(copyHead, (int)n + 10, cycle, overCap);

    // ① 节点数一致
    bool okCount = (cp.size() == n) && !cycle && !overCap;

    // ② 深拷贝：副本节点不能是原节点
    bool okDeep = true;
    for (Node *p : cp)
        if (origSet.count(p))
        {
            okDeep = false;
            break;
        }

    // ③ val 序列一致
    bool okVals = okCount;
    for (size_t i = 0; okVals && i < n; ++i)
        if (cp[i]->val != c.vals[i])
            okVals = false;

    // ④ random 指向正确（必须指向副本自己，而不是原链表）
    unordered_map<Node *, int> cpPos;
    for (size_t i = 0; i < cp.size(); ++i)
        cpPos[cp[i]] = (int)i;

    bool okRandom = okCount;
    string randomBadAt;
    for (size_t i = 0; okRandom && i < n; ++i)
    {
        int want = c.randomIdx[i];
        Node *p = cp[i]->random;
        if (want < 0)
        {
            if (p != nullptr)
            {
                okRandom = false;
                randomBadAt = "第 " + to_string(i) + " 个节点该是 null";
            }
        }
        else
        {
            auto it = cpPos.find(p);
            if (it == cpPos.end() || it->second != want)
            {
                okRandom = false;
                randomBadAt = "第 " + to_string(i) + " 个节点的 random 应指向 #" + to_string(want);
            }
        }
    }

    // ⑤ 原链表必须保持原样
    bool okOrig = true;
    {
        vector<Node *> now = collectNodes(head, (int)n + 10, cycle, overCap);
        if (now.size() != n)
            okOrig = false;
        else
        {
            unordered_map<Node *, int> pos;
            for (size_t i = 0; i < n; ++i)
                pos[now[i]] = (int)i;
            for (size_t i = 0; okOrig && i < n; ++i)
            {
                if (now[i]->val != origVals[i])
                    okOrig = false;
                int r = now[i]->random ? pos[now[i]->random] : -1;
                if (r != origRand[i])
                    okOrig = false;
            }
        }
    }

    bool pass = okCount && okDeep && okVals && okRandom && okOrig;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       输入（原链表） = " << dumpList(head) << "\n"
         << "       返回（副本）   = " << dumpList(copyHead) << "\n"
         << "       期望结构与输入完全一致，且节点全新";
    if (!pass)
    {
        if (!okCount)
            cout << "\n       ✗ 节点数不对：期望 " << n << "，实际 " << cp.size()
                 << (cycle ? "（副本成环）" : "") << (overCap ? "（超上限）" : "");
        if (!okDeep)
            cout << "\n       ✗ 浅拷贝：副本里出现了原链表的节点";
        if (okCount && !okVals)
            cout << "\n       ✗ val 序列不一致";
        if (okCount && !okRandom)
            cout << "\n       ✗ random 指向错误：" << randomBadAt;
        if (!okOrig)
            cout << "\n       ✗ 原链表被改变了（穿插法要记得还原）";
    }
    cout << endl;

    // 释放：先原链表，再副本（副本里如果有复用原节点的，跳过避免二次释放）
    for (Node *p : orig)
        delete p;
    for (Node *p : cp)
        if (!origSet.count(p))
            delete p;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 4 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {7, 13, 11, 10, 1}, {-1, 0, 4, 2, 0}},
        {"示例 2", {1, 2}, {1, 1}},
        {"示例 3", {3, 3, 3}, {-1, 0, -1}},
        {"示例 4（空链表）", {}, {}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 单节点：random 为空 / 指向自己
        {"单节点，random = null", {1}, {-1}},
        {"单节点，random 指向自己", {1}, {0}},

        // 两个节点互相指（random 和 next 方向相反）
        {"两个节点互相指", {1, 2}, {1, 0}},

        // random 的几种极端分布
        {"全部 random = null", {1, 2, 3, 4, 5}, {-1, -1, -1, -1, -1}},
        {"全部 random 指向自己", {1, 2, 3, 4}, {0, 1, 2, 3}},
        {"全部 random 指向第一个", {1, 2, 3, 4}, {0, 0, 0, 0}},
        {"全部 random 指向最后一个", {1, 2, 3, 4}, {3, 3, 3, 3}},

        // 值全部相同：只看 val 分不出副本对不对，靠 random 结构与身份校验
        {"值全相同（4 个 5）", {5, 5, 5, 5}, {2, 2, 2, 2}},

        // 题目数值边界 -10^4 .. 10^4
        {"取值上下限", {-10000, 10000, 0}, {1, 2, 0}},
    };

    // 最大规模：n = 1000（题目上限），random 用固定公式构造
    {
        const int n = 1000;
        vector<int> vals(n), idx(n);
        for (int i = 0; i < n; ++i)
        {
            vals[i] = (i * 7) % 20001 - 10000; // 落在 -10^4..10^4
            idx[i] = (i % 7 == 0) ? -1 : (i * 37 + 11) % n;
        }
        extraCases.push_back({"最大规模 n=1000", vals, idx});
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

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> // 结构快照校验用（原节点 -> 下标）
#include <stdexcept>     // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// 链表节点（与 LeetCode 定义一致）
struct ListNode
{
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *n) : val(x), next(n) {}
};

// ===================== 答题区域 =====================
// 141. 环形链表（Linked List Cycle）· 链表 · Easy
//
// 判断链表里有没有环。题目 follow-up 明确要求 **O(1) 空间** ——
// 所以标准答案就是 **Floyd 快慢指针**（龟兔赛跑）。
//
//   核心：slow 每次走 1 步，fast 每次走 2 步。
//     · **没有环**：fast 会先撞到 nullptr，循环自然结束 -> false
//     · **有环**  ：两个指针都会进环，fast 相对 slow 每轮多走 1 步，
//                   环内距离每轮缩小 1，必然在环内追上 slow -> true
//
//   骨架：
//       ListNode* slow = head;
//       ListNode* fast = head;
//       while (fast != nullptr && fast->next != nullptr) {
//           slow = slow->next;
//           fast = fast->next->next;
//           if (slow == fast) return true;        // ★ 判定放在移动之后
//       }
//       return false;
//
//   三个坑（骨架里都有对应用例）：
//     1. **循环条件的顺序不能反**：必须 `fast && fast->next`。
//        靠短路求值保证"fast 非空时才去看 fast->next"；
//        写成 `fast->next && fast` 会先解引用空指针 -> 崩。
//     2. **相遇判定必须在移动之后**：放前面的话 `slow == fast == head` 一开始就成立，
//        任何链表都会被判成有环（全错）。
//     3. **`fast = fast->next->next` 依赖 `fast->next != nullptr`**：
//        偶数长度且无环的链表（如 [1,2]、[1,2,3,4]）恰好会在 fast 走到 null 时退出；
//        少判一个条件就会崩 —— 骨架里专门放了偶数长度无环的用例。
//
//   为什么一定相遇（面试会问）：
//       设环长为 L。两指针都进环后，fast 每轮比 slow 多走 1 步，
//       它们在环内的距离 d 每轮减 1，最多 L 轮后 d = 0 —— 必相遇。
//       所以有环时最多走 O(n + L) = O(n) 步。
//
//   复杂度：时间 O(n)，空间 O(1)。
//
//   另一种写法（O(n) 空间，面试可当对照说）：
//       用 unordered_set<ListNode*> 记录走过的节点，第一次重复出现即有环。
//       代码更短，但违背 follow-up 的 O(1) 要求。
//
//   面试追问：
//     · 142「环形链表 II」要返回**入环点**怎么办？
//       -> 相遇后把一个指针放回 head，两个都改成每次走 1 步，
//          再次相遇的位置就是入环点（推导：a + nL + b 的关系）
//     · fast 走 3 步行不行？-> 可以，但"相对速度 1"最容易证明也最容易写对；
//       走 3 步在环长不是 2 的倍数时可能直接跳过 slow，分析复杂得多
//     · 空链表 / 单节点自环？-> 骨架里都有用例
//
//   同族题：142 环形链表 II（找入环点）、287 寻找重复数（数组上的 Floyd）、
//           876 链表的中间结点（同为快慢指针）
class Solution
{
public:
    bool hasCycle(ListNode *head)
    {
        // TODO: 在这里写你的解法

        ListNode *fast = head, *slow = head;

        while (fast != nullptr && fast->next != nullptr)
        {
            slow = slow->next;
            fast = fast->next->next;

            if (fast == slow)
                return true;
        }
        return false;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 按数组建链表；pos >= 0 时把尾节点接到第 pos 个节点，形成环
static ListNode *buildListWithCycle(const vector<int> &vals, int pos, vector<ListNode *> &nodes)
{
    nodes.clear();
    for (int v : vals)
        nodes.push_back(new ListNode(v));
    for (size_t i = 0; i + 1 < nodes.size(); ++i)
        nodes[i]->next = nodes[i + 1];
    if (pos >= 0 && pos < (int)nodes.size())
        nodes.back()->next = nodes[pos];
    return nodes.empty() ? nullptr : nodes[0];
}

// 结构快照：按 nodes 顺序记录 (val, next 指向的下标；-1 = nullptr，-999 = 指向表外节点)
// 用 nodes 顺序而不是遍历，所以**对成环的链表也安全**
static vector<pair<int, int>> snapshot(const vector<ListNode *> &nodes)
{
    unordered_map<ListNode *, int> pos;
    for (int i = 0; i < (int)nodes.size(); ++i)
        pos[nodes[i]] = i;
    vector<pair<int, int>> s;
    for (ListNode *p : nodes)
    {
        int nxt;
        if (p->next == nullptr)
            nxt = -1;
        else
        {
            auto it = pos.find(p->next);
            nxt = (it == pos.end()) ? -999 : it->second;
        }
        s.push_back({p->val, nxt});
    }
    return s;
}

// 把链表格式化成 [3,2,0,-4]，并标出环的位置
static string listToString(const vector<int> &vals, int pos)
{
    string s = "[";
    for (size_t i = 0; i < vals.size() && i < 12; ++i)
    {
        if (i)
            s += ",";
        s += to_string(vals[i]);
    }
    if (vals.size() > 12)
        s += ",…（共 " + to_string(vals.size()) + " 个节点）";
    s += "]";
    s += (pos >= 0) ? ("，尾 -> 下标 " + to_string(pos) + "（成环）") : "，无环";
    return s;
}

// 单条测试用例
struct TestCase
{
    string name;
    vector<int> vals; // 节点值序列
    int pos;          // 尾节点接回的下标；-1 表示无环
    bool expected;    // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<ListNode *> nodes;
    ListNode *head = buildListWithCycle(c.vals, c.pos, nodes);

    const vector<pair<int, int>> before = snapshot(nodes);

    bool got = false;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.hasCycle(head);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    if (threw)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << err << endl;
        for (ListNode *p : nodes)
            delete p;
        return false;
    }

    // 结构是否被改动（比如为了"标记访问过"去改 val 或 next）
    const vector<pair<int, int>> after = snapshot(nodes);
    bool intact = (before == after);

    bool pass = (got == c.expected) && intact;

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  n = " << c.vals.size() << "\n"
         << "       head = " << listToString(c.vals, c.pos) << "\n"
         << "       期望 = " << (c.expected ? "true" : "false")
         << "   实际 = " << (got ? "true" : "false");
    if (!intact)
        cout << "\n       ✗ 链表结构被改动了（不能通过修改节点来标记访问）";
    cout << endl;

    // 逐个释放（按 nodes 顺序释放，对成环链表也安全）
    for (ListNode *p : nodes)
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
        {"示例 1（环在中间）", {3, 2, 0, -4}, 1, true},
        {"示例 2（两个节点成环）", {1, 2}, 0, true},
        {"示例 3（单节点无环）", {1}, -1, false},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 空链表 / 单节点自环
        {"空链表", {}, -1, false},
        {"单节点自环（pos=0）", {1}, 0, true},

        // ★ 偶数长度且无环：专门抓 "fast = fast->next->next" 缺判空导致的崩溃
        {"两个节点无环（偶数长度）", {1, 2}, -1, false},
        {"四个节点无环（偶数长度）", {1, 2, 3, 4}, -1, false},
        {"奇数长度无环", {1, 2, 3}, -1, false},

        // 环的位置：头 / 中 / 尾
        {"环在头部（pos=0）", {1, 2, 3}, 0, true},
        {"环在中间（pos=2）", {1, 2, 3, 4, 5}, 2, true},
        {"尾部自环（pos=n-1）", {1, 2, 3}, 2, true},

        // 值全相同：看不出环，只能靠指针
        {"值全相同（只能靠指针判断）", {5, 5, 5, 5}, 1, true},

        // 题目数值边界 -10^5 .. 10^5
        {"取值上下限", {-100000, 100000}, 0, true},
    };

    // 长链表用例：n = 1000（题目上限）
    {
        const int n = 1000;
        vector<int> vals(n);
        for (int i = 0; i < n; ++i)
            vals[i] = i;

        vector<int> noCycle = vals;
        extraCases.push_back({"长链 n=1000 无环", noCycle, -1, false});

        vector<int> cycleMid = vals;
        extraCases.push_back({"长链 n=1000 环在中间（pos=500）", cycleMid, 500, true});

        vector<int> cycleHead = vals;
        extraCases.push_back({"长链 n=1000 环在头部（pos=0）", cycleHead, 0, true});
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

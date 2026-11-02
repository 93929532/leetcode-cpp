#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> // 结构快照 + 把返回的指针换算成下标
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
// 142. 环形链表 II（Linked List Cycle II）· 链表 · Medium
//
// 141 只问"有没有环"，这题要**返回入环的第一个节点**（没有环返回 nullptr）。
// follow-up 同样要求 O(1) 空间 —— 所以还是 Floyd，但要加**第二段**。
//
//   两段式：
//     ① 判环：slow 走 1、fast 走 2，相遇就说明有环（同 141）
//     ② 找入口：把 slow 放回 head，两人都改成每次走 1 步，
//        再次相遇的位置就是入环点
//
//   代码：
//       ListNode* slow = head;
//       ListNode* fast = head;
//       while (fast != nullptr && fast->next != nullptr) {
//           slow = slow->next;
//           fast = fast->next->next;
//           if (slow == fast) {                  // ① 相遇
//               slow = head;                     // ② 一个指针回 head
//               while (slow != fast) {
//                   slow = slow->next;
//                   fast = fast->next;           // 两人同速
//               }
//               return slow;                     // 相遇点 = 入环点
//           }
//       }
//       return nullptr;                          // 无环
//
//   ★ 为什么第二段能对上（面试必问，一定要会推）：
//       设 head 到入环点的距离 = a，入环点到相遇点的距离 = b，环长 = L。
//       相遇时：slow 走了 a + b；fast 走了 a + b + kL（k ≥ 1，多绕了 k 圈）。
//       又因为 fast 速度是 slow 的两倍：  a + b + kL = 2(a + b)
//       化简得：                        a = kL - b
//       现在把 slow 放回 head：
//         · slow 走 a 步 -> 正好到入环点
//         · fast 从相遇点走 a 步 = kL - b 步 -> 也是"从相遇点退 b 步、再绕 k 圈"，
//           而相遇点退 b 步正好是入环点
//       所以两人必然在**入环点**相遇。
//
//   四个坑：
//     1. **无环时要返回 nullptr**：fast 撞到 null 就退出循环，别返回 head。
//     2. **第二段 slow 回到 `head`**（不是 null、也不是相遇点）：
//        注意和 287（数组版）的区别 —— 数组版回的是 `nums[0]`，
//        因为数组的"起点节点"是下标 0 指向的那个值；链表版就是 `head` 本身。
//     3. **返回的是节点指针，不是值/下标**：值可能重复（比如 [1,1,1] 全相同），
//        只有指针身份能区分。骨架会按**指针**比对，不是比 val。
//     4. **不能修改链表**（不能拿节点当"已访问"标记），骨架会比对结构快照。
//
//   复杂度：时间 O(n)，空间 O(1)。
//
//   另一条 O(n) 空间的写法（面试可当对照）：
//       用 unordered_set<ListNode*> 记录走过的节点，第一个重复出现的节点就是入环点。
//
//   同族题：141 环形链表（判环，本题第一段）、287 寻找重复数（本题的数组版）、
//           876 链表的中间结点、146 LRU 缓存（双向链表）
class Solution
{
public:
    ListNode *detectCycle(ListNode *head)
    {
        // TODO: 在这里写你的解法

        ListNode *fast = head, *slow = head;

        while (fast != nullptr && fast->next != nullptr)
        {
            fast = fast->next->next;
            slow = slow->next;

            if (slow == fast)
            {
                slow = head;
                while (slow != fast)
                {
                    slow = slow->next;
                    fast = fast->next;
                }
                return slow;
            }
        }
        return nullptr;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 按数组建链表；pos >= 0 时把尾节点接回第 pos 个节点形成环
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

// 结构快照：按 nodes 顺序记录 (val, next 下标；-1 = nullptr，-999 = 指向表外)
// 用 nodes 顺序而非遍历，所以对成环链表也安全
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
    vector<int> vals; // 节点值
    int pos;          // 尾节点接回的下标；-1 = 无环
    int expected;     // 期望的入环点下标；-1 = 无环应返回 nullptr
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<ListNode *> nodes;
    ListNode *head = buildListWithCycle(c.vals, c.pos, nodes);
    const vector<pair<int, int>> before = snapshot(nodes);

    // 指针 -> 下标，用于把返回值翻译成人看得懂的"第几个节点"
    unordered_map<ListNode *, int> posOf;
    for (int i = 0; i < (int)nodes.size(); ++i)
        posOf[nodes[i]] = i;

    ListNode *got = nullptr;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.detectCycle(head);
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

    // ① 返回值必须是**入环点那个节点本身**（按指针比，不是按 val 比）
    ListNode *want = (c.expected >= 0) ? nodes[c.expected] : nullptr;
    bool okNode = (got == want);

    // ② 链表结构不能变
    bool intact = (before == snapshot(nodes));

    bool pass = okNode && intact;

    // 把结果翻译成下标 + 值，方便看
    auto describe = [&](ListNode *p) -> string
    {
        if (p == nullptr)
            return "nullptr";
        auto it = posOf.find(p);
        if (it == posOf.end())
            return "表外节点(?)";
        return "下标 " + to_string(it->second) + "（值 " + to_string(p->val) + "）";
    };

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  n = " << c.vals.size() << "\n"
         << "       head   = " << listToString(c.vals, c.pos) << "\n"
         << "       期望   = " << describe(want) << "\n"
         << "       实际   = " << describe(got);
    if (!okNode && got != nullptr && want != nullptr && got->val == want->val)
        cout << "\n       ✗ 值对但节点不对：返回了另一个 val 相同的节点（必须返回入环点本身）";
    else if (!okNode)
        cout << "\n       ✗ 返回的节点不是入环点";
    if (!intact)
        cout << "\n       ✗ 链表结构被改动了（不能拿节点当访问标记）";
    cout << endl;

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
        {"示例 1（环在中间）", {3, 2, 0, -4}, 1, 1},
        {"示例 2（两个节点成环）", {1, 2}, 0, 0},
        {"示例 3（无环）", {1}, -1, -1},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 空链表 / 单节点
        {"空链表", {}, -1, -1},
        {"单节点自环（pos=0）", {1}, 0, 0},

        // 无环的几种长度
        {"两个节点无环", {1, 2}, -1, -1},
        {"三个节点无环", {1, 2, 3}, -1, -1},

        // 入环点的位置：头 / 中 / 尾
        {"入环点在头部（pos=0）", {1, 2, 3}, 0, 0},
        {"入环点在中间（pos=2）", {1, 2, 3, 4, 5}, 2, 2},
        {"入环点在尾部（自环，pos=n-1）", {1, 2, 3}, 2, 2},

        // ★ 值全相同：只能靠**指针身份**区分，比 val 的写法会挂
        {"值全相同，入环点在最后一个（pos=2）", {5, 5, 5}, 2, 2},
        {"值全相同，入环点在中间（pos=1）", {7, 7, 7, 7}, 1, 1},

        // 题目数值边界 -10^5 .. 10^5
        {"取值上下限", {-100000, 100000}, 0, 0},
    };

    // 长链表用例：n = 1000（题目上限）
    {
        const int n = 1000;
        vector<int> vals(n);
        for (int i = 0; i < n; ++i)
            vals[i] = i;

        extraCases.push_back({"长链 n=1000 无环", vals, -1, -1});
        extraCases.push_back({"长链 n=1000 入环点在中间（pos=500）", vals, 500, 500});
        extraCases.push_back({"长链 n=1000 入环点在头部（pos=0）", vals, 0, 0});
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

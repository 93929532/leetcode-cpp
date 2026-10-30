#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> // 记录"每个节点原本的值"，用于校验没有偷换 val
#include <unordered_set> // 节点身份校验 + 环检测
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
// 143. 重排链表（Reorder List）· 链表 · Medium
//
// 把 L0 → L1 → … → Ln-1 → Ln 变成 L0 → Ln → L1 → Ln-1 → …
//
// 这是链表"三板斧"的第一次合体：**找中点 + 反转 + 归并**。
// 你刚在 206 练过第 2 步（三指针反转），这题把三件事串起来。
//
//   ① 找中点：快慢指针，让 slow 停在前半段的**最后一个**节点
//          ListNode* slow = head;
//          ListNode* fast = head;
//          while (fast->next != nullptr && fast->next->next != nullptr) {
//              slow = slow->next;
//              fast = fast->next->next;
//          }
//      ★ 条件是 next && next->next（不是 fast && fast->next）。
//        这样偶数长度时 slow 停在"前半段最后一个"，奇数长度时停在正中间，
//        两种情况下**前半段都 ≥ 后半段**，交替插入才好收尾。
//
//   ② 断开 + 反转后半段（用 206 的三指针）
//          ListNode* second = slow->next;
//          slow->next = nullptr;          // ★ 必须断开，否则合并时成环
//          second = reverseList(second);
//
//   ③ 交替合并两段
//          ListNode* p1 = head;
//          ListNode* p2 = second;
//          while (p2 != nullptr) {                 // ★ 以 p2 为准（后半段更短）
//              ListNode* n1 = p1->next;
//              ListNode* n2 = p2->next;
//              p1->next = p2;
//              p2->next = n1;                      // 接回前半段剩下的部分
//              p1 = n1;
//              p2 = n2;
//          }
//
//   四个坑（都实测过，骨架会分别报出来）：
//     1. **忘记 `slow->next = nullptr`** → 合并到后半段末尾时又接回前半段，形成环。
//        实测：14/15 条失败，骨架报"成环"（有步数上限，不会真的卡死）。
//     2. **`second` 取成 `slow` 而不是 `slow->next`** → 两段重叠，中点被重复插入。
//        实测：14/15 失败。
//     3. **合并循环用 `p1` 当条件**（应该用 `p2`）→ 后半段先被用完，`p2` 变成空指针，
//        下一轮 `p2->next` 直接段错误。实测：进程崩溃（0xC0000005）—— 不是"答错"而是"崩"。
//        ★ 根因：后半段永远**不长于**前半段，所以一定要让**短的那段**当循环条件。
//     4. **偷换 val 蒙过去**（把值按目标顺序填回原节点，节点顺序不变）：
//        题目明确说 *You may not modify the values in the list's nodes*。
//        实测：数值序列完全正确（[1,4,2,3]），但仍被骨架点名"有节点的 val 被改动了"。
//
//   ★ 一个容易记反的地方（我一开始就写错了）：
//        `while (fast->next && fast->next->next)` 让 slow 停在中点**偏左**；
//        `while (fast && fast->next)`             让 slow 停在中点**偏右**。
//     这两个条件**都能过**（实测各 15/15）——两种情况都满足"前半段 ≥ 后半段"，
//     配合 `while (p2)` 合并即可。所以快慢指针的循环条件不是坑，别在这里纠结。
//
//   复杂度：时间 O(n)，空间 O(1)（三步全是原地操作）。
//
//   另一种写法（面试可以当对照说）：
//      把所有节点指针存进 vector，按 0, n-1, 1, n-2 … 的顺序重连 ——
//      O(n) 空间但代码更短，不容易错；代价是不满足 O(1) 空间。
//
//   同族题：206 反转链表（第 ② 步）、876 链表的中间结点（第 ① 步）、
//           21 合并两个有序链表（第 ③ 步的有序版）、234 回文链表（中点 + 反转半段 + 比较）
class Solution
{
public:
    void reorderList(ListNode *head)
    {
        // TODO: 在这里写你的解法

        if (head->next == nullptr || head->next->next == nullptr)
            return;

        ListNode *slow = head, *fast = head->next;
        while (fast->next != nullptr)
        {
            slow = slow->next;

            fast = fast->next;
            if (fast->next == nullptr)
                continue;
            fast = fast->next;
        }

        ListNode *prev = nullptr, *curr = slow->next;
        while (curr != nullptr)
        {
            ListNode *nxt = curr->next;
            curr->next = prev;
            prev = curr;
            curr = nxt;
        }

        slow->next = nullptr;
        ListNode *L1 = head, *L2 = prev;

        while (L1 != nullptr || L2 != nullptr)
        {
            if (L1 != nullptr)
            {
                ListNode *nxt = L1->next;
                L1->next = L2;
                L1 = nxt;
            }

            if (L2 != nullptr)
            {
                ListNode *nxt = L2->next;
                L2->next = L1;
                L2 = nxt;
            }
        }

        return;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 由数组构建链表，返回头节点
static ListNode *buildList(const vector<int> &nums)
{
    ListNode dummy; // 虚拟头节点，简化插入逻辑
    ListNode *cur = &dummy;
    for (int v : nums)
    {
        cur->next = new ListNode(v);
        cur = cur->next;
    }
    return dummy.next;
}

// 把数组格式化成 [1,2,3]；超过 12 个只打印首 6 末 3
static string vectorToStr(const vector<int> &v)
{
    string s = "[";
    if (v.size() <= 12)
    {
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (i)
                s += ",";
            s += to_string(v[i]);
        }
        return s + "]";
    }
    for (int i = 0; i < 6; ++i)
    {
        if (i)
            s += ",";
        s += to_string(v[i]);
    }
    s += ", …(略去 " + to_string(v.size() - 9) + " 个)…, ";
    for (size_t i = v.size() - 3; i < v.size(); ++i)
    {
        if (i != v.size() - 3)
            s += ",";
        s += to_string(v[i]);
    }
    return s + "]";
}

// 单条测试用例
//
// 本题除了比对数值序列，还校验两件事（见 runCase）：
//   ① 节点身份：没有新建节点、没有成环
//   ② 值没被篡改：每个节点仍带着自己原来的 val（题目禁止改值）
struct TestCase
{
    string name;          // 用例名称
    vector<int> head;     // 输入链表（用数组表示）
    vector<int> expected; // 期望输出（用数组表示）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    ListNode *head = buildList(c.head);

    // 记下"节点 -> 原本的值"，同时收集所有原节点用于事后释放
    unordered_map<ListNode *, int> origVal;
    for (ListNode *p = head; p != nullptr; p = p->next)
        origVal[p] = p->val;

    // 采集结果链表：带步数上限，防止错误解法成环后把骨架卡死
    const int cap = (int)c.head.size() * 2 + 10;
    vector<int> gotVec;
    bool truncated = false;
    bool hasNewNode = false;
    bool hasCycle = false;

    try
    {
        Solution sol;
        sol.reorderList(head);

        unordered_set<ListNode *> seen;
        for (ListNode *p = head; p != nullptr; p = p->next)
        {
            if ((int)gotVec.size() >= cap)
            {
                truncated = true;
                break;
            }
            if (!seen.insert(p).second) // 同一个节点出现两次 -> 成环
            {
                hasCycle = true;
                break;
            }
            if (origVal.find(p) == origVal.end()) // 出现了原链表里没有的节点
            {
                hasNewNode = true;
                break;
            }
            gotVec.push_back(p->val);
        }
    }
    catch (const exception &e)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << e.what() << endl;
        for (auto &kv : origVal)
            delete kv.first;
        return false;
    }

    // 校验"值没被篡改"：每个节点是否还带着自己原来的 val
    bool valTampered = false;
    {
        unordered_set<ListNode *> seen2;
        for (ListNode *p = head; p != nullptr; p = p->next)
        {
            if (!seen2.insert(p).second)
                break; // 成环，上面已经报告过
            auto it = origVal.find(p);
            if (it != origVal.end() && p->val != it->second)
            {
                valTampered = true;
                break;
            }
        }
    }

    string got = vectorToStr(gotVec);
    string want = vectorToStr(c.expected);

    bool pass = (gotVec == c.expected) && !hasCycle && !hasNewNode && !truncated && !valTampered;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       head = " << vectorToStr(c.head) << "\n"
         << "       期望: " << want << "\n"
         << "       实际: " << got;
    if (truncated)
        cout << "    ← 节点数超过上限，疑似成环";
    else if (hasCycle)
        cout << "    ← 链表成环（多半是忘了 slow->next = nullptr）";
    else if (hasNewNode)
        cout << "    ← 出现原链表里没有的新节点（题目要求原地重连）";
    else if (valTampered)
        cout << "    ← 有节点的 val 被改动了（题目禁止修改节点的值，必须重新接线）";
    cout << endl;

    // 释放：先释放原节点，再释放"新增"的节点（正常解法不会有后者）
    for (auto &kv : origVal)
        delete kv.first;
    if (hasNewNode)
    {
        unordered_set<ListNode *> seen3;
        for (ListNode *p = head; p != nullptr && seen3.insert(p).second; p = p->next)
            if (origVal.find(p) == origVal.end())
                delete p;
    }
    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 2, 3, 4}, {1, 4, 2, 3}},
        {"示例 2", {1, 2, 3, 4, 5}, {1, 5, 2, 4, 3}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 长度 1 / 2 / 3：最短的几种形态
        {"单节点（保持原样）", {1}, {1}},
        {"两个节点", {1, 2}, {1, 2}},
        {"三个节点", {1, 2, 3}, {1, 3, 2}},

        // 奇偶长度的标准形态
        {"四个节点", {1, 2, 3, 4}, {1, 4, 2, 3}},
        {"五个节点", {1, 2, 3, 4, 5}, {1, 5, 2, 4, 3}},
        {"六个节点（偶数）", {1, 2, 3, 4, 5, 6}, {1, 6, 2, 5, 3, 4}},
        {"七个节点（奇数）", {1, 2, 3, 4, 5, 6, 7}, {1, 7, 2, 6, 3, 5, 4}},
        {"十个节点", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {1, 10, 2, 9, 3, 8, 4, 7, 5, 6}},

        // 数值上的边界
        {"含负数", {-1, -2, -3, -4}, {-1, -4, -2, -3}},
        {"取值上下限", {5000, -5000, 0, -1}, {5000, -1, -5000, 0}},

        // 全部相同值：光看数值序列看不出结构对不对，靠身份/值校验兜底
        {"全部相同值（值序列看不出差别）", {7, 7, 7, 7}, {7, 7, 7, 7}},

        // 已经是"重排后"的顺序，再排一次应该得到另一个结果（防止实现只对某种形态正确）
        {"已经是目标顺序，再重排一次", {1, 4, 2, 3}, {1, 3, 4, 2}},
    };

    // 最大规模：n = 50000（题目上限）
    //   期望序列：1, n, 2, n-1, 3, n-2 …
    {
        const int n = 50000;
        vector<int> big(n), expect;
        for (int i = 0; i < n; ++i)
            big[i] = i + 1;
        int i = 0, j = n - 1;
        while (i <= j)
        {
            expect.push_back(big[i]);
            if (i != j)
                expect.push_back(big[j]);
            ++i;
            --j;
        }
        extraCases.push_back({"最大规模 n=50000", big, expect});
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

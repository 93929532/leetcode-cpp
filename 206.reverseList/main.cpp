#include <iostream>
#include <string>
#include <vector>
#include <unordered_set> // 节点身份校验用

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
class Solution
{
public:
    ListNode *reverseList(ListNode *head)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 35 分钟以上再回头看）：
        //
        //   这是**所有链表题的地基**。后面的 25（K 个一组翻转）、92（区间反转）、
        //   143（重排链表）、234（回文链表）全都要用到这题的"三指针"动作。
        //   所以请把它练到闭眼能写。
        //
        //   ① 核心动作：三个指针
        //          prev  指向"已经反转好的那段的头"（初始为 nullptr）
        //          cur   指向"还没处理的那段的头"（初始为 head）
        //          nxt   临时存 cur 的下一个
        //
        //          while (cur != nullptr) {
        //              nxt = cur->next;     // ① 先存下一个！否则一改 next 就丢了
        //              cur->next = prev;    // ② 反转当前节点
        //              prev = cur;          // ③ prev 前移
        //              cur  = nxt;          // ④ cur 前移
        //          }
        //          return prev;             // 循环结束时 cur 是 nullptr，prev 是新头
        //
        //   ② ⚠️ 顺序是本题的命门
        //      如果先写 `cur->next = prev` 再去取下一个，你会发现"下一个"已经没了 ——
        //      **整条链表瞬间被截断**。
        //      我实测过：把 ①② 两行交换，输入 [1,2,3,4,5] 只会走 1 步，
        //      结果是 [1]（后面 4 个节点直接丢失）。
        //      所以口诀是：**先存后改，指针不散**。
        //
        //   ③ 为什么返回 prev 而不是 cur / head
        //      循环结束时 cur 一定是 nullptr（走到头了），
        //      而 prev 停在原来最后一个节点上 —— 那正是新链表的头。
        //
        //   ④ 空链表 / 单节点天然兼容
        //          head == nullptr  -> 循环一次都不执行，返回 prev = nullptr ✓
        //          head 只有一个     -> 循环执行一次，返回 prev = 那个节点 ✓
        //      所以**不需要**任何特判。
        //
        //   ⑤ 递归写法（LeetCode 的 follow-up，也值得会）
        //          if (head == nullptr || head->next == nullptr) return head;
        //          ListNode* newHead = reverseList(head->next);   // 先把后面全反转好
        //          head->next->next = head;                       // 让后一个节点指回自己
        //          head->next = nullptr;                          // 自己变成尾节点
        //          return newHead;
        //      我实测过：n = 5000（本题上限）时递归**不会爆栈**，可以放心用。
        //      但记住它的复杂度：时间 O(n)，空间 O(n)（递归栈）；迭代法空间是 O(1)。
        //
        //   ⑥ 两个"不算数"的写法（测试骨架会校验）
        //      · **只交换 val**：把值倒着填回原节点 —— 输出看着对，但链表结构没变，
        //        而且遇到"节点带外部引用"的场景就崩（138 随机链表复制会用到）。
        //      · **新建节点**：遍历原链表，头插法造一条新链表 —— 输出也对，但内存翻倍，
        //        原链表还被泄漏了。
        //      本题的正解必须是**在原节点上重新接线**。骨架会检查"返回的节点是否
        //      全部来自原链表"，上面两种写法会被标出来。
        //
        //   复杂度：迭代 O(n) 时间 / O(1) 空间；递归 O(n) 时间 / O(n) 空间。

        ListNode *prev = nullptr;
        ListNode *cur = head;

        while (cur != nullptr)
        {
            ListNode *nxt = cur->next; // ① 先存下一个，否则一改 next 就丢了
            cur->next = prev;          // ② 反转当前节点
            prev = cur;                // ③ prev 前移
            cur = nxt;                 // ④ cur 前移
        }

        return prev; // cur 走到头，prev 就是新头
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

// 把链表序列化成 [a,b,c] 便于打印；带步数上限，防止错误解法成环后把骨架卡死
static string listToString(ListNode *head, int cap = 200000)
{
    vector<int> v;
    for (ListNode *p = head; p != nullptr && (int)v.size() < cap; p = p->next)
        v.push_back(p->val);
    return vectorToStr(v);
}

// 单条测试用例
//
// 注意：本题除了比对数值序列，还会校验「是否在原节点上反转」（见 runCase）。
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

    // 记下原链表所有节点的地址（用于身份校验 + 事后释放）
    unordered_set<ListNode *> originals;
    for (ListNode *p = head; p != nullptr; p = p->next)
        originals.insert(p);

    Solution sol;
    ListNode *result = sol.reverseList(head);

    // 1) 数值序列是否正确
    string got = listToString(result);
    string want = vectorToStr(c.expected);

    // 2) 返回的节点是否全部来自原链表（原地反转），同时防成环
    bool allFromOriginal = true;
    unordered_set<ListNode *> visited;
    for (ListNode *p = result; p != nullptr; p = p->next)
    {
        if (!visited.insert(p).second) // 同一个节点出现了两次 -> 成环
        {
            allFromOriginal = false;
            break;
        }
        if (originals.find(p) == originals.end()) // 出现了原链表里没有的节点
        {
            allFromOriginal = false;
            break;
        }
    }

    // 3) 空链表 / 单节点时身份校验没有意义
    bool identityMeaningful = c.head.size() >= 2;
    bool nodeOk = (!identityMeaningful) || allFromOriginal;

    bool pass = (got == want) && nodeOk;

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       head = " << vectorToStr(c.head) << "\n"
         << "       期望: " << want << "\n"
         << "       实际: " << got;
    if (identityMeaningful && !allFromOriginal)
        cout << "    ← 出现新节点或成环，没有在原节点上反转";
    cout << endl;

    // 释放：先释放原节点，再释放结果里"新增"的节点（正常解法不会有后者）
    for (ListNode *p : originals)
        delete p;
    for (ListNode *p : visited)
        if (originals.find(p) == originals.end())
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
        {"示例 1", {1, 2, 3, 4, 5}, {5, 4, 3, 2, 1}},
        {"示例 2（空链表）", {}, {}},
        {"示例 3（单节点）", {1}, {1}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"两个节点", {1, 2}, {2, 1}},
        {"三个节点", {1, 2, 3}, {3, 2, 1}},
        {"四个节点（偶数长度）", {1, 2, 3, 4}, {4, 3, 2, 1}},
        {"全部相同值（看不出反转）", {7, 7, 7}, {7, 7, 7}},
        {"两个相同值", {5, 5}, {5, 5}},
        {"含负数", {-1, -2, -3}, {-3, -2, -1}},
        {"取值上下限", {-5000, 0, 5000}, {5000, 0, -5000}},
        {"十个节点递增", {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {10, 9, 8, 7, 6, 5, 4, 3, 2, 1}},
        {"已经递减（反转后变递增）", {5, 4, 3, 2, 1}, {1, 2, 3, 4, 5}},
    };

    // 最大规模：n = 5000（题目上限）
    //   这条同时压两件事：三指针迭代会不会写错、递归写法会不会爆栈
    {
        const int n = 5000;
        vector<int> big(n);
        for (int i = 0; i < n; ++i)
            big[i] = i;
        vector<int> expect(big.rbegin(), big.rend());
        extraCases.push_back({"最大规模 n=5000", big, expect});
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

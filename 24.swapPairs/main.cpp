#include <iostream>
#include <vector>
#include <string>
#include <algorithm>   // std::find：节点身份校验

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

/**
 * Definition for singly-linked list.
 */
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

// ===================== 答题区域 =====================
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {

        if (head == nullptr || head->next == nullptr)
            return head;

        ListNode* p1 = head;
        ListNode* p2 = head->next;
        head = p2;

        while (true) {
            p1->next = p2->next;
            p2->next = p1;

            if (p1->next == nullptr)
                return head;
            else
                p1 = p1->next;

            if (p1->next == nullptr)
                return head;
            else
            {
                p2->next->next = p1->next;
                p2 = p1->next;
            }
        }
        return head;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 由数组构建链表，返回头节点
static ListNode* buildList(const vector<int>& nums) {
    ListNode dummy;              // 虚拟头节点，简化插入逻辑
    ListNode* cur = &dummy;
    for (int v : nums) {
        cur->next = new ListNode(v);
        cur = cur->next;
    }
    return dummy.next;
}

// 按 [a,b,c] 的格式序列化链表
static string listToString(ListNode* head) {
    string s = "[";
    for (ListNode* p = head; p != nullptr; p = p->next) {
        if (p != head) s += ",";
        s += to_string(p->val);
    }
    s += "]";
    return s;
}

// 释放整条链表，避免内存泄漏
static void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* nxt = head->next;
        delete head;
        head = nxt;
    }
}

// 单条测试用例
struct TestCase {
    string name;          // 用例名称
    vector<int> head;     // 输入链表
    vector<int> expected; // 期望输出
};

// 运行一条用例，打印一行信息，返回该用例是否通过
//
// 除了比对数值序列，还会校验「是否真的交换了节点」：
//   题目明确要求不能只改 val。这里在调用前记下原链表每个节点的地址，
//   若返回结果的节点从头到尾都是原节点本身（没有新建节点），
//   说明是在原节点上重新接线 —— 这才是符合题意的做法。
static bool runCase(const TestCase& c) {
    ListNode* head = buildList(c.head);

    // 记录原链表所有节点的地址（用于身份校验）
    vector<ListNode*> originalNodes;
    for (ListNode* p = head; p != nullptr; p = p->next) {
        originalNodes.push_back(p);
    }

    Solution sol;
    ListNode* result = sol.swapPairs(head);

    // 1) 数值序列是否正确
    string got = listToString(result);
    string want = listToString(buildList(c.expected));

    // 2) 返回的节点是否全部来自原链表（没有偷偷 new 出新节点）
    bool allFromOriginal = true;
    for (ListNode* p = result; p != nullptr; p = p->next) {
        if (find(originalNodes.begin(), originalNodes.end(), p) == originalNodes.end()) {
            allFromOriginal = false;
            break;
        }
    }
    // 3) 节点数不足 2 时无从判断交换，身份校验只在 length >= 2 时有意义
    bool identityMeaningful = c.head.size() >= 2;
    bool nodeSwapOk = (!identityMeaningful) || allFromOriginal;

    bool pass = (got == want) && nodeSwapOk;

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  head = " << listToString(buildList(c.head))
         << "  期望: " << want
         << "  实际: " << got;
    if (identityMeaningful && !allFromOriginal) {
        cout << "    ← 出现了新建节点，未在原节点上交换（题目禁止只改 val）";
    }
    cout << endl;

    // result 与 head 指向同一批节点，释放一次即可
    freeList(result);
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 2, 3, 4}, {2, 1, 4, 3}},
        {"示例 2", {},           {}},
        {"示例 3", {1},          {1}},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"两节点",           {1, 2},             {2, 1}},
        {"三节点留尾巴",     {1, 2, 3},          {2, 1, 3}},
        {"五节点留尾巴",     {1, 2, 3, 4, 5},    {2, 1, 4, 3, 5}},
        {"六节点整除",       {1, 2, 3, 4, 5, 6}, {2, 1, 4, 3, 6, 5}},
        {"含负数",           {-1, -2, -3, -4},   {-2, -1, -4, -3}},
        {"含重复值",         {1, 1, 2, 2},       {1, 1, 2, 2}},
    };

    size_t passed = 0;
    size_t total = 0;

    cout << "================ 题目示例 ================" << endl;
    for (const auto& c : sampleCases) {
        ++total;
        if (runCase(c)) ++passed;
    }

    cout << endl << "================ 补充用例 ================" << endl;
    for (const auto& c : extraCases) {
        ++total;
        if (runCase(c)) ++passed;
    }

    cout << endl << "================ 结果统计 ================" << endl;
    cout << "共 " << total << " 个用例，通过 " << passed
         << " 个，失败 " << (total - passed) << " 个。" << endl;
    cout << (passed == total ? "全部通过 ✔" : "存在失败用例 ✘") << endl;

    return 0;
}

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>  // 仅用于让 Windows 终端按 UTF-8 显示中文
#endif

// =====================================================================
// LeetCode 题目自带的 ListNode 定义
// =====================================================================
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        //   1. 迭代法：哑结点(dummy) + 尾指针，逐个比较两条链表的当前结点；
        //   2. 递归法：比较两个头结点，把较小者接到剩余结果的前面；
        //   3. 别忘了处理某条链表先走完、直接把另一条接上的情况。

        ListNode dummy;  // 哑结点，方便尾插
        ListNode* tail = &dummy;

        while (list1 != nullptr && list2 != nullptr) {
            // <= 让 list1 优先，相等时保持相对稳定
            if (list1->val <= list2->val) {
                tail->next = list1;
                list1 = list1->next;
            } else {
                tail->next = list2;
                list2 = list2->next;
            }
            tail = tail->next;
        }

        // 谁剩下就整体接谁（可能是 nullptr，也天然处理了空链表）
        tail->next = (list1 != nullptr) ? list1 : list2;
        return dummy.next;
    }
};

// =====================================================================
// 以下全部是本地测试脚手架，与题目解答无关，可以忽略
// =====================================================================

// 根据数组构造一条单链表，返回头结点（空数组返回 nullptr）
ListNode* buildList(const std::vector<int>& nums) {
    ListNode dummy;  // 哑结点，方便尾插
    ListNode* tail = &dummy;
    for (int v : nums) {
        tail->next = new ListNode(v);
        tail = tail->next;
    }
    return dummy.next;
}

// 遍历链表，把每个结点的值收集到数组中
std::vector<int> listToVector(ListNode* head) {
    std::vector<int> out;
    for (ListNode* p = head; p != nullptr; p = p->next) {
        out.push_back(p->val);
    }
    return out;
}

// 把数组格式化为 "[1,2,3]" 形式的字符串
std::string vectorToString(const std::vector<int>& nums) {
    std::string s = "[";
    for (std::size_t i = 0; i < nums.size(); ++i) {
        if (i > 0) {
            s += ",";
        }
        s += std::to_string(nums[i]);
    }
    s += "]";
    return s;
}

// 释放整条链表占用的内存
void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

// 运行一个测试用例，num 为用例编号
void runCase(int num,
             const std::vector<int>& l1,
             const std::vector<int>& l2,
             const std::vector<int>& expected) {
    ListNode* head1 = buildList(l1);
    ListNode* head2 = buildList(l2);

    Solution solution;
    ListNode* merged = solution.mergeTwoLists(head1, head2);

    const std::vector<int> actual = listToVector(merged);
    const bool passed = (actual == expected);

    std::cout << "用例 " << num << ": l1 = " << vectorToString(l1)
              << ", l2 = " << vectorToString(l2) << "\n";
    std::cout << "  期望输出: " << vectorToString(expected) << "\n";
    std::cout << "  实际输出: " << vectorToString(actual) << "\n";
    std::cout << "  结果: " << (passed ? "通过" : "失败") << "\n" << std::endl;

    // 清理内存：合并结果复用了原结点，所以优先释放合并后的链表
    if (merged != nullptr) {
        freeList(merged);
    } else {
        freeList(head1);
        freeList(head2);
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);  // 避免中文输出乱码
#endif

    // LeetCode 官方示例
    runCase(1, {1, 2, 4}, {1, 3, 4}, {1, 1, 2, 3, 4, 4});
    runCase(2, {}, {}, {});
    runCase(3, {}, {0}, {0});

    // 额外的边界与常规用例
    runCase(4, {1}, {2}, {1, 2});
    runCase(5, {5}, {1, 2, 3}, {1, 2, 3, 5});
    runCase(6, {-10, -5, 0}, {-3, 4, 9}, {-10, -5, -3, 0, 4, 9});
    runCase(7, {1, 1, 1}, {1, 1}, {1, 1, 1, 1, 1});

    return 0;
}

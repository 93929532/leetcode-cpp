#include <iostream>
#include <vector>
#include <string>

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
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode* l3 = new ListNode(0, nullptr); // 创建一个虚拟头节点
        ListNode* head = l3; // 保存头节点，方便返回结果

        int carry = 0; // 进位标志

        while(l1 != nullptr || l2 != nullptr || carry != 0)
        {
            int l1_val = (l1 != nullptr) ? l1->val : 0;
            int l2_val = (l2 != nullptr) ? l2->val : 0;
            int sum = l1_val + l2_val + carry;

            carry = sum / 10; // 更新进位
            
            l3->next = new ListNode(sum % 10); // 创建新节点存储当前位的和
            l3 = l3->next; // 移动到新创建的节点
            
            if(l1 != nullptr) l1 = l1->next; // 移动到下一个节点
            if(l2 != nullptr) l2 = l2->next;
            
        }
        return head->next; // 返回结果链表的头节点（跳过虚拟头节点）
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 根据 vector 构造链表
ListNode* createList(const std::vector<int>& nums) {
    ListNode dummy(0);
    ListNode* cur = &dummy;
    for (int v : nums) {
        cur->next = new ListNode(v);
        cur = cur->next;
    }
    return dummy.next;
}

// 打印链表
void printList(ListNode* head) {
    while (head) {
        std::cout << head->val;
        if (head->next) std::cout << " -> ";
        head = head->next;
    }
    std::cout << std::endl;
}

// 释放链表内存
void freeList(ListNode* head) {
    while (head) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

// 把链表转成字符串，便于比较结果
std::string listToString(ListNode* head) {
    std::string s;
    while (head) {
        s += std::to_string(head->val);
        head = head->next;
    }
    return s;
}

int main() {
    Solution sol;

    struct TestCase {
        std::vector<int> l1;
        std::vector<int> l2;
        std::vector<int> expected;
    };

    std::vector<TestCase> cases = {
        {{2, 4, 3},              {5, 6, 4},              {7, 0, 8}},
        {{0},                    {0},                    {0}},
        {{9, 9, 9, 9, 9, 9, 9}, {9, 9, 9, 9},           {8, 9, 9, 9, 0, 0, 0, 1}},
    };

    for (size_t i = 0; i < cases.size(); ++i) {
        ListNode* a = createList(cases[i].l1);
        ListNode* b = createList(cases[i].l2);
        ListNode* result = sol.addTwoNumbers(a, b);

        std::cout << "示例 " << (i + 1) << ":" << std::endl;
        std::cout << "  l1 = ";
        printList(a);
        std::cout << "  l2 = ";
        printList(b);
        std::cout << "  sum = ";
        printList(result);

        // 校验结果
        ListNode* expect = createList(cases[i].expected);
        if (listToString(expect) == listToString(result)) {
            std::cout << "  [通过]" << std::endl;
        } else {
            std::cout << "  [失败] 期望: " << listToString(expect) << std::endl;
        }

        freeList(a);
        freeList(b);
        freeList(result);
        freeList(expect);
    }

    return 0;
}

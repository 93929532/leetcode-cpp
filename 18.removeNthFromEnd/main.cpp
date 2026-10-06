#include <iostream>
#include <string>
#include <vector>

/**
 * Definition for singly-linked list.
 * 力扣 19. 删除链表的倒数第 N 个结点
 * https://leetcode.cn/problems/remove-nth-node-from-end-of-list/
 */
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        // 虚拟头结点：让"删除头结点"和"删除中间结点"走同一套逻辑
        ListNode dummy(0, head);
        ListNode* fast = &dummy;
        ListNode* slow = &dummy;

        for(int i = 0; i <= n; i++){
            fast = fast->next;
        }
        
        while(fast != nullptr){
            fast = fast->next;
            slow = slow->next;
        }

        slow->next = slow->next->next;

        return dummy.next;
    }
};

/* ==================== 以下为本地测试辅助代码，非答题区域 ==================== */

// 根据数组构建单链表，返回头结点
ListNode* buildList(const std::vector<int>& nums) {
    ListNode dummy;                // 虚拟头结点，简化边界处理
    ListNode* tail = &dummy;
    for (int x : nums) {
        tail->next = new ListNode(x);
        tail = tail->next;
    }
    return dummy.next;
}

// 将链表序列化成像 LeetCode 那样的字符串，方便打印对比
std::string listToString(ListNode* head) {
    std::string s = "[";
    for (ListNode* p = head; p != nullptr; p = p->next) {
        s += std::to_string(p->val);
        if (p->next != nullptr) s += ",";
    }
    return s + "]";
}

// 释放整条链表，避免内存泄漏
void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* nxt = head->next;
        delete head;
        head = nxt;
    }
}

// 运行并打印单个测试用例的结果
void runCase(const std::vector<int>& nums, int n, const std::string& expected) {
    ListNode* head = buildList(nums);
    std::cout << "输入: head = " << listToString(head) << ", n = " << n << std::endl;

    head = Solution().removeNthFromEnd(head, n);   // 调用答题区实现
    std::string actual = listToString(head);

    std::cout << "输出: " << actual << std::endl;
    std::cout << "期望: " << expected
              << "  -> " << (actual == expected ? "通过" : "失败") << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    freeList(head);
}

int main() {
    runCase({1, 2, 3, 4, 5}, 2, "[1,2,3,5]");  // 示例 1
    runCase({1}, 1, "[]");                     // 示例 2
    runCase({1, 2}, 1, "[1]");                 // 示例 3
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>

#ifdef _WIN32
#include <windows.h>  // SetConsoleOutputCP:解决中文输出乱码
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

class Solution {
public:

    // ===================== 答题区域开始 =====================
    // 题目:25. K 个一组翻转链表(困难)
    //
    // 给你链表的头节点 head,每 k 个节点一组进行翻转,请你返回修改后的链表。
    // k 是一个正整数,它的值小于或等于链表的长度。如果节点总数不是 k 的整数倍,
    // 那么请将最后剩余的节点保持原有顺序。
    // 你不能只是单纯地改变节点内部的值,而是需要实际进行节点交换。
    //
    // 示例 1: head = [1,2,3,4,5], k = 2  ->  [2,1,4,3,5]
    // 示例 2: head = [1,2,3,4,5], k = 3  ->  [3,2,1,4,5]
    //
    // 复杂度要求: 时间 O(n),额外空间 O(1)
    ListNode* reverseKGroup(ListNode* head, int k) {

        ListNode dummy(0, head);
        ListNode* groupPrev = &dummy;
        ListNode* groupEnd = head;

        for(int i = 0; i < k; i++) {
            if(groupEnd == nullptr) return dummy.next;
            groupEnd = groupEnd->next;
        }

        while(true) {
            ListNode* prev = groupEnd;
            ListNode* cur = groupPrev->next;

            while(cur != groupEnd) {
                ListNode* nxt = cur->next;
                cur->next = prev;
                prev = cur;
                cur = nxt;
            }

            ListNode* groupStart = groupPrev->next;
            groupPrev->next = prev;
            groupPrev = groupStart;

            for(int i = 0; i < k; i++) {
                if(groupEnd == nullptr) return dummy.next;
                groupEnd = groupEnd->next;
            }
        }      
    }
};

// ===================== 答题区域结束 =====================
// ===================== 以下为测试框架,无需修改 =====================

// 由数组构建链表,返回头节点
ListNode* buildList(const vector<int>& nums) {
    ListNode dummy;              // 虚拟头节点,简化插入逻辑
    ListNode* cur = &dummy;
    for (int v : nums) {
        cur->next = new ListNode(v);
        cur = cur->next;
    }
    return dummy.next;
}

// 按 [a,b,c] 的格式打印链表
void printList(ListNode* head) {
    cout << "[";
    for (ListNode* p = head; p != nullptr; p = p->next) {
        if (p != head) cout << ",";
        cout << p->val;
    }
    cout << "]" << endl;
}

// 释放整条链表,避免内存泄漏
void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* nxt = head->next;
        delete head;
        head = nxt;
    }
}

// 从输入流读取一行整数(空格分隔)作为链表节点值
bool readLine(vector<int>& nums) {
    nums.clear();
    string line;
    if (!getline(cin, line)) return false;   // 读到 EOF,结束
    istringstream iss(line);
    int v;
    while (iss >> v) nums.push_back(v);
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    cout << "输入格式:" << endl;
    cout << "  第 1 行:链表节点值(空格分隔,如 1 2 3 4 5)" << endl;
    cout << "  第 2 行:k 的值" << endl;
    cout << "可重复输入多组数据,EOF(Ctrl+Z 回车)结束。" << endl;

    vector<int> nums;
    int k = 0;
    while (readLine(nums)) {                 // 第 1 行:链表值
        if (nums.empty()) continue;
        if (!(cin >> k)) break;              // 第 2 行:k
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // 吃掉行尾换行

        ListNode* head = buildList(nums);
        Solution sol;
        ListNode* res = sol.reverseKGroup(head, k);

        cout << "输入: head = [";
        for (size_t i = 0; i < nums.size(); ++i) {
            if (i) cout << ",";
            cout << nums[i];
        }
        cout << "], k = " << k << endl;
        cout << "输出: ";
        printList(res);

        freeList(res);                       // res 含全部原始节点,释放一次即可
    }

    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
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
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        
        ListNode dummy;
        ListNode* tail = &dummy;
        
        while(true){
            int mi = -1;
            for(int i = 0; i< lists.size(); i++){
                if(lists[i] == nullptr) continue;
                if(mi == -1 || lists[i]->val < lists[mi]->val) mi = i;
            }

            if(mi == -1) break;
            tail->next = lists[mi];
            tail = tail->next;
            lists[mi] = lists[mi]->next;
        }

        return dummy.next;
    }
};
/* ------------------------- 测试脚手架 ------------------------- */

// 由数组构建链表（空数组返回 nullptr）
static ListNode* buildList(const vector<int>& nums) {
    ListNode dummy;
    ListNode* cur = &dummy;
    for (int v : nums) {
        cur->next = new ListNode(v);
        cur = cur->next;
    }
    return dummy.next;
}

// 将链表序列化为 "[1,2,3]" 形式
static string listToString(ListNode* head) {
    string s = "[";
    for (ListNode* p = head; p != nullptr; p = p->next) {
        if (p != head) s += ",";
        s += to_string(p->val);
    }
    s += "]";
    return s;
}

// 解析形如 [[1,4,5],[1,3,4],[2,6]] 的输入为链表数组
// 支持空输入 []（零个链表）与 [[]]（一个空链表）
static vector<ListNode*> parseInput(const string& raw) {
    // 去除所有空白字符
    string s;
    for (char c : raw) {
        if (!isspace(static_cast<unsigned char>(c))) s += c;
    }

    vector<ListNode*> lists;
    size_t i = 0;

    // 跳过最外层的 '['
    if (i < s.size() && s[i] == '[') ++i;

    while (i < s.size()) {
        char c = s[i];
        if (c == '[') {
            // 解析一个链表：读取 '[' 到 ']' 之间的整数
            vector<int> nums;
            string num;
            ++i;
            while (i < s.size() && s[i] != ']') {
                if (s[i] == ',') {
                    if (!num.empty()) { nums.push_back(stoi(num)); num.clear(); }
                } else {
                    num += s[i];
                }
                ++i;
            }
            if (!num.empty()) { nums.push_back(stoi(num)); }
            if (i < s.size() && s[i] == ']') ++i; // 跳过 ']'
            lists.push_back(buildList(nums));
        } else if (c == ',') {
            ++i;
        } else if (c == ']') {
            break; // 到达最外层结尾
        } else {
            ++i;
        }
    }
    return lists;
}

/* ---------------------- 内置测试用例 ---------------------- */
/*  固定用例，运行 exe（或 F5）即全部跑一遍，无需任何输入。
 *  想加用例就在 kTestCases 里加一行：输入格式同「[[1,4,5],[1,3,4]]」。
 */
struct TestCase {
    const char* name;     // 用例名称
    const char* input;    // 输入，形如 [[1,4,5],[1,3,4],[2,6]]
    const char* expected; // 期望输出
};

static const TestCase kTestCases[] = {
    // ---- 题目给出的 3 个样例 ----
    {"样例1",      "[[1,4,5],[1,3,4],[2,6]]", "[1,1,2,3,4,4,5,6]"},
    {"样例2",      "[]",                      "[]"},
    {"样例3",      "[[]]",                    "[]"},
    // ---- 边界用例，用于暴露常见 bug ----
    {"单个链表",    "[[1]]",                   "[1]"},
    {"混合空链表",  "[[],[1],[]]",             "[1]"},
    {"前导空链表",  "[[],[2,3],[1]]",          "[1,2,3]"},
    {"含负数",      "[[-3,-2,-1],[-5,0]]",     "[-5,-3,-2,-1,0]"},
    {"重复值",      "[[1,1,1],[1,1]]",         "[1,1,1,1,1]"},
    {"单元素多链",  "[[5],[3],[1]]",           "[1,3,5]"},
};

// 执行一条用例，返回是否通过
static bool runCase(const TestCase& tc) {
    vector<ListNode*> lists = parseInput(tc.input);

    Solution sol;
    ListNode* result = sol.mergeKLists(lists);

    string got = listToString(result);
    bool ok = (got == tc.expected);

    cout << (ok ? "[PASS] " : "[FAIL] ") << tc.name << "\n"
         << "       输入: " << tc.input << "\n"
         << "       期望: " << tc.expected << "\n"
         << "       实际: " << got << endl;

    // 只释放返回值，避免与 lists 中的节点重复释放
    for (ListNode* p = result; p != nullptr; ) {
        ListNode* next = p->next;
        delete p;
        p = next;
    }
    return ok;
}

// 跑完全部内置用例，返回失败个数
static int runBuiltinTests() {
    const int n = static_cast<int>(sizeof(kTestCases) / sizeof(kTestCases[0]));

    cout << "===== 内置测试用例（共 " << n << " 条）=====" << endl;
    int passed = 0;
    for (int i = 0; i < n; ++i) {
        if (runCase(kTestCases[i])) ++passed;
    }
    cout << "===== 通过 " << passed << "/" << n << " =====" << endl;
    if (passed != n) {
        cout << "提示：若某条用例没有打印 [FAIL] 就中断了，说明实现发生了崩溃" << endl;
    }
    return n - passed;
}

int main() {
#ifdef _WIN32
    // 修复 Windows 控制台中文乱码
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 直接跑内置用例，不读任何输入：
    //   · 按 F5 调试或直接运行 exe 都不会卡在等输入上
    //   · 想看真实样例之外的数据，改下面 kTestCases 里的一行即可
    return runBuiltinTests() == 0 ? 0 : 1;
}
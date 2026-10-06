#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    bool isAnagram(string s, string t) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   思路一 · 定长计数数组（推荐，O(n) 时间 / O(1) 空间）
        //     题目限定「只含小写字母」，字母表大小固定是 26，
        //     所以可以用 array<int, 26> cnt{}; 代替哈希表，
        //     下标 = c - 'a'，比 unordered_map 更快也更省。
        //     做法：先判断长度，不等直接 return false；
        //           遍历 s 对 cnt 做 +1，遍历 t 对 cnt 做 -1；
        //           最后看 cnt 是否全为 0（全 0 说明两边字母用量完全一致）。
        //
        //   思路二 · 排序后逐位比较（O(n log n)）
        //     把 s 和 t 各自 sort 一遍，再逐个字符比对。
        //     代码最短，但每多一次比较就多一次 O(n) 扫描。
        //     写法：return s.size() == t.size() && (sort(s), sort(t), s == t);
        //
        //   两个坑，写完先自己检查一遍：
        //     1. 长度不同必须提前返回 —— 只靠计数数组的话，
        //        "ab" 与 "a" 这种会在末尾被判成 true 吗？想清楚再写。
        //     2. 下标 c - 'a' 只在「保证是小写字母」时才安全；
        //        如果测试里混进大写或其他字符，会越界写坏内存。
        //
        //   进阶（题目末尾的追问）：如果输入是 unicode 字符怎么办？
        //     定长 26 数组的前提就没了，要换成 unordered_map<uint32_t, int>
        //     或者先按 UTF-8 码点解码再统计；核心思路（两边计数相消）不变。

        sort(s.begin(),s.end());
        sort(t.begin(),t.end());

        for(size_t i = 0; i < s.size() || i < t.size(); ++i){
            if(s[i] != t[i]){
                return false;
            }
        }
        return true;  
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase {
    string name;      // 用例名称
    string s;         // 输入 s
    string t;         // 输入 t
    bool expected;    // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    Solution sol;
    bool actual = sol.isAnagram(c.s, c.t);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  s = \"" << c.s << "\""
         << "  t = \"" << c.t << "\""
         << "  期望: " << boolalpha << c.expected
         << "  实际: " << actual << noboolalpha
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");  // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "anagram", "nagaram", true},
        {"示例 2", "rat", "car", false},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"完全相同", "abc", "abc", true},
        {"单字符相同", "a", "a", true},
        {"单字符不同", "a", "b", false},
        {"长度不同（短在前）", "ab", "a", false},
        {"长度不同（长在前）", "a", "ab", false},
        {"字母相同但数量不同", "aab", "abb", false},
        {"相同字母不同顺序", "listen", "silent", true},
        {"字母表全用一遍", "abcdefghijklmnopqrstuvwxyz", "zyxwvutsrqponmlkjihgfedcba", true},
        {"只差一个字符的顺序", "abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxyy", false},
        {"有重复字母的异位词", "aacc", "ccaa", true},
        {"重复字母交叉排列", "aacc", "acac", true},
        {"首尾字母互换", "az", "za", true},
        {"两边同样多但字母不同", "aabb", "ccdd", false},
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

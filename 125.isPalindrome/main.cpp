#include <iostream>
#include <string>
#include <vector>
#include <cctype>   // isalnum / tolower：处理非字母数字字符与大小写

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    bool isPalindrome(string s) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   题目要求：只考虑字母和数字，且忽略大小写；空串视为回文。
        //   这题是「双指针」主题的第一题，最自然的写法就是左右夹逼：
        //     一个指针从前往后、一个从后往前，跳过非字母数字，逐对比较。
        //
        //   思路一 · 双指针 + 原地跳过（O(n) 时间 / O(1) 空间，推荐）
        //     左右两个下标 left / right，循环 while (left < right)：
        //       遇到非字母数字就跳过（注意 left++ / right-- 的顺序）；
        //       两边都落在有效字符上时比较，统一转小写（或大写）再比。
        //     要点：判断「是否字母数字」用 isalnum，转小写用 tolower。
        //
        //   思路二 · 先清洗再判断（O(n) 时间 / O(n) 空间）
        //     先遍历一遍，把字母数字收集到一个新字符串里（顺便转小写），
        //     然后用「反转后比较」或再走一次双指针。
        //     代码更好写，但多花了 O(n) 空间 —— 面试时通常会被要求优化到 O(1)。
        //
        //   三个坑：
        //     1. 忘记忽略大小写 —— "0P" 和 "0p" 这一类会误判；
        //     2. 忘记跳过非字母数字 —— ".," 这种全是标点的串应为 true，
        //        但按字符硬比会得到 false；
        //     3. 内层跳过时的边界 —— 两个 while 里都要带 left < right 判断，
        //        否则 "..." 或 "a...." 这类输入会冲过头、越界访问。
        //
        //   一个常见写法细节：把 isalnum/tolower 的参数转成 unsigned char
        //   （isalnum(static_cast<unsigned char>(c))），避免传入负值导致未定义行为。
        //
        //   进阶（题目追问）：若字符集很大（unicode），双指针思路不变，
        //   但「是否算字符」和「大小写折叠」的规则要按码点重新定义。
        
        string str{};

        for(char c : s) {
            if(c >= '0' && c <= '9') {
                str.push_back(c);
            }
            if(c >= 'a' && c <= 'z') {
                str.push_back(c);
            }
            if(c >= 'A' && c <= 'Z') {
                str.push_back(c + ('a' - 'A'));
            }
        }


        int left = 0, right = str.size() - 1;

        while(left < right) {
            if(str[left] != str[right])
                return false;

            left++;
            right--;
        }
        

        return true;  
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例
struct TestCase {
    string name;      // 用例名称
    string s;         // 输入
    bool expected;    // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    Solution sol;
    bool actual = sol.isPalindrome(c.s);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  s = \"" << c.s << "\""
         << "  期望: " << boolalpha << c.expected
         << "  实际: " << actual << noboolalpha
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "A man, a plan, a canal: Panama", true},
        {"示例 2", "race a car", false},
        {"示例 3", " ", true},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"空串视为回文",         "", true},
        {"单字符",              "a", true},
        {"单个数字",            "0", true},
        {"两个数字相同",        "00", true},
        {"两个数字不同",        "01", false},
        {"纯标点视为回文",      ".,", true},
        {"纯标点更长",          "!@#$%^&*()", true},
        {"大小写不敏感",        "Aa", true},
        {"0P 与 0p 应不匹配",   "0P", false},
        {"数字与字母混排",      "a1b2b1a", true},
        {"中间夹标点",          "ab, ba", true},
        {"尾部有冗余标点",      "ab@ba!!!", true},
        {"头部全是标点",        "!!!abba", true},
        {"看似回文但差一个字符", "abca", false},
        {"全部同一字母",        "aaaa", true},
        {"数字回文",            "12321", true},
        {"数字非回文",          "12345", false},
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

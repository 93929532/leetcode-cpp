#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <windows.h>

using namespace std;

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        
        unordered_set<char> charSet;
        int right, left, maxLength = 0;
        // 使用滑动窗口方法
        for(left = 0, right = 0; right < s.length(); ++right) {
            
            while(charSet.count(s[right])) {
                charSet.erase(s[left]); 
                left++;
            }

            charSet.insert(s[right]);
            maxLength = max(maxLength, right - left + 1);
        }
        
        return maxLength; // 返回最长子串的长度
    }
};

// ==================== 答题区域结束，下方为测试框架 ====================

int main() {
    // 设置控制台输入/输出为 UTF-8 编码，避免中文乱码
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Solution solution;

    struct TestCase {
        string input;
        int expected;
    };

    vector<TestCase> testCases = {
        {"abcabcbb", 3},   // 示例 1：最长子串 "abc"
        {"bbbbb", 1},      // 示例 2：最长子串 "b"
        {"pwwkew", 3},     // 示例 3：最长子串 "wke"
        {"", 0},           // 空串
        {" ", 1},          // 单个字符
        {"au", 2},         // 无重复
        {"dvdf", 3},       // 重复字符穿插
    };

    bool allPassed = true;
    for (size_t i = 0; i < testCases.size(); ++i) {
        int result = solution.lengthOfLongestSubstring(testCases[i].input);
        bool pass = (result == testCases[i].expected);
        allPassed = allPassed && pass;

        cout << "测试用例 " << i + 1 << ": "
             << "s = \"" << testCases[i].input << "\" "
             << "期望 = " << testCases[i].expected
             << ", 实际 = " << result
             << (pass ? "  [通过]" : "  [失败]") << endl;
    }

    cout << endl;
    if (allPassed) {
        cout << "全部测试通过！" << endl;
        return 0;
    } else {
        cout << "存在测试未通过！" << endl;
        return 1;
    }
}



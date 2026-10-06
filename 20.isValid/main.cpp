#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <utility>

using namespace std;

class Solution {
public:
    bool isValid(string s) {
        
        vector<char> bracket;

        for(char c : s){

            if(c == '(' || c == '[' || c == '{')
                bracket.push_back(c);
            
            if(c == ')'){
                if(bracket.empty() || bracket.back() != '(')
                    return false;
                bracket.pop_back();
            }
            if(c == ']'){
                if(bracket.empty() || bracket.back() != '[')
                    return false;
                bracket.pop_back();
            }
            if(c == '}'){
                if(bracket.empty() || bracket.back() != '{')
                    return false;
                bracket.pop_back();
            }
        }

        return bracket.empty();
    }
};

// ------------------------- 测试辅助代码 -------------------------

// 单条测试用例
struct TestCase {
    string name;   // 用例名称
    string input;  // 输入字符串
    bool expected; // 期望结果
};

// 根据测试结果打印一行信息，返回该用例是否通过
bool runCase(const TestCase& c) {
    Solution sol;
    bool actual = sol.isValid(c.input);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  s = \"" << c.input << "\""
         << "  期望: " << boolalpha << c.expected
         << "  实际: " << actual << noboolalpha
         << endl;
    return pass;
}

int main() {
    // 让控制台按 UTF-8 输出，避免中文乱码（Windows 下有效）
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    // 题目给出的 5 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", "()", true},
        {"示例 2", "()[]{}", true},
        {"示例 3", "(]", false},
        {"示例 4", "([])", true},
        {"示例 5", "([)]", false},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"空字符串", "", true},
        {"单个左括号", "(", false},
        {"单个右括号", ")", false},
        {"只有左括号", "([{", false},
        {"只有右括号", ")]}", false},
        {"嵌套多层", "{[()]}", true},
        {"顺序错误", "([)]", false},
        {"类型交叉", "{[]}", true},
        {"奇数长度", "()(", false},
        {"类型不匹配", "([}}])", false},
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

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        
        string s = "";
        
        // 遍历每个字符，直到找到不匹配的字符
        for(int i = 0 ;s.size() < strs[0].size() ;i ++)
        {
            s.push_back(strs[0][i]);

            // 遍历每个字符串，检查当前字符是否相同
            for(int j = 0 ;j < strs.size() ;j ++)
            {
                if(s[i] != strs[j][i])
                {
                    s.pop_back();
                    return s;
                }           
            }
        }
        return s;
    }
};

// 打印字符串数组
void printVector(const vector<string>& strs) {
    cout << "[";
    for (size_t i = 0; i < strs.size(); ++i) {
        cout << "\"" << strs[i] << "\"";
        if (i + 1 < strs.size()) {
            cout << ",";
        }
    }
    cout << "]";
}

// 运行一组测试用例
void runTest(vector<string> strs) {
    Solution sol;
    string result = sol.longestCommonPrefix(strs);

    cout << "输入: strs = ";
    printVector(strs);
    cout << "\n输出: \"" << result << "\"\n" << endl;
}

int main() {
    // 示例 1：期望输出 "fl"
    runTest({"flower", "flow", "flight"});

    // 示例 2：期望输出 ""
    runTest({"dog", "racecar", "car"});

    return 0;
}

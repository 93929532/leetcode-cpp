#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    /*
     * 实现支持 '.' 和 '*' 的正则表达式匹配。
     * '.' 匹配任意单个字符
     * '*' 匹配零个或多个前面的那一个元素
     * 要求匹配覆盖整个输入字符串 s（而非部分）。
     */
    bool isMatch(string s, string p) {
        int m = s.size(), n = p.size();
        // dp[i][j]: s[0..i-1] 能否被 p[0..j-1] 匹配
        vector<vector<bool>> dp(m + 1, vector<bool>(n + 1, false));

        // 空串匹配空模式
        dp[0][0] = true;

        // 空串匹配模式：只有 'x*' 这种可以被抵消成空
        for(int j = 1; j <= n; j++)
        {
            // 如果模式的当前字符是 '*'，则检查前一个字符是否可以匹配空串
            if(p[j - 1] == '*')
            {
                dp[0][j] = dp[0][j - 2];
            }
        }

        for(int i = 1; i <= m; i++)
            for(int j = 1; j <= n; j ++)
            {
                if(p[j - 1] == '*')
                {
                    // 匹配 0 个前一元素
                    dp[i][j] = dp[i][j - 2];
                    // 匹配 1 个或多个前一元素
                    if (p[j - 2] == '.' || p[j - 2] == s[i - 1])
                    {
                        dp[i][j] = dp[i][j] || dp[i - 1][j];
                    }
                }
                else
                {
                    // 匹配单个字符
                    if(p[j - 1] == '.' || p[j - 1] == s[i - 1])
                    {
                        dp[i][j] = dp[i - 1][j - 1];
                    }
                }
            }

        return dp[m][n];
    }
};

// 测试用例
static void runTest(const string& s, const string& p, bool expected) {
    Solution sol;
    bool result = sol.isMatch(s, p);
    cout << "s = \"" << s << "\", p = \"" << p << "\" => "
         << (result ? "true" : "false")
         << " (期望: " << (expected ? "true" : "false") << ")"
         << (result == expected ? "  [通过]" : "  [失败]")
         << endl;
}

int main() {
    // 示例 1
    runTest("aa", "a", false);
    // 示例 2
    runTest("aa", "a*", true);
    // 示例 3
    runTest("ab", ".*", true);
    // 额外用例
    runTest("aab", "c*a*b", true);
    runTest("mississippi", "mis*is*p*.", false);
    runTest("", "", true);
    runTest("", "a*", true);

    return 0;
}

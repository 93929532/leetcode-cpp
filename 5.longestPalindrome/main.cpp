#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    string longestPalindrome(string s) {

        string longest = "";

        for(int i=0; i < s.length(); i++)
        {
            for(int j=i ; j < s.length(); j++)
            {
                string sub = s.substr(i, j-i+1);
                string rev = sub;

                for(int k=0; k < rev.length()/2; k++)
                {
                    swap(rev[k], rev[rev.length()-1-k]);
                }

                if(sub == rev)
                {
                    if(sub.length() > longest.length())
                    {
                        longest = sub;
                    }
                }
            }
        }
        
        return longest;
    }
};

int main() {
    Solution solution;

    // 示例 1
    {
        string s = "babad";
        string result = solution.longestPalindrome(s);
        cout << "输入: s = \"" << s << "\"" << endl;
        cout << "输出: \"" << result << "\"  (正确: \"bab\" 或 \"aba\")" << endl << endl;
    }

    // 示例 2
    {
        string s = "cbbd";
        string result = solution.longestPalindrome(s);
        cout << "输入: s = \"" << s << "\"" << endl;
        cout << "输出: \"" << result << "\"  (正确: \"bb\")" << endl << endl;
    }

    // 边界用例
    {
        string s = "a";
        string result = solution.longestPalindrome(s);
        cout << "输入: s = \"" << s << "\"" << endl;
        cout << "输出: \"" << result << "\"  (正确: \"a\")" << endl << endl;
    }

    return 0;
}

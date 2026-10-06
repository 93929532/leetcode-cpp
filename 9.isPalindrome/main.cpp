#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    bool isPalindrome(int x) {
        
        string num;

        if(x < 0)
            return false;

        while(x != 0)
        {
            num.push_back(x % 10 + '0');
            x /= 10;
        }

        for(int i = 0; i < num.size()/2; i++)
        {
            if(num[i] != num[num.size() - (i + 1)])
            {
                return false;
            }
        }

        return true;
    }
};

int main() {
    Solution solution;

    // 示例测试用例
    struct TestCase {
        int x;
        bool expected;
    };

    vector<TestCase> testCases = {
        {121, true},
        {-121, false},
        {10, false},
        {7007, true},
    };

    int index = 1;
    for (const auto& tc : testCases) {
        bool result = solution.isPalindrome(tc.x);
        cout << "示例 " << index++
             << "：x = " << tc.x
             << "，期望 " << (tc.expected ? "true" : "false")
             << "，实际 " << (result ? "true" : "false")
             << (result == tc.expected ? "（通过）" : "（失败）")
             << endl;
    }

    return 0;
}
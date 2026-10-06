#include <iostream>
#include <vector>
#include <string>
#include <climits>

using namespace std;

#include <cmath>

class Solution {
public:
    int reverse(int x) {
        
        long long rev = 0;

        while (x != 0) {
            int digit = x % 10;
            x /= 10;
            
            if (rev > INT_MAX / 10 || (rev == INT_MAX / 10 && digit > 7)) 
                return 0; // 溢出检查

            if (rev < INT_MIN / 10 || (rev == INT_MIN / 10 && digit < -8))
                return 0; // 溢出检查

            rev = rev * 10 + digit;
        }
        return rev; 
    }
};

// 测试辅助函数：将测试用例编号、输入、期望输出和实际输出打印出来进行比对
void run_test(int test_id, int input, int expected) {
    Solution sol;
    int result = sol.reverse(input);

    cout << "示例 " << test_id << ":" << endl;
    cout << "  输入: x = " << input << endl;
    cout << "  输出: " << result << endl;
    cout << "  期望: " << expected << endl;
    cout << "  " << (result == expected ? "通过 ✓" : "失败 ✗") << endl;
    cout << endl;
}

int main() {
    // 题目给出的示例
    run_test(1, 123, 321);       // 示例 1
    run_test(2, -123, -321);     // 示例 2
    run_test(3, 120, 21);        // 示例 3

    // 额外边界用例
    run_test(4, 0, 0);                          // 0
    run_test(5, 1534236469, 0);                 // 反转后溢出，应返回 0
    run_test(6, -2147483648, 0);                // INT_MIN 反转后溢出
    run_test(7, 2147483647, 0);                 // INT_MAX 反转后溢出
    run_test(8, -2147483412, -2143847412);      // 在范围内
    run_test(9, 1463847412, 2147483641);        // 接近上界

    return 0;
}

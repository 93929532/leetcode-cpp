#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    string convert(string s, int numRows) {
        // 特殊情况处理
        if (numRows == 1) 
            return s;   
        // 创建一个字符串数组来存储每一行的字符
        vector<string> rows(numRows);
    
        // 当前行索引和方向标志
        int currentRow = 0;
        bool down = true;

        for (int i = 0; i < s.length(); i++)
        {
            rows[currentRow].push_back(s[i]);
            
            if(down) {
                currentRow++;
                if (currentRow == numRows - 1) {
                    down = false; // 到达底部，改变方向
                }
            } else {
                currentRow--;
                if (currentRow == 0) {
                    down = true; // 到达顶部，改变方向
                }
            }
        }

        // 将每一行的字符连接起来形成最终结果
        s = "";
        for (const string& row : rows) {
            s += row;
        }

        return s;
    }
};

// 测试辅助函数
static void runTest(const string& s, int numRows, const string& expected) {
    Solution sol;
    string result = sol.convert(s, numRows);
    cout << "输入: \"" << s << "\", numRows = " << numRows << "\n";
    cout << "期望: \"" << expected << "\"\n";
    cout << "实际: \"" << result << "\"\n";
    cout << (result == expected ? "[通过]" : "[失败]") << "\n\n";
}

int main() {
    // 示例 1
    runTest("PAYPALISHIRING", 3, "PAHNAPLSIIGYIR");
    // 示例 2
    runTest("PAYPALISHIRING", 4, "PINALSIGYAHRPI");

    // 边界情况
    runTest("", 3, "");              // 空字符串
    runTest("AB", 5, "AB");          // numRows >= 长度
    runTest("ABC", 1, "ABC");        // numRows == 1（当前会崩溃）

    return 0;
}

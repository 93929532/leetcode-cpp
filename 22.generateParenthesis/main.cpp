// 22. 括号生成 (Generate Parentheses)
// 题目：数字 n 代表生成括号的对数，设计一个函数，生成所有可能的并且有效的括号组合。
// 提示：1 <= n <= 8
//
// 说明：以下代码中，除 generateParenthesis 的函数体（答题区域）外，其余部分均已补全，
//       可直接编译运行。请在标有“答题区域”的位置填写你的解法。

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class Solution {
public:
    vector<string> generateParenthesis(int n) {
        vector<string> res;
        dfs("", 0, 0, n, res);

        return res;
    }
private:
    void dfs(string cur, int left, int right, int n, vector<string> &res){
            
        if(left == n && right == n){
            res.push_back(cur);
            return;
        }

        if(left < n){
            dfs(cur + '(', left + 1, right, n, res);
        }

        if(right < left){
            dfs(cur + ')', left, right + 1, n, res);
        }
        return;
    }
};

// 按力扣格式打印结果，例如：["((()))","(()())","(())()","()(())","()()()"]
void printResult(const vector<string> &res) {
    cout << "[";
    for (size_t i = 0; i < res.size(); ++i) {
        if (i > 0) {
            cout << ",";
        }
        cout << "\"" << res[i] << "\"";
    }
    cout << "]" << endl;
}

// 运行单个测试用例
void runCase(int n) {
    Solution sol;
    vector<string> res = sol.generateParenthesis(n);
    cout << "输入: n = " << n << endl;
    cout << "输出: ";
    printResult(res);
    cout << "组合数量: " << res.size() << endl;
    cout << "----------------------------------------" << endl;
}

int main() {
#ifdef _WIN32
    // 修复 Windows 控制台输出中文乱码（源码 UTF-8 <-> 控制台代码页）
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 力扣示例
    runCase(3); // 期望 5 个组合
    runCase(1); // 期望 ["()"]

    // 其他测试用例（范围 1 <= n <= 8）
    runCase(2); // 期望 2 个组合
    runCase(4); // 期望 14 个组合

    return 0;
}
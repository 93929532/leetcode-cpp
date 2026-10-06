#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    bool containsDuplicate(vector<int>& nums) {
        
        for(size_t i = 0; i < nums.size(); i++)
            for(size_t j = i + 1; j < nums.size(); j++) {
                if(nums[i] == nums[j])
                    return true;
            }

        return false;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 "[1, 2, 3]" 形式
static string toString(const vector<int>& nums) {
    string s = "[";
    for (size_t i = 0; i < nums.size(); ++i) {
        if (i > 0) s += ", ";
        s += to_string(nums[i]);
    }
    s += "]";
    return s;
}

// 单条测试用例
struct TestCase {
    string name;       // 用例名称
    vector<int> nums;  // 输入数组
    bool expected;     // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    // 题目签名是 vector<int>&，这里复制一份，避免解法改动输入影响结果打印
    vector<int> nums = c.nums;

    Solution sol;
    bool actual = sol.containsDuplicate(nums);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << toString(c.nums)
         << "  期望: " << boolalpha << c.expected
         << "  实际: " << actual << noboolalpha
         << endl;
    return pass;
}

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");  // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 2, 3, 1}, true},
        {"示例 2", {1, 2, 3, 4}, false},
        {"示例 3", {1, 1, 1, 3, 3, 4, 3, 2, 4, 2}, true},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"单元素", {1}, false},
        {"两个相同元素", {5, 5}, true},
        {"两个不同元素", {5, 6}, false},
        {"全是同一个数", {-1, -1, -1}, true},
        {"重复出现在首尾", {7, 1, 2, 3, 7}, true},
        {"重复出现在中间", {4, 8, 15, 16, 15, 23, 42}, true},
        {"只有最后一对相同", {1, 2, 3, 4, 4}, true},
        {"升序无重复", {-5, -3, -1, 0, 2, 4}, false},
        {"取值到达上界", {-1000000000, 1000000000, -1000000000}, true},
        {"取值到达上下界但不重复", {-1000000000, 0, 1000000000}, false},
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

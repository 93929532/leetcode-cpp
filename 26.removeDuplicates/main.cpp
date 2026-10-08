#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    int removeDuplicates(vector<int>& nums) {

        int fast = 0, slow = 0;

        while (fast < nums.size()) {
            if (nums[fast] > nums[slow]) {
                slow++;
                nums[slow] = nums[fast];
            }
            else {
                fast++;
            }
        }
        return slow + 1;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 打印数组的前 k 个元素（模拟判题只看 [0, k) 区间）
static string arrayToString(const vector<int>& nums, int k) {
    if (k <= 12) {
        string s = "[";
        for (int i = 0; i < k; ++i) {
            if (i > 0) s += ", ";
            s += to_string(nums[i]);
        }
        return s + "]";
    }

    // 过长的数组只打印首 6 个与末 3 个，避免刷屏
    string s = "[";
    for (int i = 0; i < 6; ++i) {
        if (i > 0) s += ", ";
        s += to_string(nums[i]);
    }
    s += ", …(略去 " + to_string(k - 9) + " 个)…, ";
    for (int i = k - 3; i < k; ++i) {
        if (i > k - 3) s += ", ";
        s += to_string(nums[i]);
    }
    return s + "]";
}

// 单条测试用例
struct TestCase {
    string name;          // 用例名称
    vector<int> nums;     // 输入数组（非严格递增）
    vector<int> expected; // 期望的前 k 个元素
};

// 运行一条用例，打印一行信息，返回该用例是否通过
// 判题标准与题目给出的一致：
//   int k = removeDuplicates(nums);
//   assert k == expectedNums.length;
//   for (int i = 0; i < k; i++) assert nums[i] == expectedNums[i];
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 解法会原地修改，先拷贝一份，保留原始输入用于打印

    Solution sol;
    int k = sol.removeDuplicates(nums);

    bool pass = true;
    string reason;

    if (k != static_cast<int>(c.expected.size())) {
        pass = false;
        reason = "返回长度 k = " + to_string(k) +
                 "，期望长度 = " + to_string(c.expected.size());
    }
    else if (k < 0 || k > static_cast<int>(nums.size())) {
        pass = false;
        reason = "返回的 k 越界，合法区间是 [0, " + to_string(nums.size()) + "]";
    }
    else {
        for (int i = 0; i < k; ++i) {
            if (nums[i] != c.expected[i]) {
                pass = false;
                reason = "nums[" + to_string(i) + "] = " + to_string(nums[i]) +
                         "，期望 = " + to_string(c.expected[i]);
                break;
            }
        }
    }

    // k 越界时只打印安全区间，避免测试脚手架自己越界崩溃
    const int safeK = (k < 0 || k > static_cast<int>(nums.size())) ? 0 : k;

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  输入: " << arrayToString(c.nums, static_cast<int>(c.nums.size()))
         << "  期望前 " << c.expected.size() << " 个: "
         << arrayToString(c.expected, static_cast<int>(c.expected.size()))
         << "  实际前 " << safeK << " 个: " << arrayToString(nums, safeK)
         << endl;
    if (!pass) {
        cout << "       原因: " << reason << endl;
    }
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 1, 2}, {1, 2}},
        {"示例 2", {0, 0, 1, 1, 1, 2, 2, 3, 3, 4}, {0, 1, 2, 3, 4}},
    };

    // 额外的边界与补充用例
    // 注意：题目约束 1 <= nums.length <= 3*10^4，空数组不在约束范围内，故不设空数组用例
    //      （现有解法 `return slow + 1` 在空数组上会返回 1）
    vector<TestCase> extraCases = {
        {"单个元素",       {7}, {7}},
        {"两个元素相同",   {1, 1}, {1}},
        {"两个元素不同",   {1, 2}, {1, 2}},
        {"全部元素相同",   {3, 3, 3, 3, 3}, {3}},
        {"本身无重复",     {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}},
        {"含负数与重复",   {-5, -5, -3, 0, 0, 0, 1, 2, 2}, {-5, -3, 0, 1, 2}},
    };

    // 长数组用例：1000 个不同值，每个值重复 1~3 次
    {
        vector<int> nums;
        vector<int> expected;
        for (int v = 0; v < 1000; ++v) {
            const int repeat = (v % 3) + 1;
            for (int r = 0; r < repeat; ++r) nums.push_back(v);
            expected.push_back(v);
        }
        extraCases.push_back({"长数组（1000 个不同值）", nums, expected});
    }

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

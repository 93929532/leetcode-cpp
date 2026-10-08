#include <iostream>
#include <vector>
#include <string>
#include <algorithm>   // std::sort：题目不要求剩余元素顺序，比较前统一排序

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {

        int fast = 0, slow = 0;

        for (int i = 0; i < nums.size(); i++) {
            if (nums[fast] != val) {
                nums[slow] = nums[fast];
                fast++;
                slow++;
            }
            else {
                fast++;
            }
        }
        return slow;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 打印数组的前 k 个元素（题目只看 [0, k) 区间）
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
    vector<int> nums;     // 输入数组
    int val;              // 要移除的值
    vector<int> expected; // 期望剩下的元素（顺序无关）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
// 判题标准与题目一致：返回 k，且 nums 的前 k 个元素恰好是全部不等于 val 的元素（顺序任意）
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 解法会原地修改，先拷贝一份，保留原始输入用于打印

    Solution sol;
    int k = sol.removeElement(nums, c.val);

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
        vector<int> head(nums.begin(), nums.begin() + k);   // 只取前 k 个，顺序不作要求
        vector<int> want = c.expected;
        sort(head.begin(), head.end());
        sort(want.begin(), want.end());

        if (head != want) {
            pass = false;
            reason = "前 " + to_string(k) + " 个元素的集合与期望不一致";
        }
    }

    // k 越界时只打印安全区间，避免测试脚手架自己越界崩溃
    const int safeK = (k < 0 || k > static_cast<int>(nums.size())) ? 0 : k;

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << arrayToString(c.nums, static_cast<int>(c.nums.size()))
         << "  val = " << c.val
         << "  期望剩下: " << arrayToString(c.expected, static_cast<int>(c.expected.size()))
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
        {"示例 1", {3, 2, 2, 3}, 3, {2, 2}},
        {"示例 2", {0, 1, 2, 2, 3, 0, 4, 2}, 2, {0, 0, 1, 3, 4}},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"空数组",           {}, 0, {}},
        {"单个元素等于 val", {1}, 1, {}},
        {"单个元素不等于 val", {1}, 2, {1}},
        {"全部等于 val",     {2, 2, 2}, 2, {}},
        {"全部不等于 val",   {1, 2, 3}, 0, {1, 2, 3}},
        {"含负数",           {-1, -2, -1, 0}, -1, {-2, 0}},
        {"val 同时出现在首尾", {5, 1, 2, 5}, 5, {1, 2}},
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

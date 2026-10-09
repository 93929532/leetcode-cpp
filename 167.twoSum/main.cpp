#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    vector<int> twoSum(vector<int> &numbers, int target)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   题目三个关键条件，每一个都要用上：
        //     ① 数组已按【非递减】排序  ② 恰好存在一个解  ③ 返回 1-based 下标
        //
        //   思路一 · 对撞双指针（O(n) 时间 / O(1) 空间，本题标准答案）
        //     左指针在最左、右指针在最右，比较 numbers[left] + numbers[right] 与 target：
        //       · 和等于 target → 找到，返回 {left+1, right+1}（注意 +1 是 1-based）
        //       · 和小于 target → 说明左侧的数太小，left++
        //       · 和大于 target → 说明右侧的数太大，right--
        //     为什么这样不会漏解？因为数组有序：当和偏小时，numbers[left] 与任何
        //     比 numbers[right] 更靠左的数配对都会更小，所以 numbers[left] 可以安全排除。
        //
        //   思路二 · 哈希表（O(n) 时间 / O(n) 空间）
        //     照搬第 1 题「两数之和」的做法：边遍历边查 target - x 是否出现过。
        //     正确但没用上「已排序」这个条件 —— 面试官会追问能不能 O(1) 空间。
        //
        //   思路三 · 固定一个数后二分（O(n log n)）
        //     对每个 i，在 i+1..n-1 里二分查找 target - numbers[i]。
        //     比思路二差，只作为「知道能用有序性」的过渡理解。
        //
        //   三个坑：
        //     1. 下标是 1-based —— 返回时要 +1，这是本题最常见的失分点；
        //     2. 不能重复使用同一元素 —— 所以循环条件是 left < right 而不是 <=；
        //     3. 不要在循环里用 numbers.size() - 1 直接算右指针，转换类型时留意
        //        无符号下溢（空数组时 size()-1 会变成极大值）。

        int index1 = 1, index2 = numbers.size();

        while (index1 < index2)
        {
            int sum = numbers[index1 - 1] + numbers[index2 - 1];

            if (sum < target)
                ++index1;
            else if (sum > target)
                --index2;
            else
                break;
        }

        return {index1, index2};
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 "[1, 2, 3]"
static string toString(const vector<int> &v)
{
    string s = "[";
    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i)
            s += ", ";
        s += to_string(v[i]);
    }
    s += "]";
    return s;
}

// 单条测试用例
struct TestCase
{
    string name;          // 用例名称
    vector<int> numbers;  // 输入数组（已排序）
    int target;           // 目标和
    vector<int> expected; // 期望的 1-based 下标
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    // 题目签名是 vector<int>&，复制一份避免解法改动输入影响结果打印
    vector<int> numbers = c.numbers;

    Solution sol;
    vector<int> actual = sol.twoSum(numbers, c.target);

    bool pass = (actual == c.expected);
    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  numbers = " << toString(c.numbers)
         << "  target = " << c.target
         << "  期望下标 = " << toString(c.expected)
         << "  实际下标 = " << toString(actual)
         << endl;

    // 额外校验：返回的下标必须真的是 1-based 且指向的两个数之和等于 target
    if (pass)
    {
        int i = actual[0], j = actual[1];
        bool validIdx = (i >= 1 && i <= (int)c.numbers.size() &&
                         j >= 1 && j <= (int)c.numbers.size() && i < j);
        bool sumOk = validIdx && (c.numbers[i - 1] + c.numbers[j - 1] == c.target);
        if (!validIdx || !sumOk)
        {
            cout << "       ⚠ 数值虽然匹配，但下标不合法或求和不对（1-based 校验未通过）" << endl;
            return false;
        }
    }
    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {2, 7, 11, 15}, 9, {1, 2}},
        {"示例 2", {2, 3, 4}, 6, {1, 3}},
        {"示例 3", {-1, 0}, -1, {1, 2}},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"最短数组", {1, 2}, 3, {1, 2}},
        {"答案在数组中间", {1, 2, 3, 4, 5}, 7, {2, 5}},
        {"答案相邻在下标 1-2", {1, 2, 3, 4, 5}, 3, {1, 2}},
        {"答案相邻在末尾", {1, 2, 3, 4, 5}, 9, {4, 5}},
        {"两数相等各取一次", {3, 3}, 6, {1, 2}},
        {"含负数与零", {-3, -1, 0, 2, 4}, 1, {1, 5}},
        {"全负数", {-5, -4, -3, -2}, -6, {2, 4}},
        {"答案跨越整个数组", {-10, -1, 0, 1, 10}, 0, {1, 5}},
        {"大数值不溢出", {1000, 2000, 3000}, 5000, {2, 3}},
        {"含重复值取靠前组合", {1, 1, 2, 3}, 2, {1, 2}},
        {"长数组答案在两端", {-1000, -500, 0, 500, 1000}, 0, {1, 5}},
    };

    size_t passed = 0;
    size_t total = 0;

    cout << "================ 题目示例 ================" << endl;
    for (const auto &c : sampleCases)
    {
        ++total;
        if (runCase(c))
            ++passed;
    }

    cout << endl
         << "================ 补充用例 ================" << endl;
    for (const auto &c : extraCases)
    {
        ++total;
        if (runCase(c))
            ++passed;
    }

    cout << endl
         << "================ 结果统计 ================" << endl;
    cout << "共 " << total << " 个用例，通过 " << passed
         << " 个，失败 " << (total - passed) << " 个。" << endl;
    cout << (passed == total ? "全部通过 ✔" : "存在失败用例 ✘") << endl;

    return 0;
}

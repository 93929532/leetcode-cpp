#include <iostream>
#include <string>
#include <vector>
#include <algorithm>     // sort：测试骨架比较"命中的两个数"时要用
#include <unordered_map> // 思路二用：哈希表

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    vector<int> twoSum(vector<int> &nums, int target)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   一句话概括这题：**用空间换时间，把"找另一个数"从 O(n) 的扫描
        //   变成 O(1) 的查表。**
        //
        //   思路一 · 暴力双层循环（O(n²)，先说出来的保底解）
        //     固定 i，再扫 j > i 找 nums[j] == target - nums[i]。能过，
        //     但面试官会立刻追问"能不能 O(n)"。
        //
        //   思路二 · 一次遍历 + 哈希表（标准解，O(n) 时间 / O(n) 空间）
        //     遍历 nums，对每个 x：
        //         if (map.count(target - x)) return {map[target - x], i};   // ① 先查
        //         map[x] = i;                                               // ② 再插
        //     关键全在顺序上：**必须先查再插**。
        //     反例 [3,3] target = 6：先插后查的话，i=1 时会在表里查到自己
        //     （刚被 i=0 写进去的 3），于是返回两个相同下标，属于"同一个元素用了两次"。
        //
        //   思路三 · 先建全表再查 / 排序 + 双指针（都只当对照）
        //     · 先建表再查：也能过，但会漏掉"同一元素用两次"的检查
        //       （[3,3] 会返回 {0,0} 这种非法答案）。
        //     · 排序 + 双指针：O(n log n)，而且**下标会被排序打乱**，
        //       得额外存 (值, 原下标) 才能返回 —— 所以本题不适合双指针。
        //       双指针是 167 的解法，因为 167 给的数组本来就是有序的。
        //
        //   六个坑：
        //     1. **先查后插**（见上，[3,3] 是标准反例，而且错了还不会崩溃）。
        //     2. **返回的是下标，不是值**：别把表存成"值 → 值"。
        //     3. **判断"在不在表里"要用 count / find，不要用 `map[key] != 0`**：
        //        `operator[]` 会给不存在的 key 默认构造一个 0，而下标 0 本身是合法答案，
        //        两种情况撞在一起就分不清了。
        //     4. **中间量刚好不溢出**：target 与 nums[i] 都在 ±1e9，所以
        //        target - x ∈ [-2e9, 2e9]，int 装得下（上限 2.147e9）—— 贴着边界但安全。
        //        想更稳可以用 long long 算这个差值。
        //     5. **题目保证只有一组解**，所以不必处理多解；但实现别假设一定有解，
        //        循环走完仍要有 return（返回空 vector 即可）。
        //     6. **表里存下标**：遇到重复值覆盖写入没关系 —— [3,3] 在 i=1 时就返回了，
        //        根本走不到第二次插入。
        //
        //   进阶：
        //     · 167「两数之和 II」给的是**有序**数组 → 双指针、O(1) 额外空间
        //       （你计划里序号 11，正好是双指针主题的下一道）
        //     · 本题的哈希思路推广出去：15 三数之和 / 18 四数之和 / 454 四数相加 II
        //     · 如果要求返回**所有**不重复的数对 → 变成 3Sum 那套"排序 + 双指针 + 去重"
        //     · 「边遍历边建表」在"数据流式到达"的题里就是唯一解，这道题是它的原型

        unordered_map<int, int> indexMap;

        for (int i = 0; i < nums.size(); ++i)
        {
            int c = target - nums[i];

            if (indexMap.find(c) != indexMap.end())
                return {indexMap[c], i};
            
            indexMap[nums[i]] = i;
        }

        return {};
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [2,7,11,15] 便于打印；过长的数组只打印首 6 末 3
static string vectorToString(const vector<int> &v)
{
    if (v.size() <= 12)
    {
        string s = "[";
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (i)
                s += ",";
            s += to_string(v[i]);
        }
        return s + "]";
    }

    string s = "[";
    for (int i = 0; i < 6; ++i)
    {
        if (i)
            s += ",";
        s += to_string(v[i]);
    }
    s += ", …(略去 " + to_string(v.size() - 9) + " 个)…, ";
    for (size_t i = v.size() - 3; i < v.size(); ++i)
    {
        if (i != v.size() - 3)
            s += ",";
        s += to_string(v[i]);
    }
    return s + "]";
}

// 单条测试用例
//
// 注意这里存的是**期望命中的两个数**，而不是期望的下标对：
// 题目只保证"值"层面的解唯一，不保证下标唯一（例如 [5,5,5] target=10，
// 三对下标都合法）。所以校验方式是"属性检查"——见下面的 runCase。
struct TestCase
{
    string name;                // 用例名称
    vector<int> nums;           // 输入数组
    int target;                 // 目标和
    vector<int> expectedValues; // 期望命中的两个数（顺序无关）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
//
// 校验的是「返回的到底是不是一个合法答案」，共四步：
//   ① 长度必须是 2
//   ② 两个下标都得在 [0, n) 内
//   ③ 两个下标必须不同（题目不允许同一个元素用两次）
//   ④ 两个位置的值相加必须等于 target，且这两个值就是期望的那两个
static bool runCase(const TestCase &c)
{
    vector<int> nums = c.nums; // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    vector<int> ans = sol.twoSum(nums, c.target);

    bool pass = true;
    string why;
    string detail;

    if (ans.size() != 2)
    {
        pass = false;
        why = "返回的元素个数是 " + to_string(ans.size()) + "，应当是 2";
    }
    else
    {
        const int a = ans[0], b = ans[1];
        if (a < 0 || b < 0 || a >= static_cast<int>(nums.size()) || b >= static_cast<int>(nums.size()))
        {
            pass = false;
            why = "下标越界：返回 [" + to_string(a) + "," + to_string(b) +
                  "]，合法范围是 [0," + to_string(nums.size()) + ")";
        }
        else if (a == b)
        {
            pass = false;
            why = "同一个元素被用了两次（两个下标相同：" + to_string(a) + "）";
        }
        else
        {
            long long sum = static_cast<long long>(nums[a]) + nums[b];
            detail = "下标 [" + to_string(a) + "," + to_string(b) + "]" + "（值 " + to_string(nums[a]) + " + " + to_string(nums[b]) + " = " + to_string(sum) + "）";
            if (sum != c.target)
            {
                pass = false;
                why = "两数之和 = " + to_string(sum) + "，target = " + to_string(c.target);
            }
            else
            {
                vector<int> got = {nums[a], nums[b]};
                vector<int> want = c.expectedValues;
                sort(got.begin(), got.end());
                sort(want.begin(), want.end());
                if (got != want)
                {
                    pass = false;
                    why = "命中的两个数与期望不符";
                }
            }
        }
    }

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << vectorToString(c.nums)
         << "  target = " << c.target
         << "  期望命中: " << vectorToString(c.expectedValues);
    if (pass)
    {
        cout << "  实际: " << detail;
    }
    else
    {
        cout << "  实际返回: " << vectorToString(ans) << "\n"
             << "       原因: " << why;
    }
    cout << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {2, 7, 11, 15}, 9, {2, 7}},
        {"示例 2", {3, 2, 4}, 6, {2, 4}},
        {"示例 3（同一个值出现两次）", {3, 3}, 6, {3, 3}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"两个元素", {1, 2}, 3, {1, 2}},
        {"全负数", {-1, -2, -3, -4, -5}, -8, {-3, -5}},
        {"含 0 且答案在两端", {0, 4, 3, 0}, 0, {0, 0}},
        {"负数配正数", {-3, 4, 3, 90}, 0, {-3, 3}},
        {"答案在末尾", {1, 2, 3, 4, 5, 6}, 11, {5, 6}},
        {"有重复值但不是答案", {1, 1, 2, 3}, 5, {2, 3}},
        {"三个相同值（任一对都合法）", {5, 5, 5}, 10, {5, 5}},
        {"答案贴在下标 0", {7, 1, 2, 9}, 16, {7, 9}},
        {"±1e9 极值", {-1000000000, 500000000, 500000000, 1000000000}, 0, {-1000000000, 1000000000}},
        {"target 由两个大数构成", {1000000000, 999999999, 1, 2}, 1000000000, {999999999, 1}},
    };

    // 最大规模用例：n = 10^4（题目约束上限），答案落在最后两个下标
    {
        vector<int> nums;
        nums.reserve(10000);
        for (int v = 1; v <= 10000; ++v)
            nums.push_back(v); // 1..10000，19999 只能由 9999+10000 得到
        extraCases.push_back({"最大规模 n=10^4（答案在最后两个）", nums, 19999, {9999, 10000}});
    }

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

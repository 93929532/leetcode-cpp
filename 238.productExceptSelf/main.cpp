#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   先把答案写成一个公式：answer[i] = (nums[0..i-1] 的积) × (nums[i+1..n-1] 的积)
        //   也就是「左侧所有数」乘「右侧所有数」。于是本题只有一个难点：
        //   怎么在**不重新扫一遍**的前提下，对每个 i 都拿到左边和右边的积。
        //
        //   思路一 · 前缀积 + 后缀积两个数组（最好想，O(n) 时间 / O(n) 额外空间）
        //     prefix[i] = nums[0..i-1] 的积，suffix[i] = nums[i+1..n-1] 的积，
        //     最后 answer[i] = prefix[i] * suffix[i]。
        //     先把这个写出来保底，再想怎么省掉一个数组。
        //
        //   思路二 · 正向填左积 + 反向乘右积（最优，O(n) 时间 / O(1) 额外空间）
        //     第一遍从左往右：answer[i] 先只存「左侧积」（answer[0] = 1）；
        //     第二遍从右往左：用一个变量 right 维护「右侧积」（初值 1），
        //        answer[i] *= right;   // 先把右积乘进去
        //        right    *= nums[i];  // 再把 nums[i] 并入右积
        //     两遍结束，answer 就是最终答案。
        //     关键点：**这两行的顺序不能反**。先 right *= nums[i] 的话，
        //             nums[i] 会把自己也乘进去，答案全错（而且不报错）。
        //
        //   思路三 · 先算总积再除（面试里可以当"先说出来的朴素解"，但有前提）
        //     answer[i] = total / nums[i]。问题是遇到 0 就崩，必须分情况：
        //       0 个零 -> 直接除；1 个零 -> 只有零那一位置非零，其余全 0；≥2 个零 -> 全 0。
        //     而且本题的 follow-up 原文就是「不能用除法」，所以这条路只能当过渡。
        //
        //   六个坑：
        //     1. **边界是 1 不是 0**：answer[0] 的左侧积、answer[n-1] 的右侧积都是「空积」，
        //        空积的幺元是 1。写成 0 会全军覆没。这是本题最常见的错法。
        //     2. **反向遍历时"先乘后更新"**：见思路二的关键点，顺序反了不报错但全错。
        //     3. **0 不需要特判**：前缀积/后缀积方案天然处理 0（含两个 0 时全为 0），
        //        这正是它比"总积除法"强的地方 —— 面试里值得主动说出来。
        //     4. **溢出**：题目保证「任意前缀或后缀的积都在 32 位 int 范围内」，
        //        所以在约束内 int 就够；但用 long long 做中间量更稳，
        //        只是别忘了最后转回 vector<int>。
        //     5. **负数与符号**：结果可能为负，全程用有符号类型，别顺手写 unsigned。
        //     6. **「O(1) 额外空间」怎么算**：力扣语境下**输出数组不计入**空间复杂度，
        //        所以思路二算 O(1)。面试里最好主动说明这句，面试官常常会追问。
        //
        //   进阶（这题是「左右各扫一遍」这个套路的模板）：
        //     · 同族：135 分发糖果（左右各扫一遍取 max）、42 接雨水（左右最大值）、
        //       53 最大子数组和 / 560 和为 K 的子数组（前缀和）
        //     · 你计划里序号 14 的「42 接雨水」和序号 15 的「121 买卖股票」都是这个家族的亲戚
        //     · 换个问法：如果要求「除了自身以外的**和**」就是前缀和，套路完全一样
        //     · 如果要求支持**在线修改**某个元素再查询，那就得换成前缀积 + 逆元（或线段树）

        // ↓↓↓ 占位返回：先让文件编译通过（现在跑起来用例全红）。
        //     按上面的思路写完实现后，把下面这两行删掉、换成你的 return。
        
        vector<int> cntl = nums, cntr = nums;
        
        for(size_t left = 1; left < cntl.size(); left++) 
            cntl[left] = cntl[left] * cntl[left - 1];
        
        for(size_t right = cntr.size() - 2; right >= 0; right--) 
            cntr[right] = cntr[right] * cntr[right + 1];
        
        for(size_t n = 0; n < nums.size(); n++) {
            if(n == 0)
                nums[n] = cntr[n + 1];
            else if(n == nums.size() - 1)
                nums[n] = cntl[n - 1];
            else
                nums[n] = cntl[n - 1] * cntr[n + 1];
        }

        return nums;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [24,12,8,6] 便于打印；过长的数组只打印首 6 末 3，避免刷屏
static string vectorToString(const vector<int>& v) {
    if (v.size() <= 12) {
        string s = "[";
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) s += ",";
            s += to_string(v[i]);
        }
        return s + "]";
    }

    string s = "[";
    for (int i = 0; i < 6; ++i) {
        if (i) s += ",";
        s += to_string(v[i]);
    }
    s += ", …(略去 " + to_string(v.size() - 9) + " 个)…, ";
    for (size_t i = v.size() - 3; i < v.size(); ++i) {
        if (i != v.size() - 3) s += ",";
        s += to_string(v[i]);
    }
    return s + "]";
}

// 单条测试用例
struct TestCase {
    string name;          // 用例名称
    vector<int> nums;     // 输入数组
    vector<int> expected; // 期望结果
};

// 运行一条用例，打印一行信息，返回该用例是否通过
//
// 注意：本题的输出是「按下标一一对应」的，顺序有意义，
//       所以直接比较，不能像「前 K 个高频元素」那样先排序再比。
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    vector<int> actual = sol.productExceptSelf(nums);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << vectorToString(c.nums) << "\n"
         << "       期望: " << vectorToString(c.expected) << "\n"
         << "       实际: " << vectorToString(actual) << endl;

    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 2, 3, 4}, {24, 12, 8, 6}},
        {"示例 2", {-1, 1, 0, -3, 3}, {0, 0, 9, 0, 0}},
    };

    // 额外的边界与补充用例
    // 注意：题目约束 2 <= nums.length，所以没有「只有一个元素」的用例
    vector<TestCase> extraCases = {
        {"两个元素",              {2, 3}, {3, 2}},
        {"含一个 0（除法法会崩）", {0, 1, 2}, {2, 0, 0}},
        {"含两个 0 且不全是 0",   {-1, 0, -3, 0}, {0, 0, 0, 0}},
        {"三个 0 加一个非 0",     {0, 0, 0, 7}, {0, 0, 0, 0}},
        {"偶数个负数",            {-1, -2, -3, -4}, {-24, -12, -8, -6}},
        {"奇数个负数",            {-1, 2, -3}, {-6, 3, -2}},
        {"单个 -1 在开头",        {-1, 1, 1}, {1, -1, -1}},
        {"符号交替（答案与输入不同）", {-1, 1, -1, 1, -1}, {1, -1, 1, -1, 1}},
        {"含 1 的乘积不变性",     {1, 2, 3, 4, 5}, {120, 60, 40, 30, 24}},
    };

    // 大数值用例：30 个 2
    //   每个答案 = 2^29 = 536870912（在 int 范围内），
    //   而任意前缀积最大是 2^30 = 1073741824，也没超 int —— 符合题目给出的保证
    {
        vector<int> nums(30, 2);
        vector<int> expected(30, 536870912);
        extraCases.push_back({"30 个 2（接近 int 上限）", nums, expected});
    }

    // 最大规模用例：n = 100000
    //   99999 个 1 加一个 -1：除 -1 自身外每个答案都是 -1，-1 位置是 1
    //   乘积恒为 ±1，不会溢出，专门测 O(n) 的时间与下标边界
    {
        vector<int> nums(100000, 1);
        nums.back() = -1;
        vector<int> expected(100000, -1);
        expected.back() = 1;
        extraCases.push_back({"n = 100000（99999 个 1 + 1 个 -1）", nums, expected});
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

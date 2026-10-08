#include <iostream>
#include <string>
#include <vector>
#include <algorithm>      // sort：解法与测试骨架的归一化都要用
#include <unordered_map>  // 提示一/二/三都要用：统计频率
#include <queue>          // 提示二用：priority_queue
#include <functional>     // 提示二用：greater<>

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   第一步没什么争议：先用哈希表把每个数的出现次数数出来（O(n)），
        //   拿到「值 -> 频率」这张表之后，问题就变成「按频率取前 k 个」。
        //   三种拿法，复杂度和面试评价依次升高：
        //
        //   思路一 · 频率表 + 全排序（最好写，O(n log n)）
        //     把 (值, 频率) 全部丢进 vector，按频率降序 sort，取前 k 个。
        //     面试里能过，但会被追问「能不能不排全部」。
        //
        //   思路二 · 频率表 + 小顶堆（本题标准解，O(n log k)）
        //     只维护一个大小为 k 的最小堆：堆里放「频率最小的那个在最上面」，
        //     新元素频率比堆顶大就把堆顶挤掉。遍历完，堆里就是前 k 高频。
        //     关键：priority_queue 默认是「大顶堆」，要做小顶堆得自己指定比较器，
        //          或者干脆把频率存成 -freq 绕过去（能跑，但别在面试里这么写）。
        //
        //   思路三 · 频率表 + 桶排序（O(n)，面试加分项）
        //     频率的取值范围是 1..n，天然可以当下标：开 n+1 个桶，
        //     buckets[f] 里装所有出现 f 次的数；再从最大的桶往下取，取满 k 个为止。
        //     比思路二更快，而且不需要堆。
        //
        //   四个坑：
        //     1. priority_queue 的默认比较器是 less，出来的**是**大顶堆；
        //        想留高频就得用小顶堆（greater），别把方向搞反。
        //     2. 堆里存 pair<int,int> 时，比较先看 first 再看 second ——
        //        pair<频率, 值> 和 pair<值, 频率> 的堆序完全不同，写错不会编译报错，
        //        只会静默给错答案。
        //     3. 桶排序的桶数量由 nums.size() 决定（频率最大可以到 n），
        //        不是由「不同元素的个数」决定；开小了会越界。
        //     4. 输出顺序是任意的（力扣原文：可以按任意顺序返回答案），
        //        但「前 k 高频」这个集合是唯一的（题目保证无并列歧义），
        //        本地骨架会把顺序归一化后再比，所以别为了对齐顺序去硬凑。
        //
        //   进阶（本题的两种后续形态）：
        //     · 数据是流式的、边来边问 -> 「703 数据流中的第 K 大元素」的堆维护
        //     · 不要堆、直接按频率分桶的思想 -> 「451 根据字符出现频率排序」
        //     · 同主题下一题：「215 数组中的第 K 个最大元素」（堆 vs 快速选择）
        //
        //   想热身的话可以先回忆 242 的「26 字母定长计数数组」——那是固定字符集下的
        //   频率统计；本题的字符集是 int，范围不固定，所以这里用 unordered_map。

        unordered_map<int, int> numMap;

        for(int n : nums) {
            numMap[n]++;
        }

        vector<pair<int, int>> freqVec;
        for(auto& [key, value] : numMap) {
            freqVec.push_back({value, key});
        }
        
        sort(freqVec.begin(), freqVec.end(), greater<pair<int, int>>());
        freqVec.resize(k);

        vector<int> res;
        for(auto& [freq, val] : freqVec) {
            res.push_back(val);
        }

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 归一化：把数组排序。力扣允许按任意顺序返回答案，
// 所以必须把「顺序无关」这一层剥掉再比较，否则会把正确解判成失败。
static vector<int> normalize(vector<int> v) {
    sort(v.begin(), v.end());
    return v;
}

// 把数组格式化成 [1,2] 便于打印
static string vectorToString(const vector<int>& nums) {
    string s = "[";
    for (size_t i = 0; i < nums.size(); ++i) {
        if (i > 0) s += ",";
        s += to_string(nums[i]);
    }
    s += "]";
    return s;
}

// 单条测试用例
struct TestCase {
    string name;          // 用例名称
    vector<int> nums;     // 输入数组
    int k;                // 取前 k 高频
    vector<int> expected; // 期望结果（顺序任意）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 复制一份，避免解法改动输入影响结果打印

    Solution sol;
    vector<int> actual = sol.topKFrequent(nums, c.k);

    bool pass = (normalize(actual) == normalize(c.expected));

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << vectorToString(c.nums)
         << "  k = " << c.k
         << "  期望: " << vectorToString(normalize(c.expected))
         << "  实际: " << vectorToString(normalize(actual))
         << endl;

    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {1, 1, 1, 2, 2, 3}, 2, {1, 2}},
        {"示例 2", {1}, 1, {1}},
    };

    // 额外的边界与补充用例
    // 注意：题目保证「前 k 高频的集合唯一」，所以这里不设频率并列到会改变答案集合的用例
    //      （例如 {1,1,2,2,3,3} 取 k=2，答案不唯一，属于题目保证不会出现的输入）
    const vector<TestCase> extraCases = {
        {"两个元素各一次",     {1, 2}, 2, {1, 2}},
        {"全部元素相同",       {1, 1, 1, 1}, 1, {1}},
        {"每个元素各不同",     {1, 2, 3, 4}, 4, {1, 2, 3, 4}},
        {"两个高频并列",       {1, 1, 2, 2, 3}, 2, {1, 2}},
        {"k 取满所有不同元素", {4, 4, 5, 5, 6}, 3, {4, 5, 6}},
        {"含负数",             {-1, -1, 2, 2, 2, 3}, 1, {2}},
        {"只取最高的一个",     {5, 5, 5, 1, 1, 2}, 1, {5}},
        {"含大数值",           {100000, -100000, 100000}, 1, {100000}},
        {"不同元素多于 k",     {1, 1, 1, 2, 2, 3, 3, 3, 3}, 1, {3}},
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

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>      // 思路一用：sort
#include <unordered_set>  // 思路二用：哈希集合
#include <unordered_map>  // 思路三用：哈希表 + 端点长度

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    int longestConsecutive(vector<int>& nums) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   核心矛盾一句话：题目要求 **O(n)**，而"先排序再数连续段"是 O(n log n)
        //   —— 排序这条路被堵死了。你必须用哈希集合做 O(1) 的「某个数存不存在」查询。
        //
        //   思路一 · 排序后扫描（最直观，但 O(n log n)；先说出来当过渡）
        //     先 sort，然后从左到右数连续段。注意相等元素要跳过（去重），
        //     否则 [1,1,2] 会被数成 3。面试里可以先用它对齐题意，再回答"能不能 O(n)"。
        //
        //   思路二 · 哈希集合 + 只从「序列起点」扩展（标准解，O(n) 均摊）
        //     把 nums 全塞进 unordered_set<int>，然后遍历每个数 x：
        //         if (s.count(x - 1)) continue;          // ★ x 不是起点，跳过
        //         int cur = x, len = 1;
        //         while (s.count(cur + 1)) { ++cur; ++len; }
        //         best = max(best, len);
        //     为什么是 O(n)：外层对每个数做一次 count；内层 while 只会从「起点」出发，
        //     而每个元素最多被内层访问一次（它只属于一条连续段，那条段只有一个起点）。
        //     所以总的哈希查询次数 ≤ 2n。
        //
        //   思路三 · 哈希表 + 端点长度（并查集味道，更炫但更容易写错）
        //     遍历 x（已处理过就跳过）：left = map[x-1]（不存在算 0），
        //     right = map[x+1]（不存在算 0），len = left + right + 1；
        //     然后 map[x] = len，并且把**两个端点**的值也改成 len：
        //         map[x - left] = len;   map[x + right] = len;
        //     每个点 O(1)，总 O(n)。难点是"为什么只更新端点就够"——
        //     因为中间的点以后再也不会被查询到，只有端点的邻居会被查。
        //
        //   七个坑：
        //     1. **忘了「只从起点扩展」这个判断 → 退化成 O(n²)**。代码看起来完全正确、
        //        小数据也全过，但 10^5 会超时。这是本题最容易踩、也最该在面试里
        //        主动说出来的点（"我只从 x-1 不存在的数开始数，所以是 O(n)"）。
        //        （骨架里最后一条用例就是专门抓这个的：10 万个数的单条长序列。）
        //     2. **重复元素**：用 unordered_set 天然去重；走排序思路则必须跳过相等项。
        //     3. **空数组**：约束允许 nums.length == 0，要返回 0。别把 best 初始化成 1。
        //     4. **值域到 ±1e9**：不能开数组或 vector<bool> 当下标（2e9 个格子开不出来），
        //        这也是"必须用哈希"的另一个理由。
        //     5. **x+1 溢出**：约束内（|nums[i]| ≤ 1e9）不会发生；但若传入 INT_MAX，
        //        x+1 是有符号溢出（UB）。严格写法可以先判 x < INT_MAX，或用 long long 扩展。
        //        实测输入 {INT_MIN, INT_MAX} 时：-O0 返回 2（错）、-O2/-O3 返回 1（对）——
        //        同一份源码在不同优化等级下答案不同，这就是 UB 的典型长相。
        //     6. **别把"连续"理解成"相邻下标"**：这题与原数组的顺序完全无关，
        //        是**值**连续。所以先排序不影响答案的正确性，只影响复杂度。
        //     7. **返回长度不是序列**：题目只问最长长度，不必维护具体序列（除非被追问）。
        //
        //   进阶：
        //     · 被追问"返回那条序列本身"：扩展时记下起点和长度即可，仍是 O(n)
        //     · 「只从端点/起点扩展」就是并查集题（323 连通分量 / 684 冗余连接）
        //       里 union 之后维护 size 的同一套想法
        //     · 你已建工程里 217 也是"哈希查存在性"，区别是这题要的是「值连续」
        //       这种结构关系，而不是单纯的重复判断

        if(nums.size() == 0) return 0;
        if(nums.size() == 1) return 1;

        sort(nums.begin(),nums.end());

        int tmp = 1;
        int res = tmp;

        for(size_t left = 0, right = 1; right < nums.size(); left++, right++) {
            if(nums[left] == nums[right]) continue;
            if(nums[left] + 1 == nums[right]) tmp++;
            if(nums[left] + 1 < nums[right]) tmp = 1;

            if(tmp > res) res = tmp;
        }

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [100,4,200,1,3,2] 便于打印；过长的数组只打印首 6 末 3
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
    string name;      // 用例名称
    vector<int> nums; // 输入数组
    int expected;     // 期望的最长连续序列长度
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    vector<int> nums = c.nums;   // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    int actual = sol.longestConsecutive(nums);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  nums = " << vectorToString(c.nums)
         << "  期望: " << c.expected
         << "  实际: " << actual << endl;

    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1（1,2,3,4）", {100, 4, 200, 1, 3, 2}, 4},
        {"示例 2（0..8）",    {0, 3, 7, 2, 5, 8, 4, 6, 0, 1}, 9},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"空数组",                 {}, 0},
        {"单元素",                 {5}, 1},
        {"两个连续",               {1, 2}, 2},
        {"两个不连续",             {1, 3}, 1},
        {"全部重复",               {1, 1, 1}, 1},
        {"重复 + 连续",            {1, 1, 2, 2, 3}, 3},
        {"降序输入",               {5, 4, 3, 2, 1}, 5},
        {"全负数",                 {-3, -2, -1}, 3},
        {"跨零",                   {-1, 0, 1}, 3},
        {"中间断裂",               {1, 2, 4, 5, 6}, 3},
        {"两条等长序列取其一",     {1, 2, 3, 10, 11, 12}, 3},
        {"值域两端的大数",         {-1000000000, 1000000000}, 1},
    };

    // 最大规模用例：10 万个数的**单条**长序列（10 万 ... 1）
    //   正确答案是 100000。
    //   这条用例的真正作用：如果实现漏了「只从 x-1 不存在的起点开始扩展」这一步，
    //   每次都会往后数一遍，复杂度退化成 O(n²) —— 答案依然全对，但这里会明显卡住。
    //   也就是说：**这条用例抓的不是正确性，是复杂度。**
    {
        vector<int> nums;
        nums.reserve(100000);
        for (int v = 100000; v >= 1; --v) nums.push_back(v);
        extraCases.push_back({"最大规模 10 万（抓 O(n²) 退化）", nums, 100000});
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

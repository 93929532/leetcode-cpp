#include <iostream>
#include <string>
#include <vector>
#include <queue>     // priority_queue
#include <set>       // 自检用的参考实现
#include <algorithm> // sort
#include <random>    // 生成大规模用例
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX // ★ 阻止 <windows.h> 定义 min/max 宏
#endif
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 703. 数据流中的第 K 大元素（Kth Largest Element in a Stream）· 堆 · Easy
//
// 设计一个类：
//     KthLargest(k, nums)   用初始数据构造
//     add(val)              加入一个新值，返回当前第 k 大的元素
// 注意：是"排序后第 k 个"，**重复元素各算一个**（不是第 k 个不同的值）。
//
//   ★ 核心思路：用**大小为 k 的最小堆**维护"当前最大的 k 个数"。
//     堆顶 = 这 k 个数里最小的那个 = **第 k 大**。
//
//       初始：把所有 nums 塞进最小堆，如果超过 k 个就把最小的弹掉
//       add(val)：先 push(val)，如果堆大小超过 k 再弹掉堆顶（最小值）
//                 然后堆顶就是答案
//
//   ★★ 为什么用**最小**堆而不是最大堆（这是本题最反直觉的地方）：
//       你要的是"第 k 大"，听起来应该用最大堆；
//       但最大堆的堆顶永远是**最大**的，你拿不到第 k 个。
//       反过来：维护"最大的 k 个"，用最小堆，堆顶就是这 k 个里最小的 = 第 k 大。
//       —— **要第 k 大，就用大小为 k 的"小顶堆"**；要第 k 小，就用"大顶堆"。
//
//   骨架：
//       priority_queue<int, vector<int>, greater<int>> heap;   // ★ 最小堆
//       int K;
//
//       KthLargest(int k, vector<int>& nums) {
//           K = k;
//           for (int v : nums) {
//               heap.push(v);
//               if ((int)heap.size() > K) heap.pop();      // ★ 先 push 再判断
//           }
//       }
//       int add(int val) {
//           heap.push(val);
//           if ((int)heap.size() > K) heap.pop();
//           return heap.top();
//       }
//
//   四个坑（骨架都有对应用例）：
//     1. ★★ **用了最大堆**：堆顶是最大值，拿不到第 k 个。
//     2. ★ **先 pop 再 push**：`if (size > k) pop(); push(val);` 在堆未满时会提前删元素。
//        正确顺序是 **先 push，再判断是否超 k**。
//     3. **构造时 nums 可能不足 k 个**：此时堆大小 < k，不能再弹（弹了就把有效数据删了）。
//        用例 "k=2, nums=[0]" 就是这条。
//     4. **把重复元素当成"不同值"去重**：`[1,1,1]` 的第 2 大是 1，不是"只有一种值"。
//
//   复杂度：构造 O(n log k)；add O(log k)；空间 O(k)。
//   对比：如果每次 add 都排序一次，是 O(n log n) —— 数据量大时差距明显。
//
//   面试追问：
//     · 如果要求第 k 大且数据会删除？-> 需要 multiset / 有序容器 + 计数
//     · 如果 k 会变化？-> 每次调整堆大小，或改用"两个堆"（类似 295 中位数）
//     · 和 215 数组中的第 K 大有什么区别？-> 215 是一次性查询，可以用快速选择 O(n)；
//       本题是"流式 + 多次查询"，必须维护堆
//     · 295 数据流的中位数？-> 用一个大顶堆 + 一个小顶堆各存一半
//
//   同族题：215 数组中的第 K 个最大元素、347 前 K 个高频元素、973 最接近原点的 K 个点、
//           1046 最后一块石头的重量、295 数据流的中位数
class KthLargest
{
private:
    priority_queue<int, vector<int>, greater<int>> heap;
    int K;

public:
    KthLargest(int k, vector<int> &nums)
    {
        // TODO: 在这里写你的解法
        K = k;

        for (int v : nums)
        {
            heap.push(v);

            if ((int)heap.size() > K)
                heap.pop();
        }
    }

    int add(int val)
    {
        // TODO: 在这里写你的解法

        heap.push(val);

        if ((int)heap.size() > K)
            heap.pop();

        return heap.top();
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

static string vecToStr(const vector<int> &v, size_t limit = 16)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += to_string(v[i]);
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 项）";
    return s + "]";
}

// 参考实现：用 multiset 维护"最大的 k 个"（红黑树，和堆是完全不同的数据结构）
static vector<int> refAnswer(int k, const vector<int> &nums, const vector<int> &adds)
{
    multiset<int> s;
    for (int v : nums)
    {
        s.insert(v);
        if ((int)s.size() > k)
            s.erase(s.begin()); // 弹掉最小的
    }
    vector<int> res;
    for (int v : adds)
    {
        s.insert(v);
        if ((int)s.size() > k)
            s.erase(s.begin());
        res.push_back(*s.begin()); // 最小的那个 = 第 k 大
    }
    return res;
}

// 暴力参考：每次 add 后整体排序取第 k 大（只用于小规模自检）
static vector<int> bruteAnswer(int k, const vector<int> &nums, const vector<int> &adds)
{
    vector<int> all = nums;
    vector<int> res;
    for (int v : adds)
    {
        all.push_back(v);
        vector<int> t = all;
        sort(t.begin(), t.end());
        res.push_back(t[(int)t.size() - k]);
    }
    return res;
}

struct TestCase
{
    string name;
    int k;
    vector<int> nums;
    vector<int> adds;     // 依次调用 add 的值
    vector<int> expected; // 与 adds 一一对应
};

static bool checkInput(const TestCase &c, string &why)
{
    if (c.k < 1 || c.k > 10000)
    {
        why = "k 超出 [1, 10^4]";
        return false;
    }
    if ((int)c.nums.size() > 10000 || (int)c.adds.size() > 10000)
    {
        why = "数据量超出 10^4";
        return false;
    }
    for (int v : c.nums)
        if (v < -10000 || v > 10000)
        {
            why = "nums 里的值超出 [-10^4, 10^4]";
            return false;
        }
    for (int v : c.adds)
        if (v < -10000 || v > 10000)
        {
            why = "add 的值超出 [-10^4, 10^4]";
            return false;
        }
    if (c.adds.size() != c.expected.size())
    {
        why = "adds 与 expected 长度不一致";
        return false;
    }
    // 题目保证：每次 add 调用时，流里至少已有 k 个元素
    for (size_t i = 0; i < c.adds.size(); ++i)
        if ((int)(c.nums.size() + i + 1) < c.k)
        {
            why = "第 " + to_string(i + 1) + " 次 add 时元素不足 k 个";
            return false;
        }
    return true;
}

static bool runCase(const TestCase &c)
{
    string why;
    if (!checkInput(c, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    // 数据自检：参考实现必须与手写期望一致
    {
        vector<int> ref = refAnswer(c.k, c.nums, c.adds);
        if (ref != c.expected)
        {
            cout << "[用例数据错误] " << c.name << "\n"
                 << "       multiset 参考 = " << vecToStr(ref) << "\n"
                 << "       用例写的      = " << vecToStr(c.expected) << endl;
            return false;
        }
        if (c.adds.size() <= 500)
        {
            vector<int> brute = bruteAnswer(c.k, c.nums, c.adds);
            if (brute != c.expected)
            {
                cout << "[用例数据错误] " << c.name << "\n"
                     << "       每次排序暴力 = " << vecToStr(brute) << "\n"
                     << "       用例写的     = " << vecToStr(c.expected) << endl;
                return false;
            }
        }
    }

    vector<int> nums = c.nums;
    vector<int> got;
    bool threw = false;
    string err;
    try
    {
        KthLargest obj(c.k, nums);
        for (int v : c.adds)
            got.push_back(obj.add(v));
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    bool pass = (!threw) && (got == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "  k = " << c.k << "\n"
         << "       初始 nums = " << vecToStr(c.nums) << "\n"
         << "       adds      = " << vecToStr(c.adds) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       期望 = " << vecToStr(c.expected) << "\n"
             << "       实际 = " << vecToStr(got) << "\n";
    }

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例（k=3, nums=[4,5,8,2]）",
         3,
         {4, 5, 8, 2},
         {3, 5, 10, 9, 4},
         {4, 5, 5, 8, 8}},
    };

    vector<TestCase> extraCases = {
        // k = 1：只要最大值
        {"k=1 从空开始", 1, {}, {-3, 1, -1}, {-3, 1, 1}},
        {"k=1 递增流", 1, {}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}},
        {"k=1 递减流", 1, {}, {5, 4, 3, 2, 1}, {5, 5, 5, 5, 5}},

        // ★ nums 不足 k 个：构造时不能提前弹元素
        {"nums 只有 1 个但 k=2", 2, {0}, {1, -1, 2}, {0, 0, 1}},

        // 重复元素：重复值各算一个（不是 distinct）
        {"重复元素 [1,1,1], k=2", 2, {1, 1, 1}, {1}, {1}},
        {"全相同 k=3", 3, {7, 7, 7, 7}, {7}, {7}},

        // 负数
        {"负数 k=2", 2, {-5, -3}, {-4, -10}, {-4, -4}},

        // k 正好等于初始数量
        {"k=4 初始正好 4 个", 4, {4, 5, 8, 2}, {3}, {3}},

        // 新值都比现有大 / 都小
        {"add 的值都比现有大", 2, {1, 2}, {100, 200}, {2, 100}},
        {"add 的值都比现有小", 2, {10, 20}, {-5, -1}, {10, 10}},

        // 前后交错
        {"交错大小", 3, {5, 1, 9}, {2, 9, 1, 10, 0}, {2, 5, 5, 9, 9}},
    };

    // 生成用例：k = 1000，初始 1000 个随机值，再 add 5000 个（性能 + 正确性）
    {
        mt19937_64 rng(20251125);
        TestCase c;
        c.name = "k=1000，初始 1000 个 + add 5000 次";
        c.k = 1000;
        for (int i = 0; i < 1000; ++i)
            c.nums.push_back((int)(rng() % 20001) - 10000);
        for (int i = 0; i < 5000; ++i)
            c.adds.push_back((int)(rng() % 20001) - 10000);
        c.expected = refAnswer(c.k, c.nums, c.adds);
        extraCases.push_back(c);
    }

    // 生成用例：k = 10000，初始 10000 个，add 10000 次（题目上限）
    {
        mt19937_64 rng(20251126);
        TestCase c;
        c.name = "上限：k=10000，初始 10000 个 + add 10000 次";
        c.k = 10000;
        for (int i = 0; i < 10000; ++i)
            c.nums.push_back((int)(rng() % 20001) - 10000);
        for (int i = 0; i < 10000; ++i)
            c.adds.push_back((int)(rng() % 20001) - 10000);
        c.expected = refAnswer(c.k, c.nums, c.adds);
        extraCases.push_back(c);
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

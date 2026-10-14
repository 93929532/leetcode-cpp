#include <iostream>
#include <string>
#include <vector>
#include <stack>      // 单调栈用它

#ifdef _WIN32
#include <windows.h>  // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution
{
public:
    vector<int> dailyTemperatures(vector<int> &temperatures)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 35 分钟以上再回头看）：
        //
        //   这题是 239（单调队列）的**兄弟题**：容器从 deque 换成 stack，
        //   问的东西从「窗口内的最大值」变成「右边第一个更大的元素」。
        //
        //   核心动作和 239 几乎一模一样，只是方向反过来：
        //       ① 弹栈顶：while (!st.empty() && temperatures[st.top()] < temperatures[i])
        //            —— 栈顶那一天终于等到更暖的日子了，它的答案就是 i - 栈顶下标。
        //       ② 入栈：st.push(i);
        //   扫描结束后还留在栈里的下标，就是「后面再也没有更暖的日子」→ 答案保持 0。
        //
        //   四个坑：
        //     1. **栈里存下标，不是温度**。存了温度就没法算「等了多少天」。
        //     2. **比较用 `<`，不能用 `<=`**：题目要的是**更暖**（严格大于），
        //        相等不算。用 `<=` 会在"连续相同温度"的平台上给出错误答案。
        //     3. **答案是 `i - j`（相差几天）**，不是温度差。
        //     4. **答案数组要初始化为 0**：能留在栈里的那些天，答案本来就是 0。
        //
        //   为什么是 O(n)：每个下标最多入栈一次、出栈一次，总操作次数 2n。
        //   （对照：暴力对每一天往后扫，最坏 O(n²) —— 见下面的 10^5 用例。）

        stack<int> st;
        vector<int> res(temperatures.size(), 0);

        for(int i = 0; i < (int)temperatures.size(); i++)
        {
            while(!st.empty() && temperatures[st.top()] < temperatures[i])
            {
                res[st.top()] = i - st.top();
                st.pop();
            }
            
            st.push(i);
        }

        return res;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把数组格式化成 [1,1,4,2,1,1,0,0] 便于打印；过长的数组只打印首 6 末 3
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
// 注意：本题输出与输入**按下标一一对应**（第 i 个输出是第 i 天的等待天数），
//       顺序有意义，所以直接精确比较，不能排序后比。
struct TestCase
{
    string name;           // 用例名称
    vector<int> temps;     // 输入：每日温度
    vector<int> expected;  // 期望输出（长度应与输入相同）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<int> temps = c.temps; // 解法收的是非 const 引用，复制一份给它

    Solution sol;
    vector<int> actual = sol.dailyTemperatures(temps);

    bool pass = (actual == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name << "\n"
         << "       温度: " << vectorToString(c.temps) << "\n"
         << "       期望: " << vectorToString(c.expected) << "\n"
         << "       实际: " << vectorToString(actual) << endl;

    if (!pass && actual.size() != c.expected.size())
    {
        cout << "       （输出个数是 " << actual.size()
             << "，应为 " << c.expected.size() << " = 输入长度）" << endl;
    }
    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {73, 74, 75, 71, 69, 72, 76, 73}, {1, 1, 4, 2, 1, 1, 0, 0}},
        {"示例 2（递增）", {30, 40, 50, 60}, {1, 1, 1, 0}},
        {"示例 3", {30, 60, 90}, {1, 1, 0}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"只有一天", {55}, {0}},
        {"两天且升温", {30, 100}, {1, 0}},
        {"两天且降温", {100, 30}, {0, 0}},
        {"严格递减（永远等不到）", {90, 80, 70, 60}, {0, 0, 0, 0}},
        {"全部相同 - 相等不算变暖", {70, 70, 70, 70}, {0, 0, 0, 0}},
        {"先降后升", {70, 60, 50, 80}, {3, 2, 1, 0}},
        {"平台之后才升温", {50, 50, 50, 60}, {3, 2, 1, 0}},
        {"最低温度边界 30", {30, 30, 31}, {2, 1, 0}},
        {"最高温度边界 100", {99, 100, 100}, {1, 0, 0}},
        {"锯齿形", {73, 72, 74, 71, 75}, {2, 1, 2, 1, 0}},
        {"中间隔很多天才升温", {30, 31, 30, 30, 30, 32}, {1, 4, 3, 2, 1, 0}},
        {"答案全是 1（每个都比前一个暖）", {30, 31, 32, 33, 34}, {1, 1, 1, 1, 0}},
    };

    // 最大规模用例 1：n = 10^5，全部同温
    //   没有任何一天能等到更暖的日子 -> 答案全 0。
    //   这条专门压 O(n²) 的暴力：它每天都得往后扫到底，约 5×10^9 次比较。
    //   实测（本机 -O2）：暴力约 1012 ms，单调栈约 0.28 ms。
    //   也就是说暴力处在判题时限边缘 —— 判题机慢一点、或多几组数据就会超时。
    {
        const int n = 100000;
        vector<int> temps(n, 50);
        vector<int> expected(n, 0);
        extraCases.push_back({"最大规模 n=10^5（全部同温，压 O(n²)）", temps, expected});
    }

    // 最大规模用例 2：n = 10^5，前一半 30 度、后一半 100 度
    //   前半段每一天的答案都是「到第 50000 天的距离」（最大到 50000），后半段全 0。
    //   这条同时验证：答案可以很大、且不能只处理「答案是 0/1」的情形。
    //   实测：暴力约 504 ms，单调栈约 0.22 ms。
    {
        const int n = 100000, half = n / 2;
        vector<int> temps(n, 30);
        for (int i = half; i < n; ++i)
            temps[i] = 100;

        vector<int> expected(n, 0);
        for (int i = 0; i < half; ++i)
            expected[i] = half - i;

        extraCases.push_back({"最大规模 n=10^5（前半 30 度、后半 100 度）", temps, expected});
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

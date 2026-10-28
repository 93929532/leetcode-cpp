#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> // 解法大概率会用到（用 map 也行）
#include <stdexcept>     // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 981. 基于时间的键值存储（Time Based Key-Value Store）· 设计题 + 二分查找 · Medium
//
// 这是第三个设计题（前两个：155 最小栈、208 前缀树），但它和二分绑在一起，
// 而且用的正是你还没练过的那一个模板：**找"最后一个满足条件的元素"（模板 B）**。
//
//   三个方法里 set 很简单，难点全在 get：
//       返回「所有 timestamp_prev <= timestamp 的记录中，timestamp_prev 最大的那条 value」
//       找不到就返回**空字符串 ""**（不是 -1、不是 null）
//
//   ★ 关键前提（题目保证，一定要看见）：**set 的 timestamp 严格递增**。
//     所以对同一个 key，你 push_back 进去的序列天然是**有序的** —— 这就是能二分的基础。
//     有了这个前提，你不需要任何排序，只需要"追加 + 二分"。
//
//   数据结构（一句话）：
//       unordered_map<string, vector<pair<int, string>>> mp;
//       mp[key] 里按时间顺序追加 (timestamp, value)
//
//   set(key, value, timestamp):
//       mp[key].push_back({timestamp, value});          // O(1)
//
//   get(key, timestamp):  在 mp[key] 里找「最后一个 timestamp <= 目标」的下标
//       // 模板 B：闭区间 + 记录答案，比维护不变量更好写
//       auto it = mp.find(key);
//       if (it == mp.end()) return "";
//       const auto& v = it->second;
//       int lo = 0, hi = (int)v.size() - 1, ans = -1;
//       while (lo <= hi) {
//           int mid = lo + (hi - lo) / 2;
//           if (v[mid].first <= timestamp) { ans = mid; lo = mid + 1; }  // 够小：记下，再往右试
//           else                            hi = mid - 1;                // 太大：往左
//       }
//       return ans == -1 ? "" : v[ans].second;
//
//   为什么这个模板值得单独记：
//     模板 A（153）是"找第一个满足条件的"，收缩时用 hi = mid；
//     模板 B（本题）是"找最后一个满足条件的"，所以要**先记录再往右试**。
//     两者的手感完全相反，混用就会出现"差一格"的错。
//
//   也可以用 STL（面试可以先说思路，再写手写版）：
//       auto it2 = upper_bound(v.begin(), v.end(), timestamp,
//                              [](int t, const pair<int,string>& p) { return t < p.first; });
//       return it2 == v.begin() ? "" : (it2 - 1)->second;
//   upper_bound 返回第一个 timestamp > target 的位置，往前一格就是答案。
//   注意别直接 upper_bound(v.begin(), v.end(), make_pair(timestamp, ""))：
//   那样会把 second 也拉进比较，容易踩到意料之外的结果。传"只比较 first"的比较器最稳。
//
//   四个坑：
//     1. **key 不存在**：`mp[key]` 会静默插入一个空 vector（operator[] 的副作用）。
//        读的时候用 find() 更干净；就算用了 operator[]，也要意识到它在改容器。
//     2. **返回空串 ""**，不是 "null"、不是 -1。
//     3. **目标时间戳比所有记录都小** → ans 保持 -1 → 返回 ""。
//     4. **不要每次 get 都排序**：set 已保证有序，排序等于白付 O(k log k)。
//
//   复杂度：set O(1) 摊还；get O(log k)（k = 该 key 的记录数）；空间 O(总记录数)。
//   调用上限 2×10^5 次，get 也就十几轮比较，完全够。
//
//   面试追问：
//     · timestamp 不保证递增怎么办？-> 改成 map<int,string>（本身有序，直接 upper_bound），
//       或者插入时二分定位插入位置
//     · 能不能 get O(1)？-> 需要 (key, timestamp) 的哈希，但时间戳空间太大，不现实；
//       工程里就是 O(log n)，或者加"最近访问缓存"
//     · 和 33 / 153 的关系？-> 那两题在"下标空间"二分，这题在"时间戳空间"二分，
//       模板一模一样：**找最后一个 <= x 的元素**
//
//   同族题：155 最小栈、208 前缀树（另外两个设计题）、
//           1146 快照数组（同型：snapshot + 二分）、
//           677 键值映射、1804 实现 Trie II、1032 字符流（设计 + 匹配）
class TimeMap
{
private:
    unordered_map<string, vector<pair<string, int>>> mp;

public:
    TimeMap() {}

    void set(string key, string value, int timestamp)
    {
        mp[key].push_back({value, timestamp});
    }

    string get(string key, int timestamp)
    {
        if (mp.find(key) == mp.end())
            return "";

        const vector<pair<string, int>>& tmp = mp.find(key)->second;

        int lo = -1, hi = tmp.size();

        while (lo + 1 < hi)
        {
            int mid = lo + (hi - lo) / 2;

            if (tmp[mid].second <= timestamp)
                lo = mid;
            else
                hi = mid;
        }

        return lo == -1 ? "" : tmp[lo].first;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例：照搬力扣设计题的判题格式 ——
//   ops      操作序列，第一个必须是 "TimeMap"（构造）
//   args     与 ops 一一对应，每个元素是"这一次调用的参数表"（无参数就是空 vector）
//   expected 与 ops 一一对应：无返回值写 "null"；get 返回空串就写 ""（空字符串）
struct TestCase
{
    string name;
    vector<string> ops;
    vector<vector<string>> args;
    vector<string> expected;
};

// 把一次调用的参数拼成 (a,b,c)，空参数显示为 ()
static string fmtArgs(const vector<string> &a)
{
    string s = "(";
    for (size_t i = 0; i < a.size(); ++i)
    {
        if (i)
            s += ",";
        s += a[i];
    }
    return s + ")";
}

// 把并列的多个结果格式化成 [..]，空字符串显示为 ""
static string joinResults(const vector<string> &v, size_t limit = 20)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += v[i].empty() ? "\"\"" : v[i];
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 项）";
    return s + "]";
}

// 把「第 i 步做什么」打印出来，过长时截断
static string stepAt(const TestCase &c, size_t i)
{
    return c.ops[i] + fmtArgs(c.args[i]);
}

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<string> got;

    try
    {
        TimeMap *obj = nullptr;
        for (size_t i = 0; i < c.ops.size(); ++i)
        {
            const string &op = c.ops[i];
            const vector<string> &a = (i < c.args.size()) ? c.args[i] : vector<string>();

            if (op == "TimeMap")
            {
                obj = new TimeMap();
                got.push_back("null");
            }
            else if (op == "set")
            {
                obj->set(a[0], a[1], stoi(a[2]));
                got.push_back("null");
            }
            else if (op == "get")
            {
                got.push_back(obj->get(a[0], stoi(a[1])));
            }
            else
            {
                got.push_back("(未知操作 " + op + ")");
            }
        }
        delete obj;
    }
    catch (const exception &e)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << e.what() << endl;
        return false;
    }

    bool pass = (got == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  操作数 " << c.ops.size() << "\n";
    // 打印前若干步（长用例只显示开头和结尾几个 get）
    cout << "       步骤     = ";
    size_t show = (c.ops.size() <= 12) ? c.ops.size() : 8;
    for (size_t i = 0; i < show; ++i)
    {
        if (i)
            cout << " -> ";
        cout << stepAt(c, i);
    }
    if (c.ops.size() > show)
    {
        cout << " -> …（共 " << c.ops.size() << " 步）";
        if (c.ops.size() > 4)
        {
            cout << " -> … -> ";
            for (size_t i = c.ops.size() - 4; i < c.ops.size(); ++i)
            {
                if (i > c.ops.size() - 4)
                    cout << " -> ";
                cout << stepAt(c, i);
            }
        }
    }
    cout << "\n"
         << "       期望     = " << joinResults(c.expected) << "\n"
         << "       实际     = " << joinResults(got) << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例 1",
         {"TimeMap", "set", "get", "get", "set", "get", "get"},
         {{}, {"foo", "bar", "1"}, {"foo", "1"}, {"foo", "3"}, {"foo", "bar2", "4"}, {"foo", "4"}, {"foo", "5"}},
         {"null", "null", "bar", "bar", "null", "bar2", "bar2"}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 只有一个记录：目标在它之前 / 正好 / 之后
        {"单条记录的三侧边界",
         {"TimeMap", "set", "get", "get", "get"},
         {{}, {"a", "v", "10"}, {"a", "9"}, {"a", "10"}, {"a", "11"}},
         {"null", "null", "", "v", "v"}},

        // key 不存在
        {"查询不存在的 key",
         {"TimeMap", "set", "get", "get"},
         {{}, {"a", "1", "5"}, {"b", "5"}, {"a", "5"}},
         {"null", "null", "", "1"}},

        // 多个 key 交织，且时间戳交叉 —— 验证二分只在"本 key 的列表"里做
        {"多 key 交织（时间戳交叉）",
         {"TimeMap", "set", "set", "set", "set", "set", "set",
          "get", "get", "get", "get", "get"},
         {{}, {"a", "A1", "1"}, {"b", "B2", "2"}, {"a", "A3", "3"}, {"b", "B4", "4"}, {"a", "A5", "5"}, {"b", "B6", "6"}, {"a", "4"}, {"b", "5"}, {"a", "6"}, {"a", "2"}, {"b", "1"}},
         {"null", "null", "null", "null", "null", "null", "null",
          "A3", "B4", "A5", "A1", ""}},

        // 落在两条记录之间：返回前一条
        {"落在两条记录之间取前一条",
         {"TimeMap", "set", "set", "set", "get", "get", "get", "get", "get", "get"},
         {{}, {"k", "a", "1"}, {"k", "b", "5"}, {"k", "c", "9"}, {"k", "4"}, {"k", "5"}, {"k", "6"}, {"k", "9"}, {"k", "10"}, {"k", "0"}},
         {"null", "null", "null", "null", "a", "b", "b", "c", "c", ""}},

        // 逐条命中每个时间戳 + 下限边界（时间戳上界是 10^7）
        {"逐条命中与 10^7 上界",
         {"TimeMap", "set", "set", "set", "get", "get", "get", "get", "get"},
         {{}, {"k", "x", "1"}, {"k", "y", "2"}, {"k", "z", "10000000"}, {"k", "1"}, {"k", "2"}, {"k", "10000000"}, {"k", "0"}, {"k", "9999999"}},
         {"null", "null", "null", "null", "x", "y", "z", "", "y"}},
    };

    // 大规模用例：3000 次 set + 4 次 get
    //   如果 get 写成线性扫描，这里 4 次扫描 × 3000 条也还能过；
    //   所以它主要验证"时间戳有序前提下二分边界不出错"（首条 / 末条 / 不存在的下限）
    {
        TestCase c;
        c.name = "大规模：3000 次 set + 4 次 get";
        c.ops.push_back("TimeMap");
        c.args.push_back({});
        c.expected.push_back("null");
        for (int i = 1; i <= 3000; ++i)
        {
            c.ops.push_back("set");
            c.args.push_back({"k", "v" + to_string(i), to_string(i)});
            c.expected.push_back("null");
        }
        c.ops.push_back("get");
        c.args.push_back({"k", "1500"});
        c.expected.push_back("v1500");
        c.ops.push_back("get");
        c.args.push_back({"k", "1"});
        c.expected.push_back("v1");
        c.ops.push_back("get");
        c.args.push_back({"k", "3000"});
        c.expected.push_back("v3000");
        c.ops.push_back("get");
        c.args.push_back({"k", "0"});
        c.expected.push_back("");
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

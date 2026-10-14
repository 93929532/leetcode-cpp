#include <iostream>
#include <string>
#include <vector>
#include <stack>     // 思路一/二/三都用得到
#include <algorithm> // min
#include <climits>   // INT_MIN / INT_MAX：极值用例与坑 5 相关
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 这是「设计类」题目（和 271 Codec 同型）：考点不是算法，而是**状态设计**。
// 要求 push / pop / top / getMin 四种操作全部 O(1)。
class MinStack
{
public:
    MinStack()
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   getMin 为什么难：栈只能看到栈顶，而最小值可能埋在很深的地方；
        //   更麻烦的是**弹出当前最小值之后，"上一个最小值"必须能立刻拿回来**。
        //   所以本题的核心问题只有一句：
        //       **怎么把「历史最小值」随元素一起存下来。**
        //
        //   思路一 · 主栈 + 辅助最小值栈（同步进、同步出）★推荐
        //     push(v):  data.push(v);
        //               minSt.push(minSt.empty() ? v : min(v, minSt.top()));
        //     pop():    data.pop();  minSt.pop();
        //     top():    return data.top();
        //     getMin(): return minSt.top();
        //     两个栈永远等高，minSt 的栈顶就是"当前栈内最小值"。最好写、最不易错。
        //
        //   思路二 · 只用一个栈，存 pair<值, 当前最小值>
        //     push(v):  int m = st.empty() ? v : min(v, st.top().second);
        //               st.push({v, m});
        //     getMin(): return st.top().second;
        //     代码最短，代价是每个元素多存一个 int。
        //
        //   思路三 · 辅助栈只在「更小或相等」时压（省空间，但有坑）
        //     push(v): data.push(v);
        //              if (minSt.empty() || v <= minSt.top()) minSt.push(v);
        //     pop():   if (data.top() == minSt.top()) minSt.pop();
        //              data.pop();
        //     省内存，但 **必须用 `<=` 而不是 `<`**：否则连续压入相同的数时，
        //     辅助栈只记一份，弹出多次后 getMin 就错了（下面用例 5 专门抓这个）。
        //
        //   思路四 · 只用一个栈 + 存「与当前最小值的差」（O(1) 额外空间）
        //     push(v): if (st.empty()) { minv = v; st.push(0); }
        //              else { st.push((long long)v - minv); if (v < minv) minv = v; }
        //     pop():   long long d = st.top(); st.pop();
        //              if (d < 0) minv -= d;      // 弹出的正是"当时的最小值"，要还原
        //     top():   return st.top() < 0 ? minv : (int)(minv + st.top());
        //     getMin():return minv;
        //     很巧妙，但最容易写错，而且差值必须用 long long：
        //     val 能取到 ±2^31，差值最大可达 2^32，int 装不下。
        //     面试里能讲清"为什么 d < 0 就说明弹出的是最小值"就是加分项。
        //
        //   六个坑：
        //     1. **两个栈必须同步**（思路一）：pop 时漏弹 minSt，之后的 getMin 会读到陈旧值。
        //        骨架里那条 2.5 万次操作的长用例就是专门抓它的（弹完递减段后 min 必须回到 0）。
        //     2. **思路三的 `<=`**：相等也要压。这是本题最经典的静默错误 ——
        //        弹出次数比压入次数多，最小值会提前"消失"。
        //     3. **别在 getMin() 里扫一遍栈**：那是一次 O(n) 操作，题目要 O(1)；
        //        3 万次调用 × O(n) 会超时，也违背题目本意。
        //     4. **差值法的溢出**：val - minv 超出 int，必须 long long。
        //     5. **极值**：val 可取到 ±2^31-1（INT_MIN / INT_MAX），
        //        所以别用 0 或 -1 当"空栈哨兵"去和值比较。
        //     6. **pop / top / getMin 只会在非空栈上调用**（题目保证），
        //        所以不必处理空栈；但自己心里要清楚空栈时它们是什么行为。
        //
        //   进阶：
        //     · 被追问"能不能只用一个栈"→ 思路四
        //     · 被追问"能不能 O(1) 空间且不存差值"→ 没有这样的解，说清即可
        //     · 同族：146 LRU Cache、225 用队列实现栈、232 用栈实现队列、
        //       716 最大栈（会员题）、895 最大频率栈
        //     · 你已建的 20「有效的括号」是"用栈做匹配"，这题是"维护栈的元信息"，
        //       两种用法的味道完全不同
    }

    void push(int val)
    {
        data.push(val);

        if (minSt.empty())
            minSt.push(val);
        else
            minSt.push(min(minSt.top(), val));
    }

    void pop()
    {
        data.pop();
        minSt.pop();
    }

    int top()
    {
        return data.top();
    }

    int getMin()
    {
        return minSt.top();
    }

private:
    stack<int> data;
    stack<int> minSt;
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例：完全照搬力扣的判题格式 ——
//   ops      操作序列，第一个必须是 "MinStack"（构造）
//   args     与 ops 一一对应；无参数写 ""（如 "MinStack" / "pop" / "top" / "getMin"）
//   expected 与 ops 一一对应；无返回值的操作写 "null"，有返回值的写结果的字符串
struct TestCase
{
    string name;
    vector<string> ops;
    vector<string> args;
    vector<string> expected;
};

// 把字符串列表格式化成 [a,b,c] 便于打印
static string joinList(const vector<string> &v, size_t limit = 24)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += (v[i].empty() ? "_" : v[i]);
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 项）";
    return s + "]";
}

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<string> got;

    try
    {
        MinStack st; // 第一个操作是 "MinStack"，这里直接构造
        for (size_t i = 0; i < c.ops.size(); ++i)
        {
            const string &op = c.ops[i];
            const string &arg = (i < c.args.size()) ? c.args[i] : string();

            if (op == "MinStack")
                got.push_back("null");
            else if (op == "push")
            {
                st.push(stoi(arg));
                got.push_back("null");
            }
            else if (op == "pop")
            {
                st.pop();
                got.push_back("null");
            }
            else if (op == "top")
                got.push_back(to_string(st.top()));
            else if (op == "getMin")
                got.push_back(to_string(st.getMin()));
            else
                got.push_back("(未知操作 " + op + ")");
        }
    }
    catch (const exception &e)
    {
        cout << "[失败] " << c.name << "  抛出异常: " << e.what() << endl;
        return false;
    }

    bool pass = (got == c.expected);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  操作数 " << c.ops.size() << "\n"
         << "       ops      = " << joinList(c.ops) << "\n"
         << "       args     = " << joinList(c.args) << "\n"
         << "       期望     = " << joinList(c.expected) << "\n"
         << "       实际     = " << joinList(got) << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {"MinStack", "push", "push", "push", "getMin", "pop", "top", "getMin"}, {"", "-2", "0", "-3", "", "", "", ""}, {"null", "null", "null", "null", "-3", "null", "0", "-2"}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"单元素", {"MinStack", "push", "top", "getMin"}, {"", "5", "", ""}, {"null", "null", "5", "5"}},

        {"递减压入（min 一路跟着变小）",
         {"MinStack", "push", "push", "push", "getMin", "pop", "getMin", "pop", "getMin"},
         {"", "3", "2", "1", "", "", "", "", ""},
         {"null", "null", "null", "null", "1", "null", "2", "null", "3"}},

        {"递增压入（min 一直是最底下那个）",
         {"MinStack", "push", "push", "push", "getMin", "pop", "pop", "getMin", "top"},
         {"", "1", "2", "3", "", "", "", "", ""},
         {"null", "null", "null", "null", "1", "null", "null", "1", "1"}},

        {"极值 ±2^31-1",
         {"MinStack", "push", "push", "getMin", "top", "pop", "top", "getMin"},
         {"", "-2147483648", "2147483647", "", "", "", "", ""},
         {"null", "null", "null", "-2147483648", "2147483647", "null", "-2147483648", "-2147483648"}},

        // 这条专抓「思路三用 < 而不是 <=」的 bug：连续三个 0，
        // 辅助栈若只压一份，弹出两次后 getMin 就取不到值 / 取到错值
        {"重复最小值（抓 <= 写成 <）",
         {"MinStack", "push", "push", "push", "getMin", "pop", "getMin", "pop", "getMin"},
         {"", "0", "0", "0", "", "", "", "", ""},
         {"null", "null", "null", "null", "0", "null", "0", "null", "0"}},

        {"弹出后压入更小的",
         {"MinStack", "push", "push", "pop", "push", "getMin", "top"},
         {"", "2", "5", "", "1", "", ""},
         {"null", "null", "null", "null", "null", "1", "1"}},
    };

    // 长序列压力用例：合计 25003 次操作（题目约束上限是 3×10^4）
    //   阶段 1：递增压入 0..4999，每次 getMin 都是 0
    //   阶段 2：递减压入 -1..-5000，每次 getMin 都是刚压入的那个
    //   阶段 3：把阶段 2 的全部弹掉（5000 次 pop）
    //   阶段 4：此时最小值必须**回到 0**（栈里只剩阶段 1 的元素）
    //   阶段 3→4 是抓「两个栈不同步」的关键：辅助栈漏弹的话，getMin 会停在 -5000。
    {
        TestCase big;
        big.name = "长序列压力（25003 次操作）";
        big.ops.push_back("MinStack");
        big.args.push_back("");
        big.expected.push_back("null");

        for (int v = 0; v < 5000; ++v)
        {
            big.ops.push_back("push");
            big.args.push_back(to_string(v));
            big.expected.push_back("null");
            big.ops.push_back("getMin");
            big.args.push_back("");
            big.expected.push_back("0");
        }
        for (int v = 1; v <= 5000; ++v)
        {
            big.ops.push_back("push");
            big.args.push_back(to_string(-v));
            big.expected.push_back("null");
            big.ops.push_back("getMin");
            big.args.push_back("");
            big.expected.push_back(to_string(-v));
        }
        for (int i = 0; i < 5000; ++i)
        {
            big.ops.push_back("pop");
            big.args.push_back("");
            big.expected.push_back("null");
        }
        big.ops.push_back("getMin");
        big.args.push_back("");
        big.expected.push_back("0");
        big.ops.push_back("top");
        big.args.push_back("");
        big.expected.push_back("4999");

        extraCases.push_back(big);
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

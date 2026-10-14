#include <iostream>
#include <string>
#include <vector>
#include <stdexcept> // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 设计类题目 2：和 155「最小栈」同型，但**状态的形状完全不同**。
//
//   155 的记忆：两个并排的 stack，规规矩矩的线性结构；
//   208 的记忆：**一棵树** —— 对象里存一个根节点，根节点再指向更多节点。
//
// 这正是设计题最有意思的地方：套路不变（成员变量 = 对象的记忆），
// 但"记忆长什么样"要你自己想出来。要求三个操作都是 O(L)，L = 单词长度。
class Trie
{
public:
    Trie()
    {
        // TODO: 需要初始化什么成员，在这里写（不需要就留空）
        root = new Node();
    }

    void insert(string word)
    {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   核心问题只有一句：
        //       **怎么让「公共前缀」被多个单词共享？**
        //
        //   如果每个单词单独存一份字符数组，insert("apple") 之后再 search("app")，
        //   你得把每个单词都翻一遍才知道有没有这个前缀 —— 那是 O(单词数 × 长度)。
        //   把单词**按字符拆开**，让相同的前缀走同一条路，就自然长成一棵 26 叉树，
        //   这棵树叫 Trie（前缀树 / 字典树）。
        //
        //   一个节点只存三样东西：
        //     · 若干条"出边"：'a'..'z' 各指向一个子节点（也可以是哈希表：字符 -> 子节点）
        //     · 一个 bool：**"从上到下走到这里，是否刚好是一个完整单词的结尾"**
        //     · 不需要存字符本身！字符体现在"走的是哪条边"，不体现在节点里
        //
        //   思路一 · 指针 + 定长数组（★推荐，面试标准答案）
        //     struct Node {
        //         Node* ch[26] = {nullptr};   // 'a'+0 .. 'a'+25，没走过就是 nullptr
        //         bool isEnd = false;
        //     };
        //     Node* root;                    // 类里唯一要存的成员变量
        //
        //     void insert(string w) {
        //         Node* cur = root;
        //         for (char c : w) {
        //             int i = c - 'a';
        //             if (cur->ch[i] == nullptr) cur->ch[i] = new Node();
        //             cur = cur->ch[i];
        //         }
        //         cur->isEnd = true;         // ★ 循环走完之后再打标记
        //     }
        //     把它写成"从根往下走"的循环，就顺手得到了另外两个操作：
        //         search(w)：    沿 w 逐字符往下走；中途没路 -> false；走完 return cur->isEnd
        //         startsWith(p)：沿 p 逐字符往下走；中途没路 -> false；走完 return true
        //     两个函数**只差最后一行**，其余代码一模一样。
        //
        //   思路二 · 用哈希表代替定长数组
        //     unordered_map<char, Node*> ch;
        //     只存真正出现过的边，省空间；代价是常数更大，且要自己对每个节点建表。
        //     字符集很大时（大写+小写、Unicode、任意字节）这才是正解。
        //
        //   思路三 · 二维数组模拟，全静态分配（竞赛/工程常用）
        //     const int MAXN = ...;
        //     int  trie[MAXN][26] = {0};   // 0 表示"没有这条边"，否则存子节点编号
        //     bool isEnd[MAXN]    = {false};
        //     int  tot = 1;                // 0 号节点就是根
        //
        //     insert: int p = 0;
        //             for (char c : w) { int i = c - 'a';
        //                 if (trie[p][i] == 0) trie[p][i] = tot++;
        //                 p = trie[p][i]; }
        //             isEnd[p] = true;
        //     好处：没有 new/delete、节点连续、缓存友好，也不用写析构；
        //     坏处：得先估出节点数上界（本题 3×10^4 个单词 × 2000 字符太长，
        //           但真实评测里单词总和有约束，按总和估即可）。
        //     ★ 面试时先写思路一；如果被追问"内存/性能"，再讲思路三。
        //
        //   五个坑：
        //     1. **search 和 startsWith 只差最后一行**：前者要 isEnd，后者不要。
        //        写反了示例 1 立刻挂 —— 那里 insert("apple") 之后 search("app") 必须是 false。
        //     2. **isEnd 的时机**：必须等整个单词走完再置 true，提前置的话
        //        "app" 会被误判成插入过。
        //     3. **一个单词是另一个单词的前缀**：insert("a") 和 insert("ab") 之后，
        //        search("a") 和 search("ab") 都必须是 true。
        //        所以"是不是单词结尾"**必须存在节点上**，绝不能用"有没有子节点"代替
        //        （"a" 有子节点，但它本身就是个完整单词）。
        //     4. **字符必须映射到 0..25**：写 `ch[c]` 会开出 97 号下标 -> 越界。
        //        本题只有小写字母，`c - 'a'` 就够。
        //     5. **单词长度到 2000**：别用 `char buf[100]` 之类定长缓冲；递归的话注意深度。
        //
        //   写完之后顺手想一想（面试高频追问）：
        //     · 为什么 Trie 查询是 O(L)，而哈希表存单词是 O(L) 但做不到前缀查询？
        //     · 内存开销大在哪？（26 个指针 × 节点数，大量空指针）
        //     · 怎么统计"以 p 为前缀的单词个数"？→ 在节点上再加一个计数器
        //
        //   进阶 / 同族：
        //     · 211 添加与搜索单词：search 里带 '.' 通配任意字母 -> 在节点上做 DFS 回溯
        //     · 212 单词搜索 II：Trie + 网格 DFS + 剪枝（Hard）
        //     · 1268 搜索推荐系统、648 单词替换 也都是 Trie 的变奏
        //     · 对照 155：记忆从"两个栈"变成"一棵树"，成员变量可以是
        //       **指向更多同类对象的指针** —— 这就是类与类之间的组合关系

        Node *cur = root;

        for (char c : word)
        {
            if (cur->ch[c - 'a'] == nullptr)
                cur->ch[c - 'a'] = new Node();

            cur = cur->ch[c - 'a'];
        }

        cur->isEnd = true;
    }

    bool search(string word)
    {
        Node *cur = root;

        for (char c : word)
        {
            if (cur->ch[c - 'a'] != nullptr)
                cur = cur->ch[c - 'a'];
            else
                return false;
        }

        if (cur->isEnd == false)
            return false;
        else
            return true;
    }

    bool startsWith(string prefix)
    {
        Node *cur = root;

        for (char c : prefix)
        {
            if (cur->ch[c - 'a'] != nullptr)
                cur = cur->ch[c - 'a'];
            else
                return false;
        }

        return true;
    }

private:
    struct Node
    {
        Node *ch[26] = {nullptr};
        bool isEnd = false;
    };

    Node *root;
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例：完全照搬力扣的判题格式 ——
//   ops      操作序列，第一个必须是 "Trie"（构造）
//   args     与 ops 一一对应；无参数写 ""（如 "Trie"）
//   expected 与 ops 一一对应；无返回值的操作写 "null"，
//            bool 返回值按力扣的写法记成 "true" / "false"
struct TestCase
{
    string name;
    vector<string> ops;
    vector<string> args;
    vector<string> expected;
};

// 单个字符串过长时截断后再打印（压力用例里单词有 2000 字符）
static string shorten(const string &s, size_t keep = 16)
{
    if (s.size() <= keep * 2 + 3)
        return s;
    return s.substr(0, keep) + "…(" + to_string(s.size()) + "字符)…" + s.substr(s.size() - keep);
}

// 把字符串列表格式化成 [a,b,c] 便于打印
static string joinList(const vector<string> &v, size_t limit = 24)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += (v[i].empty() ? "_" : shorten(v[i]));
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 项）";
    return s + "]";
}

// 数据自检：题目保证 word / prefix 只由小写英文字母组成。
// 万一哪条用例混进了数字或大写（比如图省事写 to_string(i)），
// 实现里的 `c - 'a'` 就会变成负数下标 —— 那是**违反约束的输入**，
// 表现是"莫名其妙的崩溃或随机错误"，很难看出是谁的锅。
// 所以这里先拦一道：数据不合法就直接报错，而不是让它去触发 UB。
static bool allLowercase(const string &s)
{
    for (char c : s)
        if (c < 'a' || c > 'z')
            return false;
    return true;
}

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<string> got;

    // 先校验用例数据本身是否满足题目约束
    for (size_t i = 0; i < c.ops.size() && i < c.args.size(); ++i)
    {
        if (c.ops[i] == "insert" || c.ops[i] == "search" || c.ops[i] == "startsWith")
        {
            if (!allLowercase(c.args[i]))
            {
                cout << "[用例数据错误] " << c.name << "  第 " << i << " 步 "
                     << c.ops[i] << " 的参数含非小写字母: " << shorten(c.args[i]) << endl;
                return false;
            }
        }
    }

    try
    {
        Trie t; // 第一个操作是 "Trie"，这里直接构造
        for (size_t i = 0; i < c.ops.size(); ++i)
        {
            const string &op = c.ops[i];
            const string &arg = (i < c.args.size()) ? c.args[i] : string();

            if (op == "Trie")
                got.push_back("null");
            else if (op == "insert")
            {
                t.insert(arg);
                got.push_back("null");
            }
            else if (op == "search")
                got.push_back(t.search(arg) ? "true" : "false");
            else if (op == "startsWith")
                got.push_back(t.startsWith(arg) ? "true" : "false");
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
        {"示例 1", {"Trie", "insert", "search", "search", "startsWith", "insert", "search"}, {"", "apple", "apple", "app", "app", "app", "app"}, {"null", "null", "true", "false", "true", "null", "true"}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 一个单词是另一个单词的前缀：靠节点上的 isEnd 区分，不能用"有没有子节点"代替
        {"单词互为前缀（a / ab）",
         {"Trie", "insert", "insert", "search", "search", "search",
          "startsWith", "startsWith", "startsWith"},
         {"", "a", "ab", "a", "ab", "abc", "a", "ab", "abc"},
         {"null", "null", "null", "true", "true", "false", "true", "true", "false"}},

        // 只走过、没结尾过：search 为 false，startsWith 为 true —— 这是本题最核心的分界
        {"只 insert 了前缀，没 insert 完整词",
         {"Trie", "insert", "search", "search", "startsWith", "startsWith", "search"},
         {"", "app", "app", "appl", "app", "appl", "a"},
         {"null", "null", "true", "false", "true", "false", "false"}},

        // 重复 insert 同一个词必须幂等（如果实现里用了"计数"之类的花活就会错）
        {"重复 insert 同一个词",
         {"Trie", "insert", "insert", "search", "startsWith", "insert", "search"},
         {"", "abc", "abc", "abc", "abc", "abcd", "abc"},
         {"null", "null", "null", "true", "true", "null", "true"}},

        // 完全走错分支
        {"走不存在的分支",
         {"Trie", "insert", "search", "search", "startsWith", "search", "startsWith", "search"},
         {"", "banana", "banana", "ban", "ban", "bananas", "b", "c"},
         {"null", "null", "true", "false", "true", "false", "true", "false"}},

        // 边界字母 a / z，以及"短词是长词前缀"的四种组合
        {"边界字母 a / z",
         {"Trie", "insert", "insert", "insert", "insert",
          "search", "search", "search", "search", "search",
          "startsWith", "startsWith", "search"},
         {"", "a", "z", "az", "za",
          "a", "z", "az", "za", "b",
          "a", "z", "az"},
         {"null", "null", "null", "null", "null",
          "true", "true", "true", "true", "false",
          "true", "true", "true"}},
    };

    // 长序列压力用例
    //   阶段 1：一个正好 2000 字符的单词（题目给的长度上界），
    //           它的 1999 字符前缀 search 必须 false、startsWith 必须 true
    //   阶段 2：500 个共享 100 字符长前缀的单词（后缀不同）
    //           —— 前缀只走一遍，抓"每个单词单独存一份"的假实现
    //   ★ 后缀必须只用小写字母：题目保证 word 只含 'a'..'z'，
    //     如果这里为了省事写成 to_string(i)（含数字），
    //     实现里 `c - 'a'` 就会变成负数下标 —— 那是**违反约束的输入**，
    //     会和 UB 缠在一起，调试时根本看不出是谁的错。
    {
        TestCase big;
        big.name = "长序列压力（2000 字符单词 + 500 个共享长前缀的单词）";
        big.ops.push_back("Trie");
        big.args.push_back("");
        big.expected.push_back("null");

        const string longWord(2000, 'a');
        big.ops.push_back("insert");
        big.args.push_back(longWord);
        big.expected.push_back("null");
        big.ops.push_back("search");
        big.args.push_back(longWord);
        big.expected.push_back("true");
        big.ops.push_back("search");
        big.args.push_back(string(1999, 'a'));
        big.expected.push_back("false");
        big.ops.push_back("startsWith");
        big.args.push_back(string(1999, 'a'));
        big.expected.push_back("true");
        big.ops.push_back("search");
        big.args.push_back(string(1999, 'a') + "b");
        big.expected.push_back("false");
        big.ops.push_back("startsWith");
        big.args.push_back(string(1999, 'a') + "b");
        big.expected.push_back("false");

        // 把 i 编成两个小写字母（i < 676 时唯一），保证整条词都是 'a'..'z'
        auto code = [](int i)
        {
            string s;
            s += char('a' + i / 26);
            s += char('a' + i % 26);
            return s;
        };

        const string prefix(100, 'x');
        for (int i = 0; i < 500; ++i)
        {
            big.ops.push_back("insert");
            big.args.push_back(prefix + code(i) + "y");
            big.expected.push_back("null");
        }
        for (int i = 0; i < 500; ++i)
        {
            big.ops.push_back("search");
            big.args.push_back(prefix + code(i) + "y");
            big.expected.push_back("true");
        }
        // 最后一条词：前缀 100 个 x + 两个字母 + 结尾 y
        const string lastWord = prefix + code(499) + "y";
        const string lastHead = prefix + code(499);        // 差最后一个字符 -> 只是前缀，不是单词
        const string lastWrong = prefix + code(499) + "z"; // 分支根本不存在

        big.ops.push_back("startsWith");
        big.args.push_back(prefix);
        big.expected.push_back("true");
        big.ops.push_back("search");
        big.args.push_back(prefix);
        big.expected.push_back("false");
        big.ops.push_back("startsWith");
        big.args.push_back(lastHead);
        big.expected.push_back("true");
        big.ops.push_back("search");
        big.args.push_back(lastHead);
        big.expected.push_back("false");
        big.ops.push_back("search");
        big.args.push_back(lastWord);
        big.expected.push_back("true");
        big.ops.push_back("search");
        big.args.push_back(lastWrong);
        big.expected.push_back("false");
        big.ops.push_back("startsWith");
        big.args.push_back(lastWrong);
        big.expected.push_back("false");

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

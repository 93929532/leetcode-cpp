#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> // Trie 的 map 版实现常用
#include <unordered_set> // 生成大规模用例时算期望值
#include <random>        // 同上
#include <stdexcept>     // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX // ★ 阻止 <windows.h> 定义 min/max 宏
#endif
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 211. 添加与搜索单词 - 数据结构设计
//      （Design Add and Search Words Data Structure）· Trie + DFS · Medium
//
// 实现一个支持两种操作的数据结构：
//     addWord(word)      加入一个单词
//     search(word)       查询是否存在匹配的单词，其中 **`'.'` 可以匹配任意单个字母**
//
//   ★ 这就是 208「实现 Trie」加了一个通配符。前缀树依然是唯一正确的底座：
//     · 单词大量共享前缀 -> Trie 共享节点，省空间
//     · `.` 需要"某一层把所有可能都试一遍" -> 天然对应 Trie 的**多个孩子** + DFS 回溯
//
//   Trie 节点（本题只有小写字母，26 叉）：
//       struct Node {
//           Node* child[26] = {nullptr};   // 26 个小写字母
//           bool  isEnd = false;           // 是否有单词在这里结束
//       };
//
//   addWord(word)：沿着字符一路下钻，走不到的节点就新建；最后一个字符处标记 isEnd。
//   search(word)：从根开始 DFS：
//       · 普通字母 c：只有 child[c-'a'] 存在才继续，否则这条分支失败；
//       · '.'      ：**对 26 个孩子逐个尝试**，任意一个成功即返回 true；
//       · 走完整个 word 时：返回当前节点的 isEnd（★ 前缀不算匹配！）。
//
//   四个坑（骨架都有对应用例）：
//     1. ★★ **忘了 isEnd**：只判断"路径存在"会把**前缀**也当成单词。
//        例如只加入 "bad"，`search("ba")` 必须是 false。
//     2. ★ **`.` 要遍历所有孩子**：只试某一个（比如 child[0]）会漏匹配；
//        而且必须**回溯**（一个分支失败后继续试下一个）。
//     3. **长度必须一致**：`search("badd")` 和 `search("ba")` 都要能正确返回 false。
//     4. **共享前缀不能各自存一份**：addWord("bad")/addWord("dad")/addWord("mad")
//        应该共享根节点（否则第一次查询就会退化成线性扫描）。
//
//   复杂度：addWord O(L)（L = 单词长度，≤ 25）；
//           search 最坏 O(26^k)，k 是 `.` 的个数 —— 但实际数据下很快，
//           因为每个字母都唯一确定分支。空间 O(总字符数)。
//
//   面试追问：
//     · 和 208 实现 Trie 的区别？-> 208 只有 insert/search/startsWith，没有通配符
//     · 如果 `.` 能匹配"任意长度"（不是单个字母）？-> 那就变成正则/通配符匹配，
//       需要回溯 + 记忆化，或者转成 AC 自动机
//     · 如果要支持前缀搜索 / 模糊搜索？-> 745 题（前缀和后缀搜索）、1032 字符流
//     · 词数很多、查询也很多怎么办？-> Trie + 失配指针（AC 自动机）一次扫描
//
//   同族题：208 实现 Trie、212 单词搜索 II（Trie + 网格 DFS）、
//           745 前缀和后缀搜索、1032 字符流、648 单词替换
class WordDictionary
{
private:
    struct wordNode
    {
        wordNode *Node[26] = {nullptr};
        bool isEnd = false;
    };

    wordNode *root;

public:
    WordDictionary()
    {
        // TODO: 在这里写你的解法

        root = new wordNode();
    }

    void addWord(string word)
    {
        // TODO: 在这里写你的解法

        wordNode *p = root;

        for (char c : word)
        {
            int num = c - 'a';

            if (p->Node[num] == nullptr)
                p->Node[num] = new wordNode();

            p = p->Node[num];
        }
        p->isEnd = true;
    }

    bool search(string word)
    {
        // TODO: 在这里写你的解法

        wordNode *p = root;

        for (int i = 0; i < word.size(); i++)
        {
            if (word[i] == '.')
            {
                for (int j = 0; j < 26; j++)
                {
                    if (p->Node[j] != nullptr)
                    {
                        if (search(word.substr(0, i) + (char)('a' + j) + word.substr(i + 1)))
                            return true;
                    }
                }
                return false;
            }

            char c = word[i];
            int num = c - 'a';

            if (p->Node[num] != nullptr)
                p = p->Node[num];
            else
                return false;
        }

        return p->isEnd;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例：照搬力扣设计题的判题格式
//   ops      操作序列，第一个必须是 "WordDictionary"（构造）
//   args     与 ops 一一对应
//   expected 与 ops 一一对应：无返回值写 "null"，search 的返回值写 "true"/"false"
struct TestCase
{
    string name;
    vector<string> ops;
    vector<vector<string>> args;
    vector<string> expected;
};

// 暴力模型：判断某个单词是否匹配带 '.' 的模式（用于生成期望值）
static bool bruteMatch(const string &word, const string &pattern)
{
    if (word.size() != pattern.size())
        return false;
    for (size_t i = 0; i < word.size(); ++i)
        if (pattern[i] != '.' && pattern[i] != word[i])
            return false;
    return true;
}

static string fmtCall(const string &op, const vector<string> &a)
{
    string s = op + "(";
    for (size_t i = 0; i < a.size(); ++i)
    {
        if (i)
            s += ",";
        s += "\"" + a[i] + "\"";
    }
    return s + ")";
}

static string joinResults(const vector<string> &v, size_t limit = 24)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += v[i];
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 项）";
    return s + "]";
}

// 数据自检：addWord 只能小写字母；search 只能小写字母或 '.'；长度 1..25；总调用 ≤ 10^4
static bool checkCase(const TestCase &c, string &why)
{
    if (c.ops.empty() || c.ops[0] != "WordDictionary")
    {
        why = "第一个操作必须是 WordDictionary";
        return false;
    }
    if ((int)c.ops.size() > 10000)
    {
        why = "操作数超过 10^4";
        return false;
    }
    for (size_t i = 0; i < c.ops.size(); ++i)
    {
        const string &op = c.ops[i];
        const vector<string> &a = c.args[i];
        if (op == "addWord" || op == "search")
        {
            if (a.size() != 1)
            {
                why = "参数个数不对：" + op;
                return false;
            }
            const string &w = a[0];
            if (w.empty() || w.size() > 25)
            {
                why = "单词长度必须在 1..25：" + w;
                return false;
            }
            for (char ch : w)
            {
                bool lower = (ch >= 'a' && ch <= 'z');
                bool dot = (op == "search" && ch == '.');
                if (!lower && !dot)
                {
                    why = "非法字符 '" + string(1, ch) + "'（" + op + "）";
                    return false;
                }
            }
        }
        else if (op != "WordDictionary")
        {
            why = "未知操作：" + op;
            return false;
        }
    }
    return true;
}

static bool runCase(const TestCase &c)
{
    string why;
    if (!checkCase(c, why))
    {
        cout << "[用例数据错误] " << c.name << "  " << why << endl;
        return false;
    }

    vector<string> got;
    try
    {
        WordDictionary *obj = nullptr;
        for (size_t i = 0; i < c.ops.size(); ++i)
        {
            const string &op = c.ops[i];
            const vector<string> &a = c.args[i];

            if (op == "WordDictionary")
            {
                obj = new WordDictionary();
                got.push_back("null");
            }
            else if (op == "addWord")
            {
                obj->addWord(a[0]);
                got.push_back("null");
            }
            else if (op == "search")
            {
                got.push_back(obj->search(a[0]) ? "true" : "false");
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
    size_t show = (c.ops.size() <= 10) ? c.ops.size() : 4;
    cout << "       前几步 = ";
    for (size_t i = 0; i < show; ++i)
    {
        if (i)
            cout << " -> ";
        cout << fmtCall(c.ops[i], c.args[i]);
    }
    if (c.ops.size() > show)
    {
        cout << " -> …（共 " << c.ops.size() << " 步）-> ";
        for (size_t i = c.ops.size() - 2; i < c.ops.size(); ++i)
        {
            if (i > c.ops.size() - 2)
                cout << " -> ";
            cout << fmtCall(c.ops[i], c.args[i]);
        }
    }
    cout << "\n       期望 = " << joinResults(c.expected) << "\n"
         << "       实际 = " << joinResults(got) << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例（加入 bad/dad/mad，查询 4 次）",
         {"WordDictionary", "addWord", "addWord", "addWord",
          "search", "search", "search", "search"},
         {{}, {"bad"}, {"dad"}, {"mad"}, {"pad"}, {"bad"}, {".ad"}, {"b.."}},
         {"null", "null", "null", "null",
          "false", "true", "true", "true"}},
    };

    // 补充用例
    vector<TestCase> extraCases = {
        // 前缀不是单词
        {"只加 bad，查前缀 ba 应为 false",
         {"WordDictionary", "addWord", "search", "search", "search"},
         {{}, {"bad"}, {"ba"}, {"bad"}, {"b"}},
         {"null", "null", "false", "true", "false"}},

        // 长度必须一致
        {"长度不一致",
         {"WordDictionary", "addWord", "search", "search", "search"},
         {{}, {"bad"}, {"badd"}, {"b"}, {"bad"}},
         {"null", "null", "false", "false", "true"}},

        // 全通配符
        {"全通配符",
         {"WordDictionary", "addWord", "search", "search"},
         {{}, {"a"}, {"."}, {".."}},
         {"null", "null", "true", "false"}},

        // 通配符在首/中/尾
        {"通配符在各位置",
         {"WordDictionary", "addWord", "search", "search", "search", "search"},
         {{}, {"hello"}, {".ello"}, {"h.llo"}, {"hell."}, {"h.ll."}},
         {"null", "null", "true", "true", "true", "true"}},

        // 多个词共享前缀
        {"共享前缀：bad/dad/mad/pad",
         {"WordDictionary", "addWord", "addWord", "addWord", "addWord",
          "search", "search", "search", "search"},
         {{}, {"bad"}, {"dad"}, {"mad"}, {"pad"}, {"bad"}, {"dad"}, {"mad"}, {"pad"}},
         {"null", "null", "null", "null", "null",
          "true", "true", "true", "true"}},

        // 只加一个词，别的都不匹配
        {"不匹配的模式",
         {"WordDictionary", "addWord", "search", "search", "search"},
         {{}, {"abc"}, {"abd"}, {"ab."}, {"..."}},
         {"null", "null", "false", "true", "true"}},

        // 重复加入同一个词
        {"重复加入同一个词",
         {"WordDictionary", "addWord", "addWord", "search"},
         {{}, {"x"}, {"x"}, {"x"}},
         {"null", "null", "null", "true"}},

        // 单个字母 + 通配符
        {"单字母",
         {"WordDictionary", "addWord", "search", "search"},
         {{}, {"a"}, {"a"}, {"."}},
         {"null", "null", "true", "true"}},

        // 长单词（25 字符，题目上限）
        {"25 字符单词",
         {"WordDictionary", "addWord", "search", "search", "search"},
         {{}, {"abcdefghijklmnopqrstuvwxy"}, {"abcdefghijklmnopqrstuvwxy"}, {"........................."}, {"abcdefghijklmnopqrstuvwx."}},
         {"null", "null", "true", "true", "true"}},

        // 通配符没有匹配（该层孩子都不存在）
        {"通配符无法匹配",
         {"WordDictionary", "addWord", "search"},
         {{}, {"aaa"}, {"..b"}},
         {"null", "null", "false"}},
    };

    // 大规模用例：4000 次 addWord + 4000 次 search（总 8001 ≤ 10^4）
    // 期望值用"暴力存集合 + 逐个匹配"独立算出，不依赖 Trie 实现
    {
        mt19937_64 rng(20251112);
        TestCase c;
        c.name = "大规模：4000 加入 + 4000 查询";
        c.ops.push_back("WordDictionary");
        c.args.push_back({});
        c.expected.push_back("null");

        unordered_set<string> dict;
        auto randWord = [&](int len)
        {
            string w;
            for (int i = 0; i < len; ++i)
                w += (char)('a' + (int)(rng() % 26));
            return w;
        };

        for (int i = 0; i < 4000; ++i)
        {
            string w = randWord(3 + (int)(rng() % 6)); // 长度 3..8
            dict.insert(w);
            c.ops.push_back("addWord");
            c.args.push_back({w});
            c.expected.push_back("null");
        }
        for (int i = 0; i < 4000; ++i)
        {
            string pattern = randWord(3 + (int)(rng() % 6));
            // 30% 的字符替换成 '.'
            for (char &ch : pattern)
                if (rng() % 100 < 30)
                    ch = '.';
            bool found = false;
            for (const string &w : dict)
                if (bruteMatch(w, pattern))
                {
                    found = true;
                    break;
                }
            c.ops.push_back("search");
            c.args.push_back({pattern});
            c.expected.push_back(found ? "true" : "false");
        }
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

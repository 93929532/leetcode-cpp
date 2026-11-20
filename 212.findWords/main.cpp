#include <iostream>
#include <string>
#include <vector>
#include <algorithm>     // sort
#include <unordered_set> // 去重 / 自检
#include <set>
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
// 212. 单词搜索 II（Word Search II）· Trie + 网格 DFS · Hard
//
// 给定一个字母网格 board 和一个单词表 words，找出**所有能在网格中拼出**的单词。
// 规则：相邻格子（上下左右）依次连接，**同一个格子在一个单词里只能用一次**。
//
//   ★★ 为什么不能"对每个单词单独搜一遍"：
//      单词数 K ≤ 3*10^4、单词长度 L ≤ 10、网格 ≤ 12×12。
//      每个单词做一次 DFS 的代价是 O(m·n·4^L)，K 个单词就是天文数字 —— 必然超时。
//
//   ★ 正解：**把单词表建成 Trie，让搜索过程同时匹配所有单词**
//      · 从每个格子出发做 DFS，一边在网格上走，一边在 Trie 上走；
//      · Trie 上走不动了（没有对应孩子）就立刻剪枝 —— 这一步把大量单词的公共前缀共享了；
//      · 走到某个 Trie 节点的 isEnd 处，说明命中了一个单词。
//      复杂度从 O(K · 4^L) 降到 O(m·n·4^L) —— **与单词数量无关**。
//
//   骨架（把单词存到 Trie 节点的 word 字段里，命中时直接取）：
//       struct Node {
//           Node* child[26] = {nullptr};
//           string word;              // 空表示不是单词结尾；非空表示这个节点结束的完整单词
//       };
//
//       void dfs(int r, int c, Node* node, vector<string>& res, vector<vector<char>>& b) {
//           char ch = b[r][c];
//           Node* next = node->child[ch - 'a'];
//           if (!next) return;                       // ★ 剪枝：Trie 上没有这条边
//
//           if (!next->word.empty()) {               // 命中一个单词
//               res.push_back(next->word);
//               next->word.clear();                  // ★ 去重：同一个词只加一次
//           }
//
//           b[r][c] = '#';                           // ★ 标记已访问
//           for each 4 neighbors (nr, nc) in bounds:
//               dfs(nr, nc, next, res, b);
//           b[r][c] = ch;                            // ★ 回溯还原
//       }
//
//   四个坑（骨架都有对应用例）：
//     1. ★★ **不标记"已访问"**：同一个格子会被反复使用，
//        `[["a"]] + ["aa"]` 会错误命中（正确答案是空）。
//     2. ★ **命中后不去重**：同一个单词可能有**多条路径**都能拼出来，
//        结果里会出现重复。可以把该节点的 word 清空（或记入 set）。
//     3. **起点要遍历所有格子**：单词可能从任意位置开始，不能只从 (0,0)。
//     4. **边界与回溯**：`b[r][c] = '#'` 后必须还原；越界的邻居要跳过。
//
//   复杂度：时间 O(m·n·4^L)（L ≤ 10，实际因剪枝远小于此）；空间 O(K·L + m·n)。
//
//   面试追问：
//     · 和 79 单词搜索有什么关系？-> 79 是"单个单词"，本题是"一批单词"，
//       批量用 Trie 共享前缀，是同一思路的升级
//     · 还能怎么加速？-> ① Trie 节点剪枝（没有孩子的节点及时删）② 从边缘开始搜
//       ③ 用位图代替字符修改 ④ 若词表极大而网格小，可以反向做"从网格枚举所有路径"
//     · 为什么要用 `'#'` 覆盖 board？-> 省一个 visited 数组，而且天然回溯
//
//   同族题：79 单词搜索、211 添加与搜索单词、648 单词替换、
//           1032 字符流、425 单词方块
class Solution
{
private:
    struct Node
    {
        Node *child[26] = {nullptr};
        string word;
    };

    Node *root;
    int R, C;

    void dfs(vector<vector<char>> &b, int r, int c, Node *node)
    {
        if (r < 0 || r >= R || c < 0 || c >= C)
            return; // 越界

        char ch = b[r][c]; 
        if (ch == '#')
            return; // 已访问

        Node *next = node->child[ch - 'a'];
        if (next == nullptr)
            return; // Trie 上没有这条边，剪枝

    }

public:
    vector<string> findWords(vector<vector<char>> &board, vector<string> &words)
    {
        // TODO: 在这里写你的解法

        root = new Node();
        R = board.size();
        C = board[0].size();

        for (string w : words)
        {
            Node *p = root;
            for (char c : w)
            {
                int k = c - 'a';
                if (p->child[k] == nullptr)
                    p->child[k] = new Node();
                p = p->child[k];
            }
            p->word = w;
        }
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 暴力：判断单词 word 能否在网格中拼出来（会临时修改 grid，然后还原）
static bool bruteOne(vector<string> &grid, const string &word, int r, int c, size_t i)
{
    if (i == word.size())
        return true;
    int R = (int)grid.size();
    int C = (int)grid[0].size();
    if (r < 0 || r >= R || c < 0 || c >= C)
        return false;
    if (grid[r][c] != word[i])
        return false;

    char save = grid[r][c];
    grid[r][c] = '#';
    bool ok = bruteOne(grid, word, r + 1, c, i + 1) ||
              bruteOne(grid, word, r - 1, c, i + 1) ||
              bruteOne(grid, word, r, c + 1, i + 1) ||
              bruteOne(grid, word, r, c - 1, i + 1);
    grid[r][c] = save;
    return ok;
}

// 暴力求全部答案（用于数据自检），返回排序后的结果
static vector<string> bruteAll(const vector<string> &board, const vector<string> &words)
{
    vector<string> res;
    for (const string &w : words)
    {
        vector<string> grid = board;
        bool found = false;
        for (int r = 0; r < (int)board.size() && !found; ++r)
            for (int c = 0; c < (int)board[0].size() && !found; ++c)
                if (bruteOne(grid, w, r, c, 0))
                    found = true;
        if (found)
            res.push_back(w);
    }
    sort(res.begin(), res.end());
    return res;
}

static string vecToStr(const vector<string> &v, size_t limit = 12)
{
    string s = "[";
    for (size_t i = 0; i < v.size() && i < limit; ++i)
    {
        if (i)
            s += ",";
        s += "\"" + v[i] + "\"";
    }
    if (v.size() > limit)
        s += ",…（共 " + to_string(v.size()) + " 个）";
    return s + "]";
}

static string boardToStr(const vector<string> &b, size_t maxRows = 4)
{
    string s = "[";
    for (size_t i = 0; i < b.size() && i < maxRows; ++i)
    {
        if (i)
            s += ",";
        s += "\"" + b[i] + "\"";
    }
    if (b.size() > maxRows)
        s += ",…（共 " + to_string(b.size()) + " 行）";
    return s + "]";
}

struct TestCase
{
    string name;
    vector<string> board; // 每行一个字符串
    vector<string> words;
    vector<string> expected; // 已排序
};

// 输入自检：网格矩形、全小写；单词非空、全小写、无重复
static bool checkInput(const TestCase &c, string &why)
{
    if (c.board.empty() || c.board[0].empty())
    {
        why = "网格为空";
        return false;
    }
    size_t C = c.board[0].size();
    for (const string &row : c.board)
    {
        if (row.size() != C)
        {
            why = "网格不是矩形";
            return false;
        }
        for (char ch : row)
            if (ch < 'a' || ch > 'z')
            {
                why = "网格里出现非小写字母";
                return false;
            }
    }
    unordered_set<string> seen;
    for (const string &w : c.words)
    {
        if (w.empty() || w.size() > 10)
        {
            why = "单词长度必须在 1..10：" + w;
            return false;
        }
        for (char ch : w)
            if (ch < 'a' || ch > 'z')
            {
                why = "单词里出现非小写字母：" + w;
                return false;
            }
        if (!seen.insert(w).second)
        {
            why = "单词表里有重复：" + w;
            return false;
        }
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

    // 数据自检：小规模用例用暴力重算，防止我手写期望值出错
    if (c.words.size() <= 600)
    {
        vector<string> brute = bruteAll(c.board, c.words);
        vector<string> selfExpected = c.expected;
        sort(selfExpected.begin(), selfExpected.end());
        if (brute != selfExpected)
        {
            cout << "[用例数据错误] " << c.name << "\n"
                 << "       暴力算出   = " << vecToStr(brute) << "\n"
                 << "       用例写的 = " << vecToStr(selfExpected) << endl;
            return false;
        }
    }

    vector<vector<char>> board;
    for (const string &row : c.board)
        board.push_back(vector<char>(row.begin(), row.end()));
    vector<string> words = c.words;

    vector<string> got;
    bool threw = false;
    string err;
    try
    {
        Solution sol;
        got = sol.findWords(board, words);
    }
    catch (const exception &e)
    {
        threw = true;
        err = e.what();
    }

    // 校验：① 无重复 ② 每个结果都在词表里 ③ 排序后与期望一致
    unordered_set<string> inputSet(c.words.begin(), c.words.end());
    unordered_set<string> gotSet;
    bool noDup = true, allInInput = true;
    for (const string &w : got)
    {
        if (!gotSet.insert(w).second)
            noDup = false;
        if (!inputSet.count(w))
            allInInput = false;
    }

    vector<string> gotSorted = got;
    sort(gotSorted.begin(), gotSorted.end());
    vector<string> want = c.expected;
    sort(want.begin(), want.end());

    bool pass = (!threw) && noDup && allInInput && (gotSorted == want);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  网格 " << c.board.size() << "x" << c.board[0].size()
         << "，词表 " << c.words.size() << " 个\n"
         << "       board = " << boardToStr(c.board) << "\n"
         << "       words = " << vecToStr(c.words) << "\n";
    if (threw)
        cout << "       异常 = " << err << "\n";
    else
    {
        cout << "       期望 = " << vecToStr(want) << "\n"
             << "       实际 = " << vecToStr(gotSorted);
        if (!noDup)
            cout << "\n       ✗ 结果里有重复单词";
        if (!allInInput)
            cout << "\n       ✗ 结果里有不在词表中的单词";
        if (gotSorted != want)
            cout << "\n       ✗ 结果与期望不一致";
        cout << "\n";
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
        {"示例 1",
         {"oaan", "etae", "ihkr", "iflv"},
         {"oath", "pea", "eat", "rain"},
         {"eat", "oath"}},
        {"示例 2（都拼不出来）",
         {"ab", "cd"},
         {"abcb"},
         {}},
    };

    vector<TestCase> extraCases = {
        // 单格
        {"单格 + 单字符", {"a"}, {"a"}, {"a"}},
        // ★ 同一格不能重复使用
        {"单格 + 双字符（必须为空）", {"a"}, {"a", "aa"}, {"a"}},
        // ★ 真正能抓"不标记 visited"的用例：必须走回已经用过的格子才可能拼出
        {"1x2 全 a，问 aaa（必须为空）", {"aa"}, {"a", "aa", "aaa"}, {"a", "aa"}},
        {"2x2 全 a，问五个 a（必须为空）", {"aa", "aa"}, {"aaaaa"}, {}},

        // 两个相邻格子
        {"左右相邻两个词", {"ab"}, {"ab", "ba"}, {"ab", "ba"}},
        // 同一个词有多条路径 -> 结果不能重复
        {"多条路径拼同一词", {"ab", "ba"}, {"ab"}, {"ab"}},
        // 对角不算相邻
        {"对角不可达", {"ab", "cd"}, {"ad"}, {}},

        // 全 a 网格：验证"同一格只能用一次"
        {"3x3 全 a 的三个词", {"aaa", "aaa", "aaa"}, {"a", "aa", "aaa"}, {"a", "aa", "aaa"}},

        // 共享前缀：o / oa / oat / oath 都可以
        {"共享前缀四个词",
         {"oaan", "etae", "ihkr", "iflv"},
         {"o", "oa", "oat", "oath"},
         {"o", "oa", "oat", "oath"}},

        // 单词不存在
        {"单词不在网格中", {"ab", "cd"}, {"ac", "bd", "abc"}, {"ac", "bd"}},
    };

    // 生成用例：12x12 全 'a'，词表 a / aa / ... / a^10 —— 全部能拼出来
    {
        TestCase c;
        c.name = "12x12 全 a + 词表 a..a^10";
        c.board.assign(12, string(12, 'a'));
        for (int k = 1; k <= 10; ++k)
            c.words.push_back(string(k, 'a'));
        c.expected = c.words;
        extraCases.push_back(c);
    }

    // 生成用例：12x12 随机网格 + 1000 个短词（长度 2..4），期望值由暴力算
    {
        mt19937_64 rng(20251116);
        TestCase c;
        c.name = "12x12 随机网格 + 400 个短词";
        for (int r = 0; r < 12; ++r)
        {
            string row;
            for (int k = 0; k < 12; ++k)
                row += (char)('a' + (int)(rng() % 4)); // 只用 a/b/c/d，制造大量匹配
            c.board.push_back(row);
        }
        set<string> uniq;
        while (uniq.size() < 400) // a-d 下长度 2..5 共 1360 种，够用
        {
            int len = 2 + (int)(rng() % 4); // 2..5
            string w;
            for (int k = 0; k < len; ++k)
                w += (char)('a' + (int)(rng() % 4));
            uniq.insert(w);
        }
        c.words.assign(uniq.begin(), uniq.end());
        c.expected = bruteAll(c.board, c.words); // 骨架自己算（1000 词，可接受）
        extraCases.push_back(c);
    }

    // 生成用例：大规模词表（3000 个），但网格里没有 'z' -> 全部拼不出来
    // 期望值为空是"构造保证"的，不需要暴力验证
    {
        mt19937_64 rng(20251117);
        TestCase c;
        c.name = "3000 词全部含 z（网格无 z，结果必为空）";
        for (int r = 0; r < 12; ++r)
        {
            string row;
            for (int k = 0; k < 12; ++k)
                row += (char)('a' + (int)(rng() % 25)); // a..y，不含 z
            c.board.push_back(row);
        }
        set<string> uniq;
        while (uniq.size() < 3000)
        {
            int len = 2 + (int)(rng() % 8); // 2..9，加 z 后 ≤ 10
            string w;
            for (int k = 0; k < len; ++k)
                w += (char)('a' + (int)(rng() % 26));
            w += 'z'; // 保证含 z -> 网格里拼不出来
            uniq.insert(w);
        }
        c.words.assign(uniq.begin(), uniq.end());
        c.expected.clear();
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

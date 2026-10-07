#include <iostream>
#include <string>
#include <vector>
#include <algorithm>   // sort：解法要用，测试骨架归一化也要用
#include <unordered_map>
#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   核心问题：怎么给「互为异位词的一组」算出一个相同的标识（key）？
        //   只要能把每个字符串映射到同一个 key，剩下的就是分组 —— 而分组
        //   在 C++ 里就是「哈希表 + 把同 key 的收集到同一个桶」。
        //
        //   思路一 · 排序后的字符串当 key（最直观，O(n · k log k)）
        //     "eat" / "tea" / "ate" 排序后都是 "aet"，天然同 key。
        //     用 unordered_map<string, vector<string>>。
        //     实现：对每个 word 复制一份、sort、把它 push 进 mp[key]。
        //
        //   思路二 · 26 字母计数当 key（更快，O(n · k)）
        //     题目限定只含小写字母，所以每个字符串可以压成一个长度 26 的
        //     计数向量 / 字符串。和 242 的定长数组是同一个思路的延伸。
        //     注意：C++ 里 vector<int> 不能直接做 unordered_map 的 key，
        //     要么换成 string（把计数编码进去），要么自己写哈希。
        //
        //   两个坑：
        //     1. 输入里有 "" 时（题目允许 strs[i].length >= 0），它自成一组，
        //        而且组内可能不止一个 —— 别在循环里把空串 continue 掉。
        //     2. 组间顺序任意，力扣判题不看顺序；但你的本地骨架会归一化后再比，
        //        所以顺序怎么摆都能「通过」，不代表顺序没被检查。
        //
        //   进阶：若输入含 unicode，思路一的 key 依然成立（先解码成码点再排序），
        //   思路二的固定 26 数组前提消失。
        unordered_map<string,vector<string>> strMap;

        for(string s : strs) {
            string key = s;
            sort(key.begin(),key.end());

            strMap[key].push_back(s);
        }

        vector<vector<string>> ans;

        for(const auto& [k, v] : strMap) {
            ans.push_back(v);
        }
        
        return ans;
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 归一化一个分组方案：组内字符串排序，组与组之间再排序。
// 因为力扣不要求组的先后顺序、也不要求组内字符串的先后顺序，
// 我们必须把「顺序无关」这一层剥掉再比较，否则会把正确解判成失败。
static vector<vector<string>> normalize(vector<vector<string>> groups) {
    for (auto& g : groups) {
        sort(g.begin(), g.end());
    }
    sort(groups.begin(), groups.end());
    return groups;
}

// 把分组方案格式化成 [[a,b],[c]] 便于打印
static string groupsToString(const vector<vector<string>>& groups) {
    string s = "[";
    for (size_t i = 0; i < groups.size(); ++i) {
        if (i) s += ", ";
        s += "[";
        for (size_t j = 0; j < groups[i].size(); ++j) {
            if (j) s += ",";
            s += "\"" + groups[i][j] + "\"";
        }
        s += "]";
    }
    s += "]";
    return s;
}

// 单条测试用例
struct TestCase {
    string name;                       // 用例名称
    vector<string> strs;               // 输入
    vector<vector<string>> expected;   // 期望分组（顺序任意）
};

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase& c) {
    vector<string> input = c.strs;   // 复制一份，避免解法改动输入影响结果打印

    Solution sol;
    vector<vector<string>> actual = sol.groupAnagrams(input);

    bool pass = (normalize(actual) == normalize(c.expected));

    cout << (pass ? "[通过] " : "[失败] ") << c.name << endl
         << "       strs     = " << groupsToString({c.strs}) << endl
         << "       期望分组 = " << groupsToString(normalize(c.expected)) << endl
         << "       实际分组 = " << groupsToString(normalize(actual)) << endl;

    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 题目给出的 3 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {"eat", "tea", "tan", "ate", "nat", "bat"},
                   {{"bat"}, {"nat", "tan"}, {"ate", "eat", "tea"}}},
        {"示例 2", {""}, {{""}}},
        {"示例 3", {"a"}, {{"a"}}},
    };

    // 额外的边界与补充用例
    const vector<TestCase> extraCases = {
        {"两个互为异位词", {"ab", "ba"}, {{"ab", "ba"}}},
        {"互不相同的单词", {"abc", "def", "ghi"}, {{"abc"}, {"def"}, {"ghi"}}},
        {"多个重复词",     {"a", "a", "a"}, {{"a", "a", "a"}}},
        {"两个空串同组",   {"", ""}, {{"", ""}}},
        {"空串与普通词",   {"", "a"}, {{""}, {"a"}}},
        {"同字母不同长度", {"ab", "abc", "ba"}, {{"ab", "ba"}, {"abc"}}},
        {"全部互为异位词", {"abc", "bca", "cab", "acb"}, {{"abc", "acb", "bca", "cab"}}},
        {"长单词",         {"listen", "silent", "enlist", "google"},
                           {{"listen", "silent", "enlist"}, {"google"}}},
        {"大小写敏感",     {"Ab", "bA", "ab", "ba"}, {{"Ab", "bA"}, {"ab", "ba"}}},
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

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>   // std::exception：测试骨架捕获半成品解法抛出的异常

#ifdef _WIN32
#include <windows.h>   // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 这题没有「一个函数」的入口，而是一个编解码器类：
//     encode: vector<string>  ->  string
//     decode: string          ->  vector<string>
// 判题方式是「往返一致」：decode(encode(strs)) 必须等于 strs。
// 两个签名与力扣判题完全一致（decode 收的是值，encode 收的是非 const 引用）。
class Codec {
public:
    // 把字符串列表编码成一个字符串
    string encode(vector<string>& strs) {
        // TODO: 在这里写你的解法
        //
        // 提示（先别展开看，卡住 10 分钟以上再回头看）：
        //
        //   核心矛盾只有一句：**你选的分隔符会出现在数据里**。
        //   所以任何「拿一个字符把各段拼起来、再按它切分」的方案，都必须回答
        //   「这个字符出现在字符串内部怎么办」。三个思路就是对它的三种回答。
        //
        //   思路一 · 长度前缀（推荐，O(总字符数)，工业界的做法）
        //     每段写成「长度 + 分隔符 + 内容」，例如 ["hello","world"] -> "5#hello5#world"。
        //     解码时：先读数字直到遇到 '#'（长度可能是多位数！），拿到 len，
        //     跳过 '#'，再原样取 len 个字符 —— 内容里出现什么字符都不影响解析，
        //     因为长度是**元信息**，不是内容的一部分。
        //     关键点：① 读数字要循环读到非数字为止，别假设一位数；
        //             ② 取完内容后下标要跳过 '#' 和 len 个字符，别只 +1。
        //
        //   思路二 · 转义（escape）
        //     选一个分隔符（比如 ','），把内容里的 '\' 和 ',' 分别写成 "\\" 和 "\,"，
        //     解码时遇到 '\' 就看下一个字符、否则遇到 ',' 才切分。
        //     关键点：**必须同时转义转义符本身**，否则 ["\\", ","] 会歧义。
        //     注意：纯转义方案还分不出 [] 和 [""]（两者都编码成空串），
        //           所以要么额外带上「元素个数」，要么干脆改用思路一。
        //
        //   思路三 · 长度前缀的变体
        //     to_string(len) + ":" + s、或固定宽度（如 %5d）都算这一类的变体，
        //     本质和思路一相同；固定宽度解码更省事，但长度超过位数就废了。
        //
        //   七个坑（这题几乎全是边界）：
        //     1. **[] 和 [""] 必须区分**：前者解出来是 0 个元素，后者是 1 个空串。
        //        这是本题最经典的 WA —— 空列表编码成空串，解回来却成了 [""]。
        //     2. **内容里含你选的分隔符/前缀形态**：[","]、["#"]、["5#hello"]、["\\"]
        //        都必须能过。最快的自测方式：拿你自己的分隔符当输入。
        //     3. **长度可能是多位数**：100 个 'x' 编码后是 "100#..."，只读一位数字就全错。
        //     4. **stoi 不告诉你它吃掉了多少字符**：stoi("12#abc") 给你 12，
        //        但 '#' 在哪儿得自己找。要么自己写解析循环，要么 find + substr
        //        （C++17 的 from_chars 会返回「解析到哪」，更适合这种场景）。
        //     5. **空串元素**：["",""] 要能还原成两个空串。长度前缀天然解决（"0#0#"）。
        //     6. **size() 是字节数**：UTF-8 下一个汉字 3 字节，用字节长度才和 substr 一致，
        //        别去想「字符个数」——编码/解码只认字节。
        //     7. **别把答案藏在成员变量里**：encode 时把 strs 存进 this->cache、
        //        decode 直接返回它 —— 这在力扣能过（判题用同一个对象反复调用），
        //        但那不是编码。下面的骨架用**两个不同的 Codec 对象**分别做
        //        encode 和 decode，这种假解法会当场露馅。
        //
        //   进阶（这题是「序列化」主题的入门）：
        //     · 同一套思路就是 297（二叉树的序列化与反序列化，你计划里序号 59，Hard）
        //       和 428（N 叉树的序列化）在做的事
        //     · 真实协议里的例子：HTTP 的 chunked encoding、Redis 的 RESP
        //       （$5\r\nhello\r\n）都是长度前缀
        //     · 如果字符串里可能出现任意字节（含 '\0'），长度前缀依然成立，
        //       转义方案要额外小心这些字节


        
        return "";
    }

    // 把 encode 产出的字符串还原成原始列表
    vector<string> decode(string s) {
        // TODO: 与 encode 配对实现，必须严格可逆（round-trip），不是「看起来能读懂」
        (void)s;
        return {};
    }
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 把字符串转成「看得见」的形式：转义反斜杠/引号/换行，过长的截断
static string escapeForPrint(const string& s, size_t limit = 120) {
    string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (i >= limit) {
            out += "…(共 " + to_string(s.size()) + " 字节)";
            break;
        }
        switch (s[i]) {
            case '\\': out += "\\\\"; break;
            case '"':  out += "\\\""; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += s[i];   break;
        }
    }
    return out;
}

// 把字符串列表格式化成 ["a","b"] 便于打印
static string listToString(const vector<string>& strs) {
    string s = "[";
    for (size_t i = 0; i < strs.size(); ++i) {
        if (i) s += ",";
        s += "\"" + escapeForPrint(strs[i], 40) + "\"";
    }
    return s + "]";
}

// 单条测试用例：本题没有「期望值」，期望就是「解回来等于原输入」
struct TestCase {
    string name;          // 用例名称
    vector<string> input; // 原始列表
};

// 运行一条用例：encode -> decode -> 与原输入逐项比较，返回该用例是否通过
//
// 这里刻意用了两个不同的 Codec 对象：encode 用一个、decode 用另一个，
// 于是 decode 只能依赖 encode 产出的那个字符串，没有别的信息来源。
static bool runCase(const TestCase& c) {
    vector<string> input = c.input;   // encode 收的是非 const 引用，复制一份给它

    string encoded;
    vector<string> actual;
    try {
        Codec encoder;
        encoded = encoder.encode(input);

        Codec decoder;                // ← 新对象，挡住「把答案存在成员变量里」的假解法
        actual = decoder.decode(encoded);
    }
    catch (const exception& e) {
        // 半成品解法里 stoi / substr 很容易抛异常，这里兜住，别让整个骨架崩掉
        cout << "[失败] " << c.name << "  抛出异常: " << e.what() << endl;
        return false;
    }

    bool pass = (actual == c.input);

    cout << (pass ? "[通过] " : "[失败] ") << c.name
         << "  原始 " << c.input.size() << " 项: " << listToString(c.input) << "\n"
         << "       编码结果: \"" << escapeForPrint(encoded) << "\"\n"
         << "       解码结果 " << actual.size() << " 项: " << listToString(actual) << endl;

    if (!pass) {
        cout << "       往返不一致！" << endl;
    }
    return pass;
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的 2 个示例
    const vector<TestCase> sampleCases = {
        {"示例 1", {"Hello", "World"}},
        {"示例 2", {""}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        {"空列表（本题最经典的 WA）",      {}},
        {"两个空串",                       {"", ""}},
        {"三个空串",                       {"", "", ""}},
        {"单元素",                         {"only"}},
        {"内容含逗号",                     {"a,b", "c,d", ",,"}},
        {"内容含井号与长度前缀形态",       {"#", "##", "5#hello", "0#"}},
        {"内容含反斜杠",                   {"\\", "\\\\", "a\\b"}},
        {"内容全是分隔符候选",             {",", "#", "\\", ":", "|", "\n"}},
        {"含空格与换行",                   {"a b", "c\nd", " e "}},
        {"UTF-8 中文与 emoji（字节数≠字符数）", {"中文", "🙂", "a中b"}},
        {"长度是两位/三位数",              {string(12, 'a'), string(100, 'b'), string(999, 'c')}},
        {"重复内容",                       {"same", "same", "same"}},
    };

    // 长字符串：1000 字节，专门验证长度前缀是否能处理多位数长度
    extraCases.push_back({"长字符串 1000 字节", {string(1000, 'x'), "短"}});

    // 100 个元素：验证解码时不会把元素个数搞错
    {
        vector<string> many;
        for (int i = 0; i < 100; ++i) many.push_back("item" + to_string(i));
        extraCases.push_back({"100 个元素", many});
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

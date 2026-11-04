#include <iostream>
#include <string>
#include <vector>
#include <unordered_map> // 解法大概率要用（key -> 链表节点）
#include <stdexcept>     // 占位实现抛异常用（写完就删）

#ifdef _WIN32
#include <windows.h> // SetConsoleOutputCP：让控制台按 UTF-8 输出，避免中文乱码
#endif

using namespace std;

// ===================== 答题区域 =====================
// 146. LRU 缓存（LRU Cache）· 设计题 + 哈希表 + 双向链表 · Medium
//
// 这是链表主题的压轴，也是最经典的设计题之一。
// 要求 get 和 put 都是 **O(1)**，还要按"最近最少使用"淘汰。
//
//   ★ 先把"需要哪些能力"列出来，数据结构自然就出来了：
//     · 给定 key 要 O(1) 找到值        -> **哈希表**
//     · 要能 O(1) 把任意一个节点移到"最新"位置 -> **链表**
//     · 要能 O(1) 删除任意一个节点     -> **双向**链表（单链表拿不到前驱，删不了）
//     · 要能 O(1) 找到最久未用的       -> 链表**尾部**就是它
//   所以标准答案 = unordered_map<int, Node*> + 双向链表（带哨兵头尾）。
//
//   链表的组织（head 端最新、tail 端最旧）：
//       head <-> [最近使用] <-> … <-> [最久未用] <-> tail
//   插入新节点 -> 挂到 head 后面；淘汰 -> 删掉 tail 前面那个。
//
//   三个基本动作（写成一个函数，get/put 都复用）：
//       void remove(Node* p)   { p->prev->next = p->next; p->next->prev = p->prev; }
//       void addFront(Node* p) { p->next = head->next; p->prev = head;
//                                head->next->prev = p; head->next = p; }
//       void freshen(Node* p)  { remove(p); addFront(p); }        // "标记为最近使用"
//
//   get(key):
//       查不到 -> return -1
//       查到   -> freshen(节点) 然后 return 节点的 val     ★ 别忘 freshen
//
//   put(key, value):
//       已存在 -> 改 val + freshen(节点)                   ★ 别忘改 val，别忘 freshen
//       不存在 -> 新建节点、挂到 head 后、记进 map
//                 若 map.size() > capacity：删掉 tail->prev，并从 map 里 erase
//
//   四个坑（骨架都有对应用例）：
//     1. **忘了哨兵**：不加 head/tail 哨兵，插入/删除都要特判"是不是第一个/最后一个"，
//        边界写到你怀疑人生。加两个哑节点后，代码里**没有任何 if 判边界**。
//     2. **get 命中时不 freshen**：这样"最近使用"信息不更新，淘汰就会淘汰错人。
//        这是本题第一号 bug，用例 5 专门抓它。
//     3. **put 已存在时只 freshen、忘了更新 val**：值没变，答案错但不崩。
//     4. **淘汰时忘了 map.erase**：map 里留着悬空指针，之后 get 到它直接崩；
//        就算不崩，size 也不会减少，会一直淘汰。
//
//   复杂度：get / put 都是 O(1)；空间 O(capacity)。
//
//   也可以用 STL 偷懒（面试可以先说这个，再写手写版）：
//       list<pair<int,int>> lru;                                  // 头部最新
//       unordered_map<int, list<pair<int,int>>::iterator> mp;
//       get:  find 命中 -> 用 splice 把节点挪到 l.begin()
//       put:  存在 -> 改值 + splice；不存在 -> emplace_front + 满则 pop_back
//     注意：**绝对不能**用 vector 当 LRU 容器 —— 中间删除是 O(n)，直接违背题目要求。
//
//   面试追问：
//     · 为什么必须双向？-> 删除节点要改前驱的 next，单链表 O(1) 拿不到前驱
//     · 怎么做到 O(1) 找最久未用？-> 链表尾部天然就是，不需要额外排序
//     · 如果要求"最不经常使用"（LFU）？-> 460 题，要再加一层频率桶，难得多
//     · 多线程？-> 要加锁；分段锁（按 key 哈希分段）能减小锁粒度
//
//   同族题：460 LFU 缓存（Hard）、432 全 O(1) 的数据结构、
//           380/381 O(1) 时间插入删除获取随机元素、588 设计文件系统
class LRUCache
{
public:
    LRUCache(int capacity)
    {
        // TODO: 在这里写你的解法
        end = head;

        for (int i = 1; i <= capacity; i++)
        {
            end->next = new cashe(-1, -1, nullptr);
            end = end->next;
        }
    }

    int get(int key)
    {
        cashe *cur = head;
        cashe *prev = head->next;

        while (cur != end)
        {
            if (prev->key == key)
            {
                int val = prev->val;
                cur->next = prev->next;
                prev->next = head->next;
                head->next = prev;

                return val;
            }

            cur = cur->next;
            prev = prev->next;
        }

        return -1;
    }

    void put(int key, int value)
    {
        end->next = new cashe(key, value, nullptr);
        end = end->next;

        head->next = head->next->next;
    }

private:
    struct cashe
    {
        int key;
        int val;
        cashe *next;

        cashe(int i, int v, cashe *p) : key(i), val(v), next(p) {};
    };

    cashe *head = new cashe(-1, 0, nullptr);
    cashe *end = head;
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------

// 单条测试用例：照搬力扣设计题的判题格式
//   ops      操作序列，第一个必须是 "LRUCache"（构造）
//   args     与 ops 一一对应，每个元素是"这一次调用的参数表"
//   expected 与 ops 一一对应：无返回值写 "null"，get 的返回值写数字（-1 表示未命中）
struct TestCase
{
    string name;
    vector<string> ops;
    vector<vector<string>> args;
    vector<string> expected;
};

// 把一次调用格式化成 put(1,1) / get(2) / LRUCache(2)
static string fmtCall(const string &op, const vector<string> &a)
{
    string s = op + "(";
    for (size_t i = 0; i < a.size(); ++i)
    {
        if (i)
            s += ",";
        s += a[i];
    }
    return s + ")";
}

// 把结果列表格式化成 [null,1,null,-1]
static string joinResults(const vector<string> &v, size_t limit = 20)
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

// 运行一条用例，打印一行信息，返回该用例是否通过
static bool runCase(const TestCase &c)
{
    vector<string> got;

    try
    {
        LRUCache *obj = nullptr;
        for (size_t i = 0; i < c.ops.size(); ++i)
        {
            const string &op = c.ops[i];
            const vector<string> &a = (i < c.args.size()) ? c.args[i] : vector<string>();

            if (op == "LRUCache")
            {
                obj = new LRUCache(stoi(a[0]));
                got.push_back("null");
            }
            else if (op == "get")
            {
                got.push_back(to_string(obj->get(stoi(a[0]))));
            }
            else if (op == "put")
            {
                obj->put(stoi(a[0]), stoi(a[1]));
                got.push_back("null");
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
    cout << "       步骤   = ";
    size_t show = (c.ops.size() <= 10) ? c.ops.size() : 6;
    for (size_t i = 0; i < show; ++i)
    {
        if (i)
            cout << " -> ";
        cout << fmtCall(c.ops[i], c.args[i]);
    }
    if (c.ops.size() > show)
    {
        cout << " -> …（共 " << c.ops.size() << " 步） -> … -> ";
        for (size_t i = c.ops.size() - 3; i < c.ops.size(); ++i)
        {
            if (i > c.ops.size() - 3)
                cout << " -> ";
            cout << fmtCall(c.ops[i], c.args[i]);
        }
    }
    cout << "\n"
         << "       期望   = " << joinResults(c.expected) << "\n"
         << "       实际   = " << joinResults(got) << endl;

    return pass;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // 让控制台按 UTF-8 输出，避免中文乱码
#endif

    // 题目给出的示例
    const vector<TestCase> sampleCases = {
        {"示例 1（容量 2）",
         {"LRUCache", "put", "put", "get", "put", "get", "put", "get", "get", "get"},
         {{"2"}, {"1", "1"}, {"2", "2"}, {"1"}, {"3", "3"}, {"2"}, {"4", "4"}, {"1"}, {"3"}, {"4"}},
         {"null", "null", "null", "1", "null", "-1", "null", "-1", "3", "4"}},
    };

    // 额外的边界与补充用例
    vector<TestCase> extraCases = {
        // 容量 1 的最简场景
        {"容量 1：放满后立刻淘汰",
         {"LRUCache", "put", "get", "put", "get", "get"},
         {{"1"}, {"1", "1"}, {"1"}, {"2", "2"}, {"1"}, {"2"}},
         {"null", "null", "1", "null", "-1", "2"}},

        // put 已存在的 key：只更新值，不应该触发淘汰
        {"put 已存在的 key 只更新值",
         {"LRUCache", "put", "put", "put", "get", "get"},
         {{"2"}, {"1", "1"}, {"2", "2"}, {"1", "10"}, {"1"}, {"2"}},
         {"null", "null", "null", "null", "10", "2"}},

        // get 未命中
        {"get 未命中返回 -1",
         {"LRUCache", "get", "put", "get", "get"},
         {{"1"}, {"1"}, {"1", "1"}, {"1"}, {"2"}},
         {"null", "-1", "null", "1", "-1"}},

        // ★ 抓坑 2：get 命中后必须 freshen，否则淘汰会淘汰错人
        {"get 会把 key 变成\"最近使用\"",
         {"LRUCache", "put", "put", "get", "put", "get", "get", "get"},
         {{"2"}, {"1", "1"}, {"2", "2"}, {"1"}, {"3", "3"}, {"2"}, {"1"}, {"3"}},
         {"null", "null", "null", "1", "null", "-1", "1", "3"}},

        // 满容量后反复 put 已存在的 key，不应该把别人挤掉
        {"满容量后重复 put 现有 key 不淘汰",
         {"LRUCache", "put", "put", "put", "put", "get", "get"},
         {{"2"}, {"1", "1"}, {"2", "2"}, {"1", "5"}, {"2", "6"}, {"1"}, {"2"}},
         {"null", "null", "null", "null", "null", "5", "6"}},

        // 容量 1 反复 put 同一个 key
        {"容量 1：反复 put 同一个 key",
         {"LRUCache", "put", "put", "put", "get"},
         {{"1"}, {"1", "1"}, {"1", "2"}, {"1", "3"}, {"1"}},
         {"null", "null", "null", "null", "3"}},

        // 容量 3：get 之后再插新 key，验证淘汰的是"没被 get 过"的那个
        {"容量 3：get 过的不被淘汰",
         {"LRUCache", "put", "put", "put", "get", "put", "get", "get", "get", "get"},
         {{"3"}, {"1", "1"}, {"2", "2"}, {"3", "3"}, {"1"}, {"4", "4"}, {"2"}, {"1"}, {"3"}, {"4"}},
         {"null", "null", "null", "null", "1", "null", "-1", "1", "3", "4"}},

        // 交替 put/get，容量 2
        {"容量 2：交替 put/get",
         {"LRUCache", "put", "get", "put", "get", "put", "get", "get"},
         {{"2"}, {"1", "1"}, {"1"}, {"2", "2"}, {"2"}, {"3", "3"}, {"1"}, {"2"}},
         {"null", "null", "1", "null", "2", "null", "-1", "2"}},
    };

    // 大规模驱逐：容量 100，塞 200 个 key，前 100 个应该全被挤出去
    {
        TestCase c;
        c.name = "容量 100：塞 200 个 key，前 100 个全被淘汰";
        c.ops.push_back("LRUCache");
        c.args.push_back({"100"});
        c.expected.push_back("null");
        for (int i = 1; i <= 200; ++i)
        {
            c.ops.push_back("put");
            c.args.push_back({to_string(i), to_string(i * 10)});
            c.expected.push_back("null");
        }
        int probes[6] = {1, 50, 100, 101, 150, 200};
        for (int k = 0; k < 6; ++k)
        {
            c.ops.push_back("get");
            c.args.push_back({to_string(probes[k])});
            c.expected.push_back(probes[k] <= 100 ? "-1" : to_string(probes[k] * 10));
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

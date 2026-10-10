# LeetCode 刷题记录（C++）

用「按知识点连刷」的方式补 C++ 算法基础，同时把 Git / GitHub 练成肌肉记忆。

- 题单：**NeetCode 150 精简版**，137 条记录 / 16 个主题（Easy 25 · Medium 92 · Hard 20）
- 节奏：每周 6 天新题 + 1 天纯复习，每天约 1 小时，单题限时 35 分钟
- 语言：C++17（每个题目一个 `main.cpp`，自带测试用例；**全仓库共用一份构建配置**）

> 完整方法论、阶段划分、递归与 DP 的专门方法见
> [学习路线图](力扣学习计划/学习路线图.md)。

---

## 进度总览

| 指标 | 数量 |
| --- | --- |
| 计划题目 | 137 条记录（其中题号 323 重复登记，实际 136 道不同题目） |
| 已建工程 | 39 个 |
| 其中在计划内 | 25 个 |
| 计划外热身题 | 14 个 |

**已建工程（计划内 25 题）**

| 题号 | 题目 | 主题 | 难度 |
| --- | --- | --- | --- |
| 1 | Two Sum | 数组与哈希 | Easy |
| 2 | Add Two Numbers | 链表 | Medium |
| 3 | Longest Substring Without Repeating Characters | 滑动窗口 | Medium |
| 4 | Median of Two Sorted Arrays | 二分查找 | Hard |
| 5 | Longest Palindromic Substring | 一维动态规划 | Medium |
| 7 | Reverse Integer | 位运算 | Medium |
| 10 | Regular Expression Matching | 二维动态规划 | Hard |
| 11 | Container With Most Water | 双指针 | Medium |
| 15 | 3Sum | 双指针 | Medium |
| 17 | Letter Combinations of a Phone Number | 回溯 | Medium |
| 19 | Remove Nth Node From End of List | 链表 | Medium |
| 20 | Valid Parentheses | 栈 | Easy |
| 21 | Merge Two Sorted Lists | 链表 | Easy |
| 23 | Merge k Sorted Lists | 链表 | Hard |
| 25 | Reverse Nodes in k-Group | 链表 | Hard |
| 36 | Valid Sudoku | 数组与哈希 | Medium |
| 49 | Group Anagrams | 数组与哈希 | Medium |
| 125 | Valid Palindrome | 双指针 | Easy |
| 128 | Longest Consecutive Sequence | 数组与哈希 | Medium |
| 167 | Two Sum II - Input Array Is Sorted | 双指针 | Medium |
| 217 | Contains Duplicate | 数组与哈希 | Easy |
| 238 | Product of Array Except Self | 数组与哈希 | Medium |
| 242 | Valid Anagram | 数组与哈希 | Easy |
| 271 | Encode and Decode Strings | 数组与哈希 | Medium |
| 347 | Top K Frequent Elements | 数组与哈希 | Medium |

**计划外热身题（14 题）**：6 字形变换、8 字符串转换整数、9 回文数、12 整数转罗马数字、
13 罗马数字转整数、14 最长公共前缀、16 最接近的三数之和、18 四数之和、22 括号生成、
24 两两交换链表中的节点、26 删除有序数组中的重复项、27 移除元素、
28 找出字符串中第一个匹配项的下标、29 两数相除

> 逐题的完成日期、是否看了解析、盲写通过日期、卡点笔记，
> 记录在 [刷题进度表.xlsx](力扣学习计划/刷题进度表.xlsx) 里。

---

## 目录结构

```
leetcode-cpp/
├── README.md                  # 本文件，仓库门面
├── .gitignore                 # 声明哪些文件不进 Git（编译产物等）
├── CMakeLists.txt             # ★ 一份构建定义，自动发现全部题目
├── CMakePresets.json          # ★ 一份可移植预设（不含绝对路径）
├── .vscode/
│   ├── tasks.json             # ★ Ctrl+Shift+B 编译「当前打开的文件」
│   └── launch.json            # ★ F5 调试「当前打开的文件」
├── 力扣学习计划/                # 计划与记录
│   ├── 学习路线图.md            # 方法论、阶段划分、C++ 模板
│   └── 刷题进度表.xlsx          # 137 题逐题清单 + 自动统计看板
├── 2.addTwoNumbers/
│   └── main.cpp               # 一个题目 = 一个目录 + 一个 main.cpp
├── 3.lengthOfLongestSubstring/
│   └── main.cpp
├── ...
└── 347.topKFrequent/
    └── main.cpp               # 共 39 个题目
```

**为什么构建配置只有一份**：`main.cpp` 之间没有任何共享代码，
所以不需要每个题目一份工程文件。VS Code 任务用 `${file}` 变量指向「当前打开的文件」，
CMake 则用通配符自动发现所有 `*/main.cpp` —— 两者都只需要配置一次。

> 好处：新增一道题只需**建目录 + 放一个 `main.cpp`**，不用再复制任何配置文件；
> 而且配置里**没有任何本机绝对路径**，换电脑克隆下来就能编译。

### 每个 `main.cpp` 长什么样

分成两段，中间有醒目分隔线：

```cpp
// ===================== 答题区域 =====================
class Solution {
public:
    // 在这里写解法
};
// ===================== 答题区域结束 =====================

// ----------------- 以下为辅助函数与测试骨架，无需修改 -----------------
int main() {
    // 自动跑若干组示例，逐条打印 [通过] / [失败]
}
```

好处：写完直接跑，**看到 `[通过]` 就说明逻辑对了**，不依赖网页判题。

---

## 环境要求

两台电脑（或任何一台新机器）clone 之后，需要满足：

| 工具 | 用途 | 是否必需 |
| --- | --- | --- |
| `g++` | 编译 | ✅ 必需 |
| `cmake` | 方式二构建 | 用 CMake 时必需 |
| `ninja` | 方式二的构建后端 | 用 CMake 时必需 |
| `gdb` | F5 调试 | 调试时必需 |

**Windows 一键安装**（WinGet 版 WinLibs，自带以上全部四个）：

```powershell
winget install BrechtSanders.WinLibs.POSIX.UCRT
```

装完**关掉所有终端和 VS Code 再重开**（让新的 `PATH` 生效），然后验证：

```powershell
g++ --version; cmake --version; ninja --version; gdb --version
```

> 只要 `g++` 在 `PATH` 上，**方式一（最常用）就能用**，不需要 cmake / ninja / gdb。

---

## 怎么编译运行一道题

### 方式一：VS Code 一键编译（日常推荐）

1. 打开某题的 `main.cpp`
2. 按 <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd> → 在该目录生成 `main.exe`
3. 运行：

   ```powershell
   cd 2.addTwoNumbers
   .\main.exe
   ```

也可以按 <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>P</kbd> → `Tasks: Run Task` →
选「**编译并运行当前文件**」，一步完成编译 + 运行。

按 <kbd>F5</kbd> 可以进入调试（会先自动编译），适合看变量、单步跟踪。

### 方式二：命令行直接编译（不依赖任何配置）

```powershell
cd 2.addTwoNumbers
g++ -std=c++17 -O2 -Wall main.cpp -o main.exe
.\main.exe
```

预期输出：

```
示例 1:
  l1 = 2 -> 4 -> 3
  l2 = 5 -> 6 -> 4
  sum = 7 -> 0 -> 8
  [通过]
示例 2:
  ...
```

### 方式三：CMake（一次构建全部题目）

在**仓库根目录**执行：

```powershell
cmake --preset default          # 配置
cmake --build --preset default  # 编译全部 39 道题
```

只想编译一道题：

```powershell
cmake --build --preset default --target p15_threeSum
```

产物同样是每个题目目录下的 `main.exe`，运行方式与方式一相同。

> `CMakePresets.json` 里用的是 `"generator": "Ninja"`，不含任何绝对路径。
> 如果你的机器上没有 Ninja，可以把生成器换成 `"MinGW Makefiles"`
> （对应需要 `mingw32-make` 在 `PATH` 上），或者改用 `cmake -S . -B build -G "MinGW Makefiles"`。

---

## 提交规范

提交信息按 `类型(范围): 说明` 写，一眼能看出这次改了什么：

| 类型 | 用途 | 例子 |
| --- | --- | --- |
| `feat` | 新增题解 | `feat(15): 三数之和 排序+双指针` |
| `fix` | 修 bug | `fix(23): 修正堆比较器写反导致顺序错误` |
| `docs` | 文档 / 笔记 | `docs(plan): 补充回溯模板的两种写法` |
| `refactor` | 重写（功能不变） | `refactor(2): 链表题改用哑结点统一处理` |
| `chore` | 杂项 | `chore: 忽略 CMake 构建产物` |

**好的提交信息**告诉你「为什么」，而不只是「改了什么」：

```
feat(15): 三数之和 排序+双指针

先排序，固定第一个数后退化成两数之和，用左右指针夹逼。
关键点：跳过重复元素要在「选完当前数之后」做，否则会漏解。
```

---

## 新增一道题怎么做

1. 建目录，名字用 `题号.函数名`：`19.removeNthFromEnd/`
2. 在里面放一个 `main.cpp`（照抄任意一道题的结构，只改「答题区域」）
3. 编译运行 —— **不需要新建任何配置文件**

用 CMake 的话，`CONFIGURE_DEPENDS` 会自动发现新目录，直接
`cmake --build --preset default` 即可。

---

## 计划文件说明

| 文件 | 作用 |
| --- | --- |
| [学习路线图.md](力扣学习计划/学习路线图.md) | 为什么这样排题、六个阶段、递归四步法、DP 三段式、8 个 C++ 模板 |
| [刷题进度表.xlsx](力扣学习计划/刷题进度表.xlsx) | 137 题逐题状态；「主题看板」表用公式自动算每主题完成率 |

**核心执行纪律**（摘自学习路线图）：

1. 单题限时 **35 分钟**，到点必看题解
2. 看完题解 **必须关掉盲写一遍**，抄写不长肌肉记忆
3. 错题本只记一列：**卡在哪一步**
4. 标记「看了解析才会」的题，在 **1 天后 / 3 天后 / 7 天后**各盲写一次
5. **不跳主题**，按阶段顺序推进
6. 时间不够就只做复习，**不开始新题**（半途而废的新题是负收益）

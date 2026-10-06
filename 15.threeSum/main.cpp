#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Solution {
public:
    vector<vector<int>> threeSum(vector<int>& nums) {

        vector<vector<int>> result;

        sort(nums.begin(), nums.end());


        for (size_t i = 0; i + 2 < nums.size(); i++)
        {
            // 外层去重：nums[i] 作为最小值已经枚举过，跳过相同的值
            if (i > 0 && nums[i] == nums[i - 1]) continue;

            /*
            // 剪枝1：最小的三个数之和都大于 0，后面不可能再有解
            if (nums[i] + nums[i + 1] + nums[i + 2] > 0) break;
            // 剪枝2：nums[i] 与最大的两个数相加都小于 0，说明 nums[i] 太小，换更大的
            if (nums[i] + nums[n - 2] + nums[n - 1] < 0) continue;
            */

            size_t L = i + 1, R = nums.size() - 1;

            while (L < R)
            {
                int sum = nums[i] + nums[L] + nums[R];

                if (sum == 0)
                {
                    result.push_back({nums[i], nums[L], nums[R]});

                    // 内层去重：跳过重复的第二、第三个数，保证每个三元组只生成一次
                    while (L < R && nums[L] == nums[L + 1]) L++;
                    while (L < R && nums[R] == nums[R - 1]) R--;

                    L++;
                    R--;
                }
                else if (sum < 0)
                {
                    L++;
                }
                else
                {
                    R--;
                }
            }
        }

        // 去重已在生成时完成，无需再排序 + unique

        return result;
    }
};

// -------------------------- 以下为测试代码，无需修改 --------------------------

// 打印一维整数数组，例如 [1, 2, 3]
static void printVector(const vector<int>& v) {
    cout << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        cout << v[i];
        if (i + 1 < v.size()) cout << ", ";
    }
    cout << "]";
}

// 打印二维结果，例如 [[-1, -1, 2], [-1, 0, 1]]
static void printResult(const vector<vector<int>>& res) {
    cout << "[";
    for (size_t i = 0; i < res.size(); ++i) {
        printVector(res[i]);
        if (i + 1 < res.size()) cout << ", ";
    }
    cout << "]";
}

// 题目说明顺序不重要，这里统一排序后再比较（三元组内部升序、三元组之间升序）
static vector<vector<int>> normalize(vector<vector<int>> res) {
    for (auto& tri : res) sort(tri.begin(), tri.end());
    sort(res.begin(), res.end());
    return res;
}

// 运行单个测试用例
static void runCase(int id, vector<int> nums, const vector<vector<int>>& expected) {
    cout << "示例 " << id << ":" << endl;
    cout << "输入: nums = ";
    printVector(nums);
    cout << endl;

    vector<vector<int>> got = Solution().threeSum(nums);

    cout << "输出: ";
    printResult(got);
    cout << endl;

    cout << "预期: ";
    printResult(expected);
    cout << endl;

    cout << "结果: " << (normalize(got) == normalize(expected) ? "通过" : "不通过") << endl;
    cout << string(40, '-') << endl;
}

int main() {
    runCase(1, {-1, 0, 1, 2, -1, -4}, {{-1, -1, 2}, {-1, 0, 1}});
    runCase(2, {0, 1, 1}, {});
    runCase(3, {0, 0, 0}, {{0, 0, 0}});

    return 0;
}

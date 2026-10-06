#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

class Solution {
public:
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        int m = nums1.size(), n = nums2.size();
        // 始终在较短的数组上进行二分，保证 O(log(min(m,n))) <= O(log(m+n))
        if (m > n) return findMedianSortedArrays(nums2, nums1);

        int lo = 0, hi = m;
        int total = m + n, half = (total + 1) / 2;

        while (lo <= hi) {
            int cut1 = lo + (hi - lo) / 2;       // nums1 左半部分的元素个数
            int cut2 = half - cut1;              // nums2 左半部分的元素个数

            // 处理边界：切割点在左/右端点时，用 INT_MIN/INT_MAX 表示无穷
            int left1  = (cut1 == 0) ? INT_MIN : nums1[cut1 - 1];
            int right1 = (cut1 == m) ? INT_MAX : nums1[cut1];
            int left2  = (cut2 == 0) ? INT_MIN : nums2[cut2 - 1];
            int right2 = (cut2 == n) ? INT_MAX : nums2[cut2];

            if (left1 <= right2 && left2 <= right1) {
                // 找到正确划分
                if (total % 2 == 0) {
                    return (max(left1, left2) + min(right1, right2)) / 2.0;
                }
                return max(left1, left2);
            } else if (left1 > right2) {
                hi = cut1 - 1;   // nums1 左半部分太大，向左移动
            } else {
                lo = cut1 + 1;   // nums1 左半部分太小，向右移动
            }
        }
        return 0.0;
    }
};

// —— 以上为答题区域  ——

int main() {
    Solution sol;

    // 测试用例 1：总长度为奇数
    vector<int> nums1 = {1, 3};
    vector<int> nums2 = {2};
    cout << "Test 1: " << sol.findMedianSortedArrays(nums1, nums2) << " (expected: 2.0)" << endl;

    // 测试用例 2：总长度为偶数
    vector<int> nums3 = {1, 2};
    vector<int> nums4 = {3, 4};
    cout << "Test 2: " << sol.findMedianSortedArrays(nums3, nums4) << " (expected: 2.5)" << endl;

    // 测试用例 3：其中一个数组为空
    vector<int> nums5;
    vector<int> nums6 = {1};
    cout << "Test 3: " << sol.findMedianSortedArrays(nums5, nums6) << " (expected: 1.0)" << endl;

    // 测试用例 4：含负数
    vector<int> nums7 = {-5, 0, 5};
    vector<int> nums8 = {-2, 2};
    cout << "Test 4: " << sol.findMedianSortedArrays(nums7, nums8) << " (expected: 0.0)" << endl;

    return 0;
}

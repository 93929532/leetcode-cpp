#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int maxArea(vector<int>& height) {
        
        // 双指针法
        int left = 0, right = height.size() - 1;
        int maxA = (right - left) * min(height[left],height[right]);
        int Area = 0;

        while(left != right)
        {
            if(height[left] > height[right])
            {
                // 右指针向左移动
                right --;
                Area = (right - left) * min(height[left],height[right]);
                if(maxA < Area)
                    maxA = Area;
            }
            else
            {
                // 左指针向右移动
                left ++;
                Area = (right - left) * min(height[left],height[right]);
                if(maxA < Area)
                    maxA = Area;
            }
        }
        return maxA;
    }
};

int main() {
    Solution solution;

    // 示例 1
    vector<int> height1 = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    cout << "示例 1 输出: " << solution.maxArea(height1) << " (预期 49)" << endl;

    // 示例 2
    vector<int> height2 = {1, 1};
    cout << "示例 2 输出: " << solution.maxArea(height2) << " (预期 1)" << endl;

    return 0;
}
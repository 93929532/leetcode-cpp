#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Solution {
public:
    // 答题区域：在这里实现你的算法
    string intToRoman(int num) {
        if(num < 1||num >= 4000)
        {
            return "";
        }
        
        vector<string> ROMAN = {"M","CM","D","CD","C","XC","L","XL","X","IX","V","IV","I"};
        vector<int> values = {1000,900,500,400,100,90,50,40,10,9,5,4,1};

        string Rnum;

        for(int i = 0; i < 13; i ++)
        {
            while(num >= values[i])
            {
                Rnum.append(ROMAN[i]);
                num -= values[i];
            }
        }

        return Rnum;

    }
};

int main() {
    Solution sol;

    // 测试用例
    int nums[] = {3, 4, 9, 58, 1994, 3999};
    for (int num : nums) {
        cout << num << " -> " << sol.intToRoman(num) << endl;
    }

    return 0;
}

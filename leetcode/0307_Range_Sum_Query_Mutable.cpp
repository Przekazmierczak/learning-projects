/*
Given an integer array nums, handle multiple queries of the following types:

Update the value of an element in nums.
Calculate the sum of the elements of nums between indices left and right inclusive where left <= right.
Implement the NumArray class:

NumArray(int[] nums) Initializes the object with the integer array nums.
void update(int index, int val) Updates the value of nums[index] to be val.
int sumRange(int left, int right) Returns the sum of the elements of nums between indices left and right inclusive (i.e. nums[left] + nums[left + 1] + ... + nums[right]).
 
Example 1:
Input
["NumArray", "sumRange", "update", "sumRange"]
[[[1, 3, 5]], [0, 2], [1, 2], [0, 2]]
Output
[null, 9, null, 8]

Explanation
NumArray numArray = new NumArray([1, 3, 5]);
numArray.sumRange(0, 2); // return 1 + 3 + 5 = 9
numArray.update(1, 2);   // nums = [1, 2, 5]
numArray.sumRange(0, 2); // return 1 + 2 + 5 = 8

Constraints:
1 <= nums.length <= 3 * 104
-100 <= nums[i] <= 100
0 <= index < nums.length
-100 <= val <= 100
0 <= left <= right < nums.length
At most 3 * 104 calls will be made to update and sumRange.
*/

#include <vector>

class NumArray {
public:
    int* tree;
    int tree_size;
    int nums_size;
    
    int nextPowerOf2(int n) {
        if (n == 0) return 1;

        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        return n + 1;
    }

    int CreateTree(std::vector<int>& nums, int left, int right, int pos) {
        if (left == right) {
            tree[pos] = nums[left];
            return tree[pos];
        }
        int mid = left + (right - left) / 2;
        tree[pos] = CreateTree(nums, left, mid, pos * 2 + 1) + CreateTree(nums, mid + 1, right, pos * 2 + 2);
        return tree[pos];
    }

    int update_helper(int left, int right, int pos, int index, int val) {
        int mid = left + (right - left) / 2;
        int diff;
        
        if (index >= left && index <= right) {
            if (left == right) {
                diff = tree[pos] - val;
            } else {
                diff = update_helper(left, mid, pos * 2 + 1, index, val) + update_helper(mid + 1, right, pos * 2 + 2, index, val);
            }
            tree[pos] -= diff;
            return diff;
        }
        return 0;
    };

    int sum_helper(int curr_l, int curr_r, int left, int right, int pos) {
        int mid = curr_l + (curr_r - curr_l) / 2;
        if (left <= curr_l && right >= curr_r) {
            return tree[pos];
        }
        if (curr_r < left || curr_l > right) {
            return 0;
        }
        return sum_helper(curr_l, mid, left, right, pos * 2 + 1) + sum_helper(mid + 1, curr_r, left,right, pos * 2 + 2);
    }
    
    NumArray(std::vector<int>& nums) {
        nums_size = nums.size();
        tree_size = nextPowerOf2(nums.size() * 2);
        tree = new int[tree_size];
        for (int i = 0; i < tree_size; i++) {
            tree[i] = 0;
        }
        CreateTree(nums, 0, nums.size() - 1, 0);
    }
    
    void update(int index, int val) {
        update_helper(0, nums_size - 1, 0, index, val);
    }
    
    int sumRange(int left, int right) {
        return sum_helper(0, nums_size - 1, left, right, 0);
    }
};

/**
 * Your NumArray object will be instantiated and called as such:
 * NumArray* obj = new NumArray(nums);
 * obj->update(index,val);
 * int param_2 = obj->sumRange(left,right);
 */
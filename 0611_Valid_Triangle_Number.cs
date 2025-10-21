/*
Given an integer array nums, return the number of triplets chosen from the array that can make triangles if we take them as side lengths of a triangle.

Example 1:
Input: nums = [2,2,3,4]
Output: 3
Explanation: Valid combinations are: 
2,3,4 (using the first 2)
2,3,4 (using the second 2)
2,2,3

Example 2:
Input: nums = [4,2,3,4]
Output: 4

Constraints:

1 <= nums.length <= 1000
0 <= nums[i] <= 1000
 */
 
public class Solution {
    public int TriangleNumber(int[] nums) {
        Array.Sort(nums);
        int count = 0;

        for (int third = nums.Length - 1; third > 1; third--) {
            int first = 0;
            int second = third - 1;

            while (first < second) {
                if (nums[first] + nums[second] > nums[third]) {
                    count += second - first;
                    second--;
                } else {
                    first++;
                }
            }
        }
        return count;
    }
}
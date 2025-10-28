/*
We call a subarray alternating if no two adjacent elements in the subarray have the same value.
Return the number of alternating subarrays in nums.

Example 1:
Input: nums = [0,1,1,1]
Output: 5
Explanation:
The following subarrays are alternating: [0], [1], [1], [1], and [0,1].

Example 2:
Input: nums = [1,0,1,0]
Output: 10
Explanation:
Every subarray of the array is alternating. There are 10 possible subarrays that we can choose.

Constraints:
1 <= nums.length <= 10^5
nums[i] is either 0 or 1.
*/
 
public class Solution {
    public long CountAlternatingSubarrays(int[] nums) {
        long res = 1;
        long curr = 1;
        
        for (int i = 1; i < nums.Length; i++) {
            if (nums[i] == nums[i - 1]) {
                curr = 1;
            } else {
                curr++;
            }
            res += curr;
        }

        return res;
    }
}
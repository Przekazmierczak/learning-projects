/*
Given an integer num, find the closest two integers in absolute difference whose product equals num + 1 or num + 2.
Return the two integers in any order.

Example 1:
Input: num = 8
Output: [3,3]
Explanation: For num + 1 = 9, the closest divisors are 3 & 3, for num + 2 = 10, the closest divisors are 2 & 5, hence 3 & 3 is chosen.

Example 2:
Input: num = 123
Output: [5,25]
Example 3:

Input: num = 999
Output: [40,25]

Constraints:

1 <= num <= 10^9
 */
 
public class Solution {
    public int[] ClosestPair(int num) {
        int curr = (int)Math.Sqrt(num);
        int[] res = new int[2];
        for (int i = curr; i >= 1; i--) {
            if (num % i == 0) {
                res[0] = i;
                res[1] = num / i;
                return res;
            }
        }
        return res;
    }
    
    public int[] ClosestDivisors(int num) {
        int[] pair1 = ClosestPair(num + 1);
        int[] pair2 = ClosestPair(num + 2);
        if (pair1[1] - pair1[0] > pair2[1] - pair2[0]) {
            return pair2;
        }
        return pair1;
    }
}
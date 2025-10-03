/*
The power of the string is the maximum length of a non-empty substring that contains only one unique character.
Given a string s, return the power of s.

Example 1:
Input: s = "leetcode"
Output: 2
Explanation: The substring "ee" is of length 2 with the character 'e' only.

Example 2:
Input: s = "abbcccddddeeeeedcba"
Output: 5
Explanation: The substring "eeeee" is of length 5 with the character 'e' only.

Constraints:
1 <= s.length <= 500
s consists of only lowercase English letters.
 */
 
public class Solution {
    public int MaxPower(string s) {
        int count = 0;
        int max = 0;
        char curr = ' ';
        
        foreach (char l in s) {
            if (l == curr) {
                count++;
            } else {
                max = Math.Max(count, max);
                count = 1;
                curr = l;
            }
        }
        
        return Math.Max(count, max);
    }
}
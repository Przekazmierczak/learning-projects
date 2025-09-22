/*
Given two integer arrays nums1 and nums2, return the maximum length of a subarray that appears in both arrays.

Example 1:
Input: nums1 = [1,2,3,2,1], nums2 = [3,2,1,4,7]
Output: 3
Explanation: The repeated subarray with maximum length is [3,2,1].

Example 2:
Input: nums1 = [0,0,0,0,0], nums2 = [0,0,0,0,0]
Output: 5
Explanation: The repeated subarray with maximum length is [0,0,0,0,0].

Constraints:
1 <= nums1.length, nums2.length <= 1000
0 <= nums1[i], nums2[i] <= 100
 */
 
public class Solution {
    int[] nums1;
    int[] nums2;
    
    int len1;
    int len2;
    
    ulong BASE = 103;
    
    ulong[] pow;
    ulong[] hash1;
    ulong[] hash2;
    
    public int FindLength(int[] n1, int[] n2) {
        organize(n1, n2);

        int left = 1;
        int right = len2;
        int mid;
        while (left <= right) {
            mid = (left + right) / 2;

            if (checkSize(mid)) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return left - 1;
    }

    void organize(int[] n1, int[] n2) {
      if (n1.Length >= n2.Length) {
            nums1 = n1;
            nums2 = n2;
        } else {
            nums1 = n2;
            nums2 = n1;
        }

        len1 = nums1.Length;
        len2 = nums2.Length;

        fillPow();
        hash1 = fillHash(nums1,len1);
        hash2 = fillHash(nums2, len2);
    }

    void fillPow() {
        pow = new ulong[len1];
        pow[0] = 1;
        for (int i = 1; i < len1; i++) {
            pow[i] = pow[i - 1] * BASE;
        }
    }

    ulong[] fillHash(int[] nums, int len) {
        ulong[] hash = new ulong[len];
        hash[0] = (ulong) nums[0];
        for (int i = 1; i < len; i++) {
            hash[i] = hash[i - 1] * BASE + (ulong)nums[i];
        }
        return hash;
    }

    ulong getHash(ulong[] hash, int left, int right) {
        if (left == 0) {
            return hash[right];
        } else {
            return hash[right] - (hash[left - 1] * pow[right - (left - 1)]);
        }
    }

    bool checkMatch(int index1, int index2, int len) {
        for (int i = 0; i < len; i++) {
            if (nums1[i + index1] != nums2[i + index2]) {
                return false;
            }
        }
        return true;
    }

    bool checkSize(int size) {
        Dictionary<ulong, List<int>> dict = new Dictionary<ulong, List<int>>();
        size--;
        
        for (int i = 0; i + size < len1; i++) {
            ulong curr = getHash(hash1, i, i + size);
            if (!dict.ContainsKey(curr))
                dict[curr] = new List<int>();
            dict[curr].Add(i);
        }

        for (int i = 0; i + size < len2; i++) {
            ulong curr = getHash(hash2, i, i + size);
            if (dict.ContainsKey(curr)) {
                foreach (int j in dict[curr]) {
                    if (checkMatch(j, i, size)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
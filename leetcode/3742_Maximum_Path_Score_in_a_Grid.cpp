/*
You are given an m x n grid where each cell contains one of the values 0, 1, or 2. You are also given an integer k.
You start from the top-left corner (0, 0) and want to reach the bottom-right corner (m - 1, n - 1) by moving only right or down.
Each cell contributes a specific score and incurs an associated cost, according to their cell values:

0: adds 0 to your score and costs 0.
1: adds 1 to your score and costs 1.
2: adds 2 to your score and costs 1. ​​​​​​​
Return the maximum score achievable without exceeding a total cost of k, or -1 if no valid path exists.

Note: If you reach the last cell but the total cost exceeds k, the path is invalid.

Example 1:
Input: grid = [[0, 1],[2, 0]], k = 1
Output: 2
Explanation:​​​​​​​
The optimal path is:

Cell	grid[i][j]	Score	TotalScore	Cost	TotalCost
(0, 0)	    0	      0	        0	      0	        0
(1, 0)	    2	      2	        2	      1	        1
(1, 1)	    0	      0	        2	      0	        1
Thus, the maximum possible score is 2.

Example 2:
Input: grid = [[0, 1],[1, 2]], k = 1
Output: -1

Explanation:
There is no path that reaches cell (1, 1)​​​​​​​ without exceeding cost k. Thus, the answer is -1.

Constraints:
1 <= m, n <= 200
0 <= k <= 103​​​​​​​
​​​​​​​grid[0][0] == 0
0 <= grid[i][j] <= 2
*/

#include <vector>
#include <algorithm>

class Solution {
public:
    int maxPathScore(std::vector<std::vector<int>>& grid, int k) {
        int m = grid.size();
        int n = grid[0].size();
        int max_k = std::min(k, m + n - 2);

        int resGrid[m][n][max_k + 1];
        for (int row = 0; row < m; row++) {
            for (int col = 0; col < n; col++) {
                for (int curr_k = 0; curr_k <= max_k; curr_k++) {
                    resGrid[row][col][curr_k] = -1;
                }
            }
        }

        resGrid[0][0][0] = 0;

        for (int row = 0; row < m; row++) {
            for (int col = 0; col < n; col++) {

                if (row == 0 && col == 0) {
                    continue;
                }

                int increment = grid[row][col] != 0;

                for (int curr_k = 0; curr_k <= max_k; curr_k++) {
                    int left = -1;
                    int top = -1;

                    if (col != 0) {
                        left = resGrid[row][col - 1][curr_k];
                    }
                    if (row != 0) {
                        top = resGrid[row - 1][col][curr_k];
                    }

                    if ((left != -1 || top != -1) && curr_k + increment <= max_k) {
                        resGrid[row][col][curr_k + increment] = std::max(left, top) + grid[row][col];
                    }
                }
            }
        }

        int res = -1;
        for (int curr_k = 0; curr_k <= max_k; curr_k++) {
            res = std::max(resGrid[m - 1][n - 1][curr_k], res);
        }

        return res;
    }
};
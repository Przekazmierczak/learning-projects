/*
You are given a m x n 2D integer array grid and an integer k. You start at the top-left cell (0, 0) and your goal is to reach the bottom‐right cell (m - 1, n - 1).
There are two types of moves available:
Normal move: You can move right or down from your current cell (i, j), i.e. you can move to (i, j + 1) (right) or (i + 1, j) (down). The cost is the value of the destination cell.
Teleportation: You can teleport from any cell (i, j), to any cell (x, y) such that grid[x][y] <= grid[i][j]; the cost of this move is 0. You may teleport at most k times.
Return the minimum total cost to reach cell (m - 1, n - 1) from (0, 0).

Example 1:
Input: grid = [[1,3,3],[2,5,4],[4,3,5]], k = 2
Output: 7

Explanation:
Initially we are at (0, 0) and cost is 0.

Current Position	Move	     New Position	Total Cost
(0, 0)	            Move Down	 (1, 0)	        0 + 2 = 2
(1, 0)	            Move Right	 (1, 1)	        2 + 5 = 7
(1, 1)	            Teleport to  (2, 2)     	7 + 0 = 7
The minimum cost to reach bottom-right cell is 7.

Example 2:
Input: grid = [[1,2],[2,3],[3,4]], k = 1

Output: 9

Explanation:
Initially we are at (0, 0) and cost is 0.

Current Position	Move	    New Position	Total Cost
(0, 0)	            Move Down	(1, 0)	        0 + 2 = 2
(1, 0)	            Move Right	(1, 1)	        2 + 3 = 5
(1, 1)	            Move Down	(2, 1)	        5 + 4 = 9
The minimum cost to reach bottom-right cell is 9.

Constraints:
2 <= m, n <= 80
m == grid.length
n == grid[i].length
0 <= grid[i][j] <= 104
0 <= k <= 10
*/

#include <vector>
#include <algorithm>

struct Node {
    int weight = 0;
    int cost = std::numeric_limits<int>::max();

    bool operator<(const Node& other) const {
        return weight > other.weight;
    }
};

class Solution {
public:
    std::vector<std::vector<Node*>> table;
    std::vector<Node*> list;
    
    void dynamic() {
        for (int row = 0; row < table.size(); row++) {
            for (int col = 0; col < table[0].size(); col++) {
                if (row > 0 && col > 0) {
                    table[row][col]->cost = std::min(table[row][col]->cost, std::min(table[row - 1][col]->cost, table[row][col - 1]->cost) + table[row][col]->weight);
                } else if (row > 0) {
                    table[row][col]->cost = std::min(table[row][col]->cost, table[row - 1][col]->cost + table[row][col]->weight);
                } else if (col > 0) {
                    table[row][col]->cost = std::min(table[row][col]->cost, table[row][col - 1]->cost + table[row][col]->weight);
                } else {
                    table[row][col]->cost = 0;
                }
            }
        }
    }

    void teleport() {
        int currWeight = -1;
        int currMin = list[0]->cost;
        for (int i = 0; i < list.size(); i++) {
            if (currWeight != list[i]->weight) {
                int next = i + 1;
                while (next < list.size() && list[next]->weight == list[i]->weight) {
                    currMin = std::min(currMin, list[next]->cost);
                    next++;
                }
            }
            
            if (list[i]->cost > currMin) {
                list[i]->cost = currMin;
            }
            currWeight = list[i]->weight;
        
            currMin = std::min(currMin, list[i]->cost);
        }
    }
    
    int minCost(std::vector<std::vector<int>>& grid, int k) {
        for (int row = 0; row < grid.size(); row++) {
            std::vector<Node*> curr;
            for (int col = 0; col < grid[0].size(); col++) {
                Node *node = new Node();
                node->weight = grid[row][col];
                curr.push_back(node);
                list.push_back(node);
            }
            table.push_back(curr);
        }

        std::sort(list.begin(), list.end(), [](const Node* a, const Node* b) {
            return *a < *b;
        });
        
        dynamic();
        for (int i = 0; i < k; i++){
            teleport();
            dynamic();
        }
        
        int res =  table[table.size() - 1][table[0].size() - 1]->cost;

        for (int i = 0; i < list.size(); i++) {
            delete list[i];
        }
        
        return res;
    }
};
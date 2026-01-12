/*
There are n cities. Some of them are connected, while some are not. If city a is connected directly with city b, and city b is connected directly with city c, then city a is connected indirectly with city c.
A province is a group of directly or indirectly connected cities and no other cities outside of the group.
You are given an n x n matrix isConnected where isConnected[i][j] = 1 if the ith city and the jth city are directly connected, and isConnected[i][j] = 0 otherwise.
Return the total number of provinces.

Example 1:
Input: isConnected = [[1,1,0],[1,1,0],[0,0,1]]
Output: 2
Example 2:

Input: isConnected = [[1,0,0],[0,1,0],[0,0,1]]
Output: 3
 
Constraints:
1 <= n <= 200
n == isConnected.length
n == isConnected[i].length
isConnected[i][j] is 1 or 0.
isConnected[i][i] == 1
isConnected[i][j] == isConnected[j][i]
*/

#include <vector>

class Solution {
public:
    std::vector<int> citiesConnection;
    std::vector<int> rank;

    void unite(int city1, int city2) {
        if (rank[city1] > rank[city2]) {
            citiesConnection[city2] = city1;
        } else if (rank[city1] < rank[city2]) {
            citiesConnection[city1] = city2;
        } else {
            citiesConnection[city2] = city1;
            rank[city1]++;
        }
    }

    int find(int city) {
        if (citiesConnection[city] != city) {
            city = find(citiesConnection[city]);
        }
        return city;
    }

    int findCircleNum(std::vector<std::vector<int>>& isConnected) {
        for (int i = 0; i < isConnected.size(); i++) {
            citiesConnection.push_back(i);
            rank.push_back(0);
        }

        for (int i = 0; i < isConnected.size(); i++) {
            for (int j = i + 1; j < isConnected.size(); j++) {
                if (isConnected[i][j]) {
                    int mainCity1 = find(i);
                    int mainCity2 = find(j);

                    if (mainCity1 != mainCity2) {
                        unite(mainCity1, mainCity2);
                    }
                }
            }
        }

        int res = 0;

        for (int i = 0; i < isConnected.size(); i++) {
            if (citiesConnection[i] == i) res++;
        }

        return res;
    }
};
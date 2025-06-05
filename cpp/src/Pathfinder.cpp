#include "Pathfinder.h"
#include <queue>
#include <algorithm>

struct Node { int x; int y; };

std::vector<Province*> findPath(ProvinceGrid& grid, int startX, int startY, int goalX, int goalY) {
    int h = grid.size();
    if (h == 0) return {};
    int w = grid[0].size();

    std::queue<Node> q;
    std::vector<std::vector<bool>> visited(h, std::vector<bool>(w, false));
    std::vector<std::vector<Node>> parent(h, std::vector<Node>(w, {-1, -1}));

    q.push({startX, startY});
    visited[startY][startX] = true;

    int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    while (!q.empty()) {
        Node cur = q.front();
        q.pop();
        if (cur.x == goalX && cur.y == goalY) {
            break;
        }
        for (auto& d : dirs) {
            int nx = cur.x + d[0];
            int ny = cur.y + d[1];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h && !visited[ny][nx]) {
                if (grid[ny][nx].terrain == TerrainType::Water) {
                    continue; // water is impassable
                }
                visited[ny][nx] = true;
                parent[ny][nx] = cur;
                q.push({nx, ny});
            }
        }
    }

    if (!visited[goalY][goalX]) {
        return {};
    }

    std::vector<Province*> path;
    Node cur = {goalX, goalY};
    while (!(cur.x == startX && cur.y == startY)) {
        path.push_back(&grid[cur.y][cur.x]);
        cur = parent[cur.y][cur.x];
    }
    path.push_back(&grid[startY][startX]);
    std::reverse(path.begin(), path.end());
    return path;
}

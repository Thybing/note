#include <iostream>
#include <vector>

#include <sstream>

using namespace std;

constexpr int MX = 1e5 + 5;
constexpr int INF = 0x3f3f3f3f;

struct Edge {
    int to;
    int val;
    Edge(int to, int val) : to(to), val(val) {};
};

int n,m;
vector<Edge> g[MX];
int dis[MX];
int from[MX];

void bellman_ford(int rt) {
    for(int i = 1;i <= n;i++) {
        dis[i] = INF;
        from[i] = -1;
    }
    dis[rt] = 0;

    while(true) {
        bool changed = false;
        for (int u = 1; u <= n; u++) {
            if(dis[u] == INF) continue;
            for(auto& e: g[u]) {
                int v = e.to;
                int w = e.val;
                if(dis[v] > dis[u] + w) {
                    dis[v] = dis[u] + w;
                    from[v] = u;
                    changed = true;
                }
            }
        }
        if(!changed) break;
    }
}

int main() {

    std::istringstream input(
R"(
11 22
1 2 2
1 3 8
1 4 1
2 5 1
3 5 5
3 6 1
3 7 2
4 7 9
2 3 6
3 4 7
5 6 3
6 7 4
5 8 2
5 9 9
6 9 6
7 9 3
7 10 1
8 9 7
9 10 1  
9 11 2
8 11 9
10 11 4
)");
    std::cin.rdbuf(input.rdbuf()); // 重定向 cin
    cin >> n >> m;
    for(int i = 1;i <= m;i++) {
        int u,v,w;
        cin >> u >> v >> w;
        g[u].emplace_back(v,w);
        g[v].emplace_back(u,w);
    }
    bellman_ford(1);
    for(int i = 1;i <= n;i++) {
        cout << "Vertix" << i << " : " << dis[i] << " from : " << from[i] << endl;
    }
}

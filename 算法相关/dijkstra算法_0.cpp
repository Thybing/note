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

int n, m;

vector<Edge> g[MX];

int dist[MX];
int from[MX];
bool vst[MX];

void dijkstra(int rt) {
    for (int i = 1; i <= n; i++) {
        dist[i] = INF;
        from[i] = -1;
        vst[i] = false;
    }
    dist[rt] = 0;
    for (int i = 1; i <= n; i++) {
        int min_dis_v;
        int min_dis = INF;
        for (int j = 1; j <= n; j++) {
            if (vst[j]) {
                continue;
            }
            if(min_dis > dist[j]) {
                min_dis_v = j;
                min_dis = dist[j];
            }
        }
        if(min_dis == INF) {
            cout << "No Connectivity" << endl;
            return;
        }
        vst[min_dis_v] = true;
        for(auto e : g[min_dis_v]) {
            if(vst[e.to]) continue;

            if(dist[e.to] > dist[min_dis_v] + e.val) {
                dist[e.to] = dist[min_dis_v] + e.val;
                from[e.to] = min_dis_v;
            }
        }
    }
}

int main() {
    // 这是一个测试样例
    std::istringstream input(
R"(
11 22 1
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

    int rt;
    cin >> n >> m >> rt;
    int u, v, d;
    for (int i = 1; i <= m; i++) {
        cin >> u >> v >> d;
        g[u].emplace_back(v, d);
        g[v].emplace_back(u, d);
    }
    dijkstra(rt);
    for(int i = 1;i <= n;i++) {
        cout << "vertix" << i << " : " << dist[i] << " from : " << from[i] << endl;
    }
}
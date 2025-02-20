#include <iostream>
#include <vector>
#include <queue>

#include <sstream>

using namespace std;

constexpr int MX = 1000;
constexpr int INF = 0x3f3f3f3f;

struct Edge {
    int to;
    int val;
    Edge(int to, int val) : to(to), val(val){};
};

int n,m;
vector<Edge> g[MX];

bool in_queue[MX];
int dist[MX];
int from[MX];

int cnt[MX]; // 用于记录从rt到点u的路径数，大于等于节点数说明其中有负环

bool spfa(int rt) {
    for(int i = 1;i <= n;i++) {
        in_queue[i] = false;
        dist[i] = INF;
        from[i] = -1;
        cnt[i] = 0;
    }
    queue<int> q; 

    q.push(rt);
    in_queue[rt] = true;
    dist[rt] = 0;

    while(q.size()) {
        int u = q.front();
        q.pop();
        for(auto e:g[u]) {
            int v = e.to;
            if(dist[v] > dist[u] + e.val) {
                dist[v] = dist[u] + e.val;
                from[v] = u;
                cnt[v] = cnt[u] + 1;
                if(cnt[v] >= n) return false;
                if(!in_queue[v]) {
                    q.push(v);
                }
            }
        }
        in_queue[u] = false;
    }
    return true;
}

int main() {
    cout << "spfa" << endl;
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
// 1 2 -1
// 2 3 -1
// 3 1 -1
    std::cin.rdbuf(input.rdbuf()); // 重定向 cin
    cin >> n >> m;
    for(int i = 1;i <= m;i++) {
        int u,v,w;
        cin >> u >> v >> w;
        g[u].emplace_back(v,w);
        g[v].emplace_back(u,w);
    }

    if(!spfa(1)) {
        cout << "图中有负环" << endl;
        return 0;
    }

    for(int i = 1;i <= n;i++) {
        cout << "Vertix" << i << " : " << dist[i] << " from : " << from[i] << endl;
    }
}
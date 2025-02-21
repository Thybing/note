#include <iostream>
#include <map>
#include <queue>

#include <sstream>

constexpr int MX = 1e5 + 5;
constexpr int INF = 0x3f3f3f3f;

using namespace std;
using pii = pair<int,int>;

struct Edge {
    int to;
    int val;
    Edge(int to,int val) : to(to),val(val) {}
};

bool vis[MX];
int dis[MX];

vector<Edge> g[MX];

int n,m;

void dijkstra(int rt) {    
    for(int i = 1;i <= n;i++) {
        vis[i] = false;
        dis[i] = INF;
    }
    priority_queue<pii> hp;
    dis[rt] = 0;
    hp.push(pii(0,rt));

    int cnt = 0;
    while(hp.size()) {
        pii top = hp.top();
        hp.pop();
        int u = top.second;
        if(vis[u]) {
            continue;
        }
        vis[u] = true;
        cnt++;
        if(cnt >= n) break;
        for(auto e : g[u]) {
            int v = e.to;
            int w = e.val;
            if(dis[v] > dis[u] + w) {
                dis[v] = dis[u] + w;
                hp.push(pii(-dis[v],v));
            }
        }
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
        g[u].push_back(Edge{v,w});
        g[v].push_back(Edge{u,w});
    } 
    dijkstra(1);
    for(int i = 1;i <= n;i++) {
        cout << "vertix " << i << " : " << dis[i] << endl;
    }
}
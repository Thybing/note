#include <iostream>
#include <vector>

#include <sstream>

using namespace std;

// 这里要求输入保证只有一个连通分量
// 图和树的节点都是从1开始编号

constexpr int N = 1e4 + 5;
constexpr int inf = 0x3f3f3f3f;

struct Edge{
    int to;
    int val;
    Edge(int to,int val):to(to),val(val){};
};

bool visited[N];
int fa[N],faLen[N];
int mincost[N];

vector<Edge> g[N];

void prim(int start,int numV) {
    // init
    for(int i = 1;i <= numV;i++) {
        visited[i] = false;
        mincost[i] = inf;
        fa[i] = -1;
    }

    // add root
    visited[start] = true;
    fa[start] = -1;
    faLen[start] = 0;
    for(auto& edge : g[start]) {
        if(mincost[edge.to] > edge.val) {
            mincost[edge.to] = edge.val;
            fa[edge.to] = start;
        }
    }

    // repeat(numV - 1)
    int sum = 0;
    for (int i = 1;i <= numV - 1;i++) {
        // find min veritx
        int min = inf;
        int min_vertix = 0;
        for(int j = 1;j <= numV;j++) {
            if (!visited[j] && mincost[j] < min){
                min = mincost[j];
                min_vertix = j;
            }
        }

        //add min vertix
        visited[min_vertix] = true;
        faLen[min_vertix] = min;
        sum += min;
        // refresh fa[]
        for(auto& edge: g[min_vertix]) {
            if(!visited[edge.to] && mincost[edge.to] > edge.val) {
                mincost[edge.to] = edge.val;
                fa[edge.to] = min_vertix;
            }
        }

        // cout << "add:" << min_vertix << endl;
    }
    // cout << "sum edge len : " << sum << endl;

}

int main() {
    // 这是一个测试样例
    std::istringstream input(
R"(10 20
1 2 5
1 3 3
1 4 7
2 3 4
2 5 6
3 4 2
3 5 8
3 6 10
4 6 9
4 7 4
5 6 5
5 8 7
6 7 6
6 9 8
7 8 2
7 9 3
7 10 6
8 9 4
9 10 5
8 10 7
)");
    std::cin.rdbuf(input.rdbuf()); // 重定向 cin，将我的测试样例输入进去

    int n,m; // 假设有n个顶点，m条边    
    cin >> n >> m;
    int a,b,v;
    for (int i = 1;i <= m;i++) {
        cin >> a >> b >> v;
        g[a].emplace_back(b,v);
        g[b].emplace_back(a,v);
    }
    // 上面是vector存图(邻接表)
    prim(1,n);

    for(int i = 1;i <= n;i++) {
        cout << fa[i] << "--" << i << "  len:" << faLen[i] << endl;
    }
}
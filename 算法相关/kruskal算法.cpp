#include <iostream>
#include <queue>
#include <set>

#include <sstream>

using namespace std;

constexpr int N = 1e5 + 5;

struct Edge{
    int from;
    int to;
    int val;
    Edge(int from,int to,int val):from(from),to(to),val(val){};
};

struct EdgeLess{
    bool operator()(Edge& e1,Edge& e2) {
        return e1.val > e2.val;
    }
};

int n,m; // 有n个顶点，m条边    

priority_queue<Edge,vector<Edge>,EdgeLess> hp;
vector<Edge> treeEdge;
int fa[N];

int findFa(int x){
    if(fa[x] == x) {
        return x;
    }else {
        fa[x] = findFa(fa[x]);
        return fa[x];
    }
}

void un(int x,int y) {
    fa[findFa(x)] = findFa(y);
}

void kruskal() {
    // 初始化并查集
    for(int i = 1;i <= n;i++) {
        fa[i] = i;
    }

    while(true) {
        auto e = hp.top();
        hp.pop();
        if(findFa(e.from) == findFa(e.to)) {
            continue;
        }
        treeEdge.push_back(e);
        un(e.from,e.to);
        if(treeEdge.size() == n - 1) {
            break;
        }
    }
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
    std::cin.rdbuf(input.rdbuf()); // 重定向 cin，将测试样例输入进去

    cin >> n >> m;

    int x,y,v;
    for(int i = 1;i <= m;i++) {
        cin >> x >> y >> v;
        hp.emplace(x,y,v);
    }

    kruskal();

    for(auto& e:treeEdge) {
        cout << e.from << "--" << e.to << " val : " << e.val << endl;
    }
}
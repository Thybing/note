#include <iostream>
#include <vector>

constexpr int N = 1e5 + 5;

struct Edge{
    int to;
    int value;
    Edge(int to,int value):to(to),value(value){};
};

std::vector<Edge> graph[N];

int main() {
    // 存入一条有向的边，从节点1到节点2，边权为666
    graph[1].emplace_back(2,666);

    // 存入一条无向的边，连接节点1和节点2，边权为666
    graph[1].emplace_back(2,666);
    graph[2].emplace_back(1,666);
}
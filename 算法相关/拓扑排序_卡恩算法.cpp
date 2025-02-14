#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>
#include <sstream>

using namespace std;

constexpr int MX = 1e5 + 5;

int n,m;

vector<int> g[MX];
int indegree_num[MX];

vector<int> kahn() {
    vector<int> ret;
    queue<int> zero_in;
    for(int i = 1;i <= n;i++) {
        if(indegree_num[i] == 0) {
            zero_in.push(i);
        }
    }
    while(!zero_in.empty()) {
        auto v = zero_in.front();
        zero_in.pop();
        ret.push_back(v);
        for(auto each:g[v]) {
            indegree_num[each]--;
            if(indegree_num[each] == 0) {
                zero_in.push(each);
            }
        }
    }
    return std::move(ret);
}

int main() {
    stringstream input(R"(
7 9
1 2
2 7
2 4
3 2
4 7
3 4
3 5
5 6
4 6
)");
    cin.rdbuf(input.rdbuf());

    cin >> n >> m;
    for(int i = 1;i <= m;i++) {
        int u,v;
        cin >> u >> v;
        g[u].push_back(v);
        indegree_num[v]++;
    }
    auto seq = kahn();
    copy(seq.begin(),seq.end(),ostream_iterator<int>(cout," "));
}
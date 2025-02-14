#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

using namespace std;

constexpr int MX = 1e5 + 5;

int n, m;

enum class status : int {
    to_visited = 0,
    visiting = 1,
    visited = 2,
};

vector<int> g[MX];

status s[MX];

vector<int> seq;

bool dfs(int v) {
    if (s[v] == status::visited)
        return true;
    if (s[v] == status::visiting)
        return false;
    s[v] = status::visiting;
    for(auto each_v : g[v]) {
        if(!dfs(each_v)) return false;
    }
    seq.push_back(v);
    s[v] = status::visited;
    return true;
}

bool toposort() {
    for (int i = 1; i <= n; i++) {
        s[i] = status::to_visited;
    }
    for (int i = 1; i <= n; i++) {
        if (!dfs(i))
            return false;
    }
    reverse(seq.begin(), seq.end());
    return true;
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
    for (int i = 1; i <= m; i++) {
        int u, v;
        cin >> u >> v;
        g[u].push_back(v);
    }
    if (!toposort()) {
        cout << "not a DAG!!!" << endl;
        return 0;
    }
    copy(seq.cbegin(), seq.cend(), ostream_iterator<int>(cout, " "));
}
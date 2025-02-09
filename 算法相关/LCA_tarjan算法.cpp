#include <iostream>
#include <map>
#include <sstream>
#include <vector>

using namespace std;

using pii = pair<int, int>;

constexpr int MX = 1e5 + 5;

int n, m, rt;

int dsu[MX];
int vst[MX];
vector<pii> query[MX];
int ans[MX];

vector<int> tree[MX];

int fd(int x) {
    if(dsu[x] == x) {
        return x;
    }
    else {
        dsu[x] = fd(dsu[x]);
        return dsu[x];
    }
}

void tarjan(int cur,int fa) {
    for(auto son:tree[cur]) {
        if(son == fa) {
            continue;
        }
        tarjan(son,cur);
        dsu[son] = cur;
    }
    vst[cur] = true;

    for(auto cur_query:query[cur]) {
        int v = cur_query.first;
        int query_idx = cur_query.second;
        if(vst[v]) {
            ans[query_idx] = fd(v);
        }
    }
}

int main() {
    cin >> n >> m >> rt;
    for (int i = 1; i <= n; i++) {
        dsu[i] = i;
    }
    int u, v;
    for (int i = 1; i <= n - 1; i++) {
        cin >> u >> v;
        tree[u].push_back(v);
        tree[v].push_back(u);
    }
    for (int i = 1;i <= m;i++) {
        cin >> u >> v;
        query[u].emplace_back(v,i);
        query[v].emplace_back(u,i);
    }
    tarjan(rt,0);
    for(int i = 1;i <= m;i++) {
        cout << ans[i] << endl;
    }
}
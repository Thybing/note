#include <iostream>
#include <vector>

using namespace std;

constexpr int N = 1e5 + 5;

int n,rt;

vector<int> t[N];

int fas[N][20];
int deeps[N];

void lca_ini(int me,int fa,int deep) {
    fas[me][0] = fa;
    deeps[me] = deep;
    for(int k = 1;k <= 19;k++) {
        fas[me][k] = fas[fas[me][k - 1]][k - 1];
        // 注：由于这个递归是先根的，所以fas[fas[me][k - 1]]的一整列全都是已经计算过的，可以放心进行状态转移，没有计算顺序上的问题。
    }
    for(auto son:t[me]) {
        if(son == fa) {
            continue;
        }
        lca_ini(son,me,deep + 1);
    }
}

int lca(int u,int v){
    if(deeps[u] < deeps[v]) {
        int tmp = u;
        u = v;v = tmp;
    }

    for(int i = 19;~i;i--) {
        if(deeps[fas[u][i]] >= deeps[v]) {
            u = fas[u][i];
        }
    }

    if(u == v) return u;

    for(int i = 19;~i;i--) {
        if(fas[u][i] != fas[v][i]) {
            u = fas[u][i];
            v = fas[v][i];
        }
    }

    return fas[u][0];
}

int main() {
    cin >> n >> rt;
    int u,v;
    for(int i = 1;i <= n - 1;i++) {
        cin >> u >> v;
        t[u].push_back(v);
        t[v].push_back(u);
    }

    deeps[0] = 0;
    for(int k = 0;k <= 19;k++) {
        fas[0][k] = 0;
    }
    lca_ini(rt,0,1);

    while(cin >> u >> v) {
        cout << lca(u,v) << endl;
    }
}

#include <iostream>
#include <vector>

#include <sstream>

using namespace std;

constexpr int MX = 1e3 + 5;

constexpr int INF = 0x3f3f3f3f;

int n,m;

int mat[MX][MX];
int trans[MX][MX];

void floyd() {
    for(int i = 1;i <= n;i++) {
        for(int j = 1;j <= n;j++) {
            for(int k = 1;k <= n;k++) {
                if(mat[j][i] + mat[i][k] < mat[j][k]) {
                    mat[j][k] = mat[j][i] + mat[i][k];
                    trans[j][k] = i;
                }
            }
        }
    }
}

void calc_path(int u,int v,vector<int>& path) {
    if(path.empty()) {
        path.push_back(u);
    }
    if(trans[u][v] != u && trans[u][v] != v) {
        calc_path(u,trans[u][v],path);
        calc_path(trans[u][v],v,path);
    } else {
        path.push_back(v);
    }
}

int main() {
    istringstream input(R"(
7 12
1 2 12
1 6 16
1 7 14
2 6 7
7 6 9
2 3 10
5 7 8
3 5 5
3 4 3
4 5 4
5 6 2
3 6 6
)");
    cin.rdbuf(input.rdbuf());
    cin >> n >> m;
    for(int i = 1;i <= n;i++) {
        for(int j = 1;j <= n;j++) {
            mat[i][j] = INF;
            trans[i][j] = j;
        }
    }
    for(int i = 1;i <= n;i++) {
        mat[i][i] = 0;
    }

    int u,v;
    for(int i = 1;i <= m;i++) {
        cin >> u >> v;
        cin >> mat[u][v];
        mat[v][u] = mat[u][v];
    }
    floyd();

    for(int i = 1;i <= n;i++) {
        for(int j = 1;j <= n;j++) {
            cout << mat[i][j] << '\t';
        }
        cout << endl;
    }

    cout << endl;

    for(int i = 1;i <= n;i++) {
        for(int j = 1;j <= n;j++) {
            cout << trans[i][j] << '\t';
        }
        cout << endl;
    }

    vector<int> path;
    calc_path(1,4,path);
    for(auto v:path) {
        cout << v << "->";
    }
    cout << "NULL" << endl;
}
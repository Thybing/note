#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int n;
int a[100005];
vector<int> select_arr;
int ans = 0;

bool check() {
    return (adjacent_find(select_arr.begin(),select_arr.end(),[](int a,int b){return a >= b;}) == select_arr.end());
}

void DFS(int x) {
    // x = 1 ~ n
    if(x == n + 1) {
        if(check() && ans < select_arr.size()) {
            ans = select_arr.size();
        }
        return;
    }

    //
    DFS(x + 1);

    //
    select_arr.push_back(a[x]);
    DFS(x + 1);
    select_arr.pop_back();

}

int main() {
    cin >> n;
    for(int i = 1;i <= n;i++) {
        cin >> a[i];
    }
    DFS(1);
    cout << ans << endl;
}

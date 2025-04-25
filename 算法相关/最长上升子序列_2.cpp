#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
constexpr int INF = 0x3f3f3f3f;

int n;
int a[5005];
int b[5005];
int ans = 0;

void solve() {
    if (n == 0) {
        cout << 0 << endl;
        return;
    }
    b[1] = a[1];
    ans = 1;
    for(int i = 2;i <= n;i++) {
        auto p = lower_bound(begin(b) + 1,begin(b) + ans + 1, a[i]);
        if(p == b + ans + 1) {
            ans++;
        }
        *p = a[i];
    }
    cout << ans << endl;
}

int main() {
    cin >> n;
    for(int i = 1;i <= n;i++) {
        cin >> a[i];
    }
    solve();
}

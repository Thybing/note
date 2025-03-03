#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int n;
int a[5005];
int l[5005];
int ans = 0;

void solve() {
    for(int i = 1;i <= n;i++) {
        for(int j = 1;j < i;j++) {
            if(a[j] < a[i]) {
                l[i] = max(l[i],l[j] + 1);
            }
        }
        ans = max(ans,l[i]);
    }
    cout << ans << endl;
}

int main() {
    cin >> n;
    for(int i = 1;i <= n;i++) {
        cin >> a[i];
        l[i] = 1;
    }
    solve();
}

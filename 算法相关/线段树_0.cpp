#include <iostream>

// 洛谷P3374，无懒标记的线段树

using namespace std;
using ll = long long;

ll f[2000020], lz[2000020];
ll a[500005];

void init_tree(int idx, int l, int r) {
    if (l == r) {
        f[idx] = a[l];
        return;
    }
    int m = (l + r) / 2;
    init_tree(idx + idx, l, m);
    init_tree(idx + idx + 1, m + 1,r);
    f[idx] = f[idx + idx] + f[idx + idx + 1];
}

void update(int idx, int l, int r, int x, int k) {
    if(l == r) {
        f[idx] += k;
        return;
    }
    else {
        f[idx] += k;
        int m = (l + r) / 2;
        if(x <= m) {
            update(idx + idx,l,m,x,k);
        }else {
            update(idx + idx + 1,m + 1,r,x,k);
        }
    }
}

ll query(int idx,int l,int r, int s, int t){
    if(l == s && r == t) {
        return f[idx];
    }
    int m = (l + r) / 2;
    if (t <= m) {
        return query(idx + idx,l,m,s,t);
    } else if(s >= m + 1) {
        return query(idx + idx + 1,m + 1,r,s,t);
    }else {
        return query(idx + idx,l,m,s,m)
            + query(idx + idx + 1,m + 1,r,m + 1,t);
    }
}

int main() {
    ll n,m; cin >> n >> m;
    for(int i = 1;i <= n;i++) {
        cin >> a[i];
    }
    init_tree(1,1,n);
    for(int i = 0;i < m;i++) {
        int s,x,y;
        cin >> s >> x >> y;
        if (s == 1) {
            update(1,1,n,x,y);
        }else {
            cout << query(1,1,n,x,y) << endl;
        }    
    }
    return 0;
}
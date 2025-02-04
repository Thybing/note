#include <iostream>

// 洛谷P3372

using namespace std;
using ll = long long;

ll a[100005], f[100005 * 4], lz[100005 * 4];

void init_tree(int idx, int l, int r) {
    lz[idx] = 0;
    if (l == r) {
        f[idx] = a[l];
        return;
    }
    int m = (l + r) / 2;
    init_tree(idx + idx, l, m);
    init_tree(idx + idx + 1, m + 1, r);
    f[idx] = f[idx + idx] + f[idx + idx + 1];
}

void down_lazy(int idx,int l,int r) {
    f[idx] += (r - l + 1) * lz[idx];
    if(l != r) {
        lz[idx + idx] += lz[idx];
        lz[idx + idx + 1] += lz[idx];
    }
    lz[idx] = 0;
}

void update(int idx, int l, int r, int s, int t, ll k) {
    if(l == s && r == t) {
        lz[idx] += k;
        return;
    }
    else{
        down_lazy(idx,l,r);
        f[idx] += (t - s + 1) * k;
        int m = (l + r) / 2;
        if(t <= m) {
            update(idx + idx,l,m,s,t,k);
        } else if(s >= m + 1) {
            update(idx + idx + 1,m + 1,r,s,t,k);
        } else {
            update(idx + idx,l,m,s,m,k);
            update(idx + idx + 1,m + 1,r,m + 1,t,k);
        }
    }
}

ll query(int idx,int l,int r,int s,int t) {
    down_lazy(idx,l,r);
    if (l == s && r == t) {
        return f[idx];
    }
    int m = (l + r) / 2;
    if(t <= m) {
        return query(idx + idx,l,m,s,t);
    } else if (s >= m + 1) {
        return query(idx + idx + 1,m + 1,r,s,t);
    } else {
        return query(idx + idx,l,m,s,m) 
            + query(idx + idx + 1,m + 1,r,m + 1,t);
    }
}

int main() {
    int n, m;
    cin >> n >> m;
    for (int i = 1; i <= n; i++) {
        cin >> a[i];
    }
    init_tree(1, 1, n);

    ll s, x, y, k;

    for (int i = 1; i <= m; i++) {
        cin >> s >> x >> y;
        if (s == 1) {
            cin >> k;
            update(1,1,n,x,y,k);
        } else {
            cout << query(1,1,n,x,y) << endl;
        }
    }
    return 0;
}
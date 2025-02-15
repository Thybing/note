#include <iostream>

// 洛谷P3373

using namespace std;
using ll = long long;


ll a[100005];
ll f[100005 * 4];
ll lz_add[100005 * 4], lz_mut[100005 * 4];

ll n, q , p;

void init_tree(int idx, int l, int r) {
    lz_add[idx] = 0;
    lz_mut[idx] = 1;
    if (l == r) {
        f[idx] = a[l] % p;
        return;
    }
    int m = (l + r) / 2;
    init_tree(idx + idx, l, m);
    init_tree(idx + idx + 1, m + 1, r);
    f[idx] = (f[idx + idx] + f[idx + idx + 1]) % p;
}

void down_lazy(int idx,int l, int r) {
    int m = (l + r) / 2;
    f[idx + idx] = (f[idx + idx] * lz_mut[idx] + lz_add[idx] * (m - l + 1)) % p;
    f[idx + idx + 1] = (f[idx + idx + 1] * lz_mut[idx] + lz_add[idx] * (r - m)) % p;

    lz_add[idx + idx] = (lz_add[idx + idx] * lz_mut[idx] + lz_add[idx]) % p;
    lz_mut[idx + idx] = (lz_mut[idx + idx] * lz_mut[idx]) % p;

    lz_add[idx + idx + 1] = (lz_add[idx + idx + 1] * lz_mut[idx] + lz_add[idx]) % p;
    lz_mut[idx + idx + 1] = (lz_mut[idx + idx + 1] * lz_mut[idx]) % p;

    lz_add[idx] = 0;
    lz_mut[idx] = 1;
}

void update_add(int idx, int l, int r, int s, int t, ll k) {
    if(l == s && r == t) {
        f[idx] = (f[idx] + k * (r - l + 1)) % p;
        lz_add[idx] = (lz_add[idx] + k) % p;
        return;
    }
    down_lazy(idx,l,r);
    int m = (l + r) / 2;
    if(t <= m) {
        update_add(idx + idx,l,m,s,t,k);
    } else if(s >= m + 1) {
        update_add(idx + idx + 1,m + 1,r,s,t,k);
    } else {
        update_add(idx + idx,l,m,s,m,k);
        update_add(idx + idx + 1,m + 1,r,m + 1,t,k);
    }
    f[idx] = (f[idx + idx] + f[idx + idx + 1]) % p;
}

void update_mut(int idx, int l, int r, int s, int t, ll k) {
    if(l == s && r == t) {
        f[idx] = (f[idx] * k) % p;
        lz_add[idx] = (lz_add[idx] * k) % p;
        lz_mut[idx] = (lz_mut[idx] * k) % p;
        return;
    }
    down_lazy(idx,l,r);
    int m = (l + r) / 2;

    if(t <= m) {
        update_mut(idx + idx,l,m,s,t,k);
    } else if (s >= m + 1) {
        update_mut(idx + idx + 1,m + 1,r,s,t,k);
    } else {
        update_mut(idx + idx,l,m,s,m,k);
        update_mut(idx + idx + 1,m + 1,r,m + 1,t,k);
    }
    f[idx] = (f[idx + idx] + f[idx + idx + 1]) % p;
}

ll query(int idx, int l, int r, int s, int t){
    if (l == s && r == t) {
        return f[idx] % p;
    }
    down_lazy(idx,l,r);
    int m = (l + r) / 2;
    if(t <= m) {
        return query(idx + idx,l,m,s,t) % p;
    } else if (s >= m + 1) {
        return query(idx + idx + 1,m + 1,r,s,t) % p;
    } else {
        return (query(idx + idx,l,m,s,m) + query(idx + idx + 1,m + 1,r,m + 1,t)) % p;
    }
}

int main() {
    cin >> n >> q >> p;
    for (int i = 1; i <= n; i++) {
        cin >> a[i];
    }
    init_tree(1, 1, n);
    ll s, x, y, k;
    for (int i = 1; i <= q; i++) {
        cin >> s >> x >> y;
        if (s == 1) {
            cin >> k;
            update_mut(1, 1, n, x, y, k);
        } else if (s == 2) {
            cin >> k;
            update_add(1, 1, n, x, y, k);
        } else {
            cout << (query(1, 1, n, x, y)) % p << endl;
        }
    }
    return 0;
}
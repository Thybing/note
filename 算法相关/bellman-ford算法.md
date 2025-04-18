# bellman-ford算法(及其优化算法SPFA)

贝尔曼-福特算法是除了dijkstra算法以外的另一种单源最短路径算法。
对比dijkstra，有一个很显著的优点是，它可以处理负权边。

## 算法实现

执行多轮循环(最坏情况下循环次数为顶点数)，每一轮循环中对所有的边进行一次松弛操作，如果一轮循环中没有成功的松弛操作，那么算法结束。
(如果在执行了顶点数次数后再执行一遍，还出现了成功的松弛操作，说明图中存在负环)

## cpp实现

[bellman-ford算法](./bellman-ford算法.cpp)

## 复杂度分析

每轮循环中，都会枚举所有点的所有出边，所以时间复杂度为O(VE)

## SPFA算法

只有本轮被更新的点的出边，才有可能在下一轮引起松弛操作。
所以我们可以用队列来维护被更新的点的集合。然后每次从队头取出顶点u，枚举u的所有出边，对出边指向的顶点v进行松弛操作，然后如果成功松弛，则将v入队。

为了进一步增加效率，如果我们发现顶点v已经在队内了，就无需再一次在队内添加一次顶点v。可以用一个数组来维护顶点是否在队内。

另外，如何去判断是否存在负权环？我们想到，如果出现了负权环，那么算法进行的过程中，一定会发现从源点走向负权环中的一个顶点，所经过的边数是无穷大的，即会永远的在这个环中循环，从而减小路径和。
但是在无负权环的图中，从一个顶点a到另一个顶点b，一定不会超过numV - 1条边，即经过了所有的节点。
所以我们可以记录一下在松弛的过程中从源点到点v经过的边的数量，
并且很显然cnt[v] = cnt[u] + 1
如果我们发现cnt[v] > (numV - 1)
那么显然的，在图中出现了负环，结束算法。

## cpp实现(SPFA)

[spfa算法](./bellman-ford算法_spfa.cpp)

## 复杂度分析（这个复杂度分析有点超纲吧）

SPFA算法的复杂度大约是O(kE)，其中k是每个节点的平均入队的次数。一般来说，在稀疏图内，k是小于2的。
SPFA算法的复杂度变化很大，最坏情况下，时间复杂度达到了和bellman-ford相同的O(VE)。

好像算法题中会经常卡spfa?除非有负权还是优先用堆优化dijkstra吧

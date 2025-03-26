# 010 select

select可以让程序在多个文件描述符上等待数据，并且无需阻塞其中的任何一个。常用于网络编程和非阻塞io场景。

## 函数原型
```cpp
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
参数：
- nfds 监听的文件描述符的最大值 + 1 (一般来说上限是1024，与glibc中FD_SETSIZE的定义相同)
- readfds 需要监听可读的文件描述符集合，fd_set*类型
- writefds 需要监听可写的文件描述符集合，fd_set*类型
- exceptfds 需要监听异常时间的文件描述符集合，fd_set*类型
- timeout 超时时间(NULL表示无限等待直到有被监听的fd触发select)，timeval*类型
```c
struct timeval timeout;
timeout.tv_sec = 0;   // 秒
timeout.tv_usec = 0; // 微秒
```

返回值：
- \>0 有监听的io事件发生的数量
- 0 超时
- -1 发生错误，并设置errno

## fd_set
fd_set 是一个1024(FD_SETSIZE)长度的bitset位图，用来表示若干个fd的集合
```cpp
// glibc
typedef struct
  {
#ifdef __USE_XOPEN
    __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->fds_bits)
#else
    __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->__fds_bits)
#endif
  } fd_set;
```

## fd_set的相关操作
- FD_SET(fd, &fdset);   // 将 fd 加入 fdset 监听
- FD_CLR(fd, &fdset);   // 从 fdset 移除 fd
- FD_ISSET(fd, &fdset); // 检查 fd 是否在 fdset 中（是否有事件发生）
- FD_ZERO(&fdset);      // 清空 fdset

这些操作本质上都是位运算

## example

监听标准输入
```cpp
#include <iostream>
#include <sys/select.h>
#include <unistd.h>

int main() {
    fd_set readfds;
    char buffer[1024];

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        // 只监听可读事件
        int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
        if (ret > 0) {
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
                if (bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    std::cout << "输入: " << buffer << std::endl;
                } else if (bytesRead == 0) {
                    std::cout << "输入已关闭 (EOF)\n";
                    break;
                }
            }
        }
    }
    return 0;
}

```

我们在调用select之前，要先初始化三个fd_set位图，然后用FD_SET来指定我们感兴趣的io事件。
然后将三个fd_set传入select,select去监听我们设置感兴趣的fd的可读/可写/异常状态
当返回时，select会将我们传入的三个fd_set进行位运算，仅保留触发了select的fd。
然后，我们检查返回值。如果大于0，我们可以去遍历所有的感兴趣的监听，用FD_ISSET去判断是否是该fd触发了对应的可读/可写/异常。

select会对传入的fd_set进行修改，所以，如果想要循环用select监听，记得要在下一次调用select前，将fd_set去重新FD_SET一遍，来指定我们感兴趣的io事件，或者直接将原来的fd_set拷贝一份再传入，下一次就不需要再重新设置。

## select到底监听什么？

select() 监听的是文件描述符的状态变化，主要分为以下三类：

- 可读（readable）：如果 fd 上的数据可读（如 recv() 不会阻塞、标准输入有数据可读等）。

- 可写（writable）：如果 fd 上可以写数据（如 send() 不会阻塞）。

- 异常（exceptional condition）：如果 fd 上发生了异常（如带外数据，常用于 TCP 带外数据 OOB）。

### 监听可读（readfds）时：

- 普通文件、管道：文件尾部 EOF 也算可读（read() 返回 0）。

- 套接字（socket）：

    - 有数据可读（recv() 不会阻塞）。

    - 连接已关闭（recv() 返回 0）。

    - 有新的连接请求（监听套接字 listen_fd 可读）。

### 监听可写（writefds）时：

- 套接字：send() 不会阻塞（发送缓冲区有足够空间）。（大部分情况下是可写的，select调用后会立即返回）

### 监听异常（exceptfds）时：

- 带外数据（Out-of-Band Data, OOB）。

- 错误情况（如 EPOLLERR）。

## select返回时，fd触发表示什么？

当 select() 返回后，你需要检查哪些 fd 触发了事件：
（注意，select是水平触发的，所以如果触发了之后没有进行处理或没有处理完，那么下次调用select进行监听还会立即返回）
### 如果 FD_ISSET(fd, &readfds)：

- 普通文件/管道：read(fd, buf, size) 可以立刻读取数据。

- 监听套接字：有新的连接请求，可以调用 accept(fd, ...)。

- 连接套接字：对端有数据发送过来，可以调用 recv(fd, buf, size, 0)。

- 对端关闭连接：recv(fd, buf, size, 0) 返回 0。

### 如果 FD_ISSET(fd, &writefds)：

- send(fd, buf, size, 0) 不会阻塞，数据可以立刻发送。

### 如果 FD_ISSET(fd, &exceptfds)：

- 可能有带外数据（OOB），或者发生异常（需要 getsockopt() 获取错误信息）

## select的问题
每次都需要重新初始化 fd_set，并且 select() 修改 fd_set 后必须重新设置。
select() 的 fd 数量受 FD_SETSIZE 限制，默认 1024（可以修改但不推荐）。
性能问题：select() 每次调用都要遍历整个 fd_set，在高并发情况下性能较低。
线程不安全！多个线程对同一个fd_set操作是UB的，可能会有冲突，多线程环境下，应该每个线程维护自己的fd_set。并且保证多线程监听的fd_set相与为0，即不同线程不能监听同一个fd！！！（一个线程监听可读，另一个线程监听同一个FD的可写也可能导致相互影响）

## 总结：socket在网络编程中的作用

监听服务器的监听sockfd，如果触发说明有新连接，可以accept
监听accept返回的客户端sockfd，如果触发说明有新数据可以recv。(关闭连接也会触发)

### 为什么 socket 需要 select？
在传统的阻塞式 socket 编程 中：
- accept() 阻塞等待新连接
- recv() 阻塞等待数据
- send() 可能阻塞，等待缓冲区可用

这会导致：
- 无法同时处理多个 socket（一个 recv() 被阻塞，其他 socket 也无法被监听）
- 需要使用多线程/多进程（每个 socket 一个线程/进程，但开销大）

✅ select 解决了这个问题

让单个线程/进程 同时监听多个 socket
只在 某个 socket 真的可读/可写/异常 时才去处理
避免不必要的阻塞，提高性能

### select 在 socket 编程中的作用
select 主要用于 同时监听多个 socket 事件，通常用于：
监听多个客户端 socket（典型的 TCP 服务器）
监听多个数据源（如标准输入 + 网络 socket）
实现非阻塞 socket 通信
低并发的 I/O 复用（select 适用于 少量 socket，高并发推荐 epoll）

### select 处理 socket 的典型流程
服务器端：同时监听多个 socket
创建 监听 socket
bind() 绑定端口，listen() 开启监听
使用 select() 监听多个 socket
当 select 返回，检查哪些 socket 触发了事件：
如果是 监听 socket，说明有新连接，accept()
如果是 客户端 socket，说明有数据，recv()
如果是 客户端断开，关闭 socket，FD_CLR() 移除监听

给出一个例子：
```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>
#include <vector>

#define PORT 8080

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = {AF_INET, htons(PORT), INADDR_ANY};
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    fd_set readfds;
    std::vector<int> clients;
    int max_fd = server_fd;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        for (int client : clients) FD_SET(client, &readfds);

        int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ret > 0) {
            if (FD_ISSET(server_fd, &readfds)) { // 监听 socket 有新连接
                int new_client = accept(server_fd, NULL, NULL);
                clients.push_back(new_client);
                max_fd = std::max(max_fd, new_client);
                std::cout << "新连接: " << new_client << std::endl;
            }
            for (auto it = clients.begin(); it != clients.end(); ) {
                if (FD_ISSET(*it, &readfds)) { // 客户端有数据
                    char buffer[1024];
                    ssize_t len = recv(*it, buffer, sizeof(buffer), 0);
                    if (len > 0) {
                        std::cout << "收到数据: " << buffer << std::endl;
                    } else { // 断开连接
                        close(*it);
                        it = clients.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }
    }
}
```

### select 的局限性
✅ 适用于：
- 低并发（fd 数量较少，<1024）
- 不希望用多线程（单线程管理多个 socket）
- 简单网络服务器或客户端

❌ 局限性
- FD_SETSIZE 限制单个进程可监听的fd数量（默认 1024，可修改但不推荐）
- 性能问题：
    - 每次 select都要 遍历整个 fd 集合,O(n) 复杂度，耗时长
    - fd_set每次调用都需要进行重新设置或拷贝（另外还要拷贝到内核中）。
- 综上select不适合高并发。

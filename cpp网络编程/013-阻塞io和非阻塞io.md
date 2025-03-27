# 013  阻塞IO和非阻塞IO

select/poll/epoll这三种多路复用机制，通常被使用在事件循环中。如果这个循环被长时间阻塞，那么就无法处理其它客户端发来的请求。

例如，我们在socket编程中常用的可能阻塞的函数connect accept send recv

对于非阻塞的socket，我们也可以调用上面的函数。不过其行为会略有不同。

## 非阻塞socket调用阻塞函数的行为

connect：
（对于阻塞的socket，connect会阻塞在三次握手的过程中）
不管是否连接成功立即返回，都可能返回失败，并设置errno==EINPROGRESS
(当然，如果返回0可以跳过检查。不过大部分情况都是返回失败)
调用完成之后，检查socket是可写的（例如可以用poll检查）就说明连接过程结束
虽然结束，但是我们也不能认为连接成功了，我们还需要使用getsockopt来检查

```cpp
int error;
socklen_t len = sizeof(error);
if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
    perror("getsockopt");
    // 处理 getsockopt 调用失败的情况
    close(sockfd);
    // ...
    return -1; // 或者其他错误码
}

if (error) {
    // error 不为 0，表示连接失败
    fprintf(stderr, "Connection error: %s\n", strerror(error));
    close(sockfd);
    // ...
    return -1; // 或者其他错误码
} else {
    // error 为 0，表示连接成功
    printf("Connection established!\n");
    // ...
    return 0; // 连接成功
}
```

accept:
如果backlog队列中没有socket，立即返回失败，并设置errno==EAGAIN

recv:
如果没有数据可读（接收缓冲区为空），立即返回失败，设置errno==EAGAIN

send:
如果socket不可写，立即返回失败，并设置errno==EAGAIN

## 如何将socket设置为非阻塞

使用fcntl函数

fcntl函数是 Unix-like 系统中用于文件控制的通用函数。 它可以执行多种不同的操作，包括获取和设置文件状态标志、复制文件描述符、设置文件锁等。 fcntl() 的名称来源于 "file control"。
```cpp
int set_nonblocking(int sockfd) {
  int flags = fcntl(sockfd, F_GETFL, 0); // 获取 socket 现有的 flags
  if (flags == -1) {
    perror("fcntl F_GETFL");
    return -1; // 错误处理
  }

  flags |= O_NONBLOCK; // 添加 O_NONBLOCK 标志

  if (fcntl(sockfd, F_SETFL, flags) == -1) {
    perror("fcntl F_SETFL O_NONBLOCK");
    return -1; // 错误处理
  }

  return 0; // 成功
}
```

使用socket()函数的SOCK_NONBLOCK标志
(此方法linux限定)
```cpp
int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // 创建时直接设置非阻塞
if (sockfd == -1) {
  perror("socket with SOCK_NONBLOCK");
  // 错误处理
}
```

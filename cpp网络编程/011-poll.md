# 011 poll

poll与select的使用大部分是类似的。

## 函数原型
```cpp
    int poll (struct pollfd *__fds, nfds_t __nfds, int __timeout);
```

参数：
- fds 指向一个pollfd结构体数组的指针。
- nfds fds中结构体的数量，用来表示文件描述符的数量。
- timeout 超时时间，单位是毫秒。设置为0为非阻塞轮询，设置为-1代表无限等待。

返回值：
- \>0 其revents字段被设置为非零值的文件描述符的数量
- 0 超时，并且其中没有发生指定的事件。
- -1 失败，并设置errno。常见错误有：
    - EBADF: 一个或多个在 __fds 中指定的文件描述符无效。
    - EFAULT: __fds 指向的内存区域不可访问。
    - EINTR: 在 poll 等待期间接收到信号。
    - EINVAL: __nfds 的值小于 0，或者 __timeout 的值无效。
    - ENOMEM: 没有足够的内存来完成操作。

## pollfd结构体
```cpp
struct pollfd
  {
    int fd;			/* File descriptor to poll.  */
    short int events;		/* Types of events poller cares about.  */
    short int revents;		/* Types of events that actually occurred.  */
  };
```
成员说明：
- fd 需要监视的文件描述符。如果是负数，会自动忽略event成员并将revents设置为0
- events 感兴趣的事件。位掩码，可以使用按位或 (|) 操作符组合多个事件，常用的事件有
    - POLLIN 有数据可读
    - POLLOUT 可以写数据
    - POLLPRI 有紧急数据可读
    - POLLHUB 挂起
    - POLLERR 发生错误
    - POLLNVAL 无效的fd
- revents 当poll返回时，内核会设置该成员。用来告诉用户哪些感兴趣的事件被触发。在返回之后，我们可以去检查该位掩码，并做出相应的处理。

## 相比select的优势
- poll没有fd数量的限制。我们可以通过调整pollfd数组的大小来监视更多的fd。
- 更清晰的事件处理。select使用了三个独立的位图来分别监听三种事件。而epoll将fd、感兴趣的事件和监听结果统一放到一个结构体中进行管理。
- 性能优势：对于仅监听少量高位fd的情况。select，将三个固定大小的fd_set拷贝到内核中，且会从0遍历到传入的nfds。但是poll可以仅拷贝和遍历pollfd数组。
- 支持边沿触发（默认是水平触发，但是可以设置event标志位来设置为边沿触发）

## poll的缺点
- 仍然需要遍历传入的pollfd数组。在高并发情况下效率低，可能有性能瓶颈。
- 边沿触发虽然支持，但是不够灵活和高效。

## example
一个用poll监听标准输入的例子。
```cpp
#include <iostream>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <string>

int main() {
    // 创建一个 pollfd 结构体数组，用于监听标准输入（文件描述符 0）
    std::vector<pollfd> fds(1);
    fds[0].fd = 0;           // 0 代表标准输入
    fds[0].events = POLLIN;  // 我们对可读事件感兴趣

    int timeout = 5000; // 设置超时时间为 5000 毫秒 (5 秒)

    std::cout << "等待标准输入..." << std::endl;

    int ret = poll(fds.data(), fds.size(), timeout);

    if (ret > 0) {
        // poll 返回值大于 0，表示有文件描述符发生了事件
        if (fds[0].revents & POLLIN) {
            // 标准输入有数据可读
            std::cout << "标准输入有数据可读。" << std::endl;
            std::string input;
            std::getline(std::cin, input);
            std::cout << "你输入了: " << input << std::endl;
        } else {
            // 其他事件发生，例如错误或挂起
            std::cout << "标准输入上发生了其他事件 (revents: " << fds[0].revents << ")" << std::endl;
        }
    } else if (ret == 0) {
        // poll 返回值为 0，表示超时
        std::cout << "超时，没有任何数据输入。" << std::endl;
    } else {
        // poll 返回值为 -1，表示发生错误
        perror("poll");
        return 1;
    }

    return 0;
}
```
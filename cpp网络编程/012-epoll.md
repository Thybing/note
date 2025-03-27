# 012-epoll

epoll的作用与select/poll类似。但是它采用了事件触发的方式，而不是轮询，有着相比于select和poll高非常多的效率。

## epoll_creat函数
函数原型：
```cpp
    int epoll_create (int __size);
```
说明：创建一个epoll实例，返回它的文件描述符。
参数: size 已经被废弃。传入一个大于0的值即可。

## epoll_event结构体
```cpp
struct epoll_event
{
  uint32_t events;	/* Epoll events */
  epoll_data_t data;	/* User data variable */
};

```
成员
- events 位掩码，类似于pollfd的events，设置感兴趣的事件。
    - `EPOLLIN`：可读事件(常用)。
    - `EPOLLOUT`：可写事件(常用)。
    - `EPOLLRDHUP` (since Linux 2.6.17)：连接的另一端关闭（例如，socket 连接关闭） (此事件是针对socket连接的)。
    - `EPOLLPRI`：紧急数据可读。
    - `EPOLLERR`：错误事件。
    - `EPOLLHUP`：挂起事件（例如，连接断开）。
    - `EPOLLET`：边缘触发 (Edge-Triggered)(默认是水平触发的)。
    - `EPOLLONESHOT`：只触发一次事件。 (例如用于避免多个线程同时处理同一个事件)
- data 一个联合体，用于存储用户数据。 通常，我们使用 data.fd 来存储文件描述符，以便在事件发生时能够快速找到对应的文件描述符。
```cpp
typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;
```

## epoll_ctl函数
```cpp
int epoll_ctl (int __epfd, int __op, int __fd,
		      struct epoll_event *__event);
```
说明：用于控制与 epoll 实例关联的文件描述符。 通过这个函数，你可以添加、修改或删除 epoll 实例中监视的文件描述符。 

参数： 
- epfd 要操作的epoll实例的fd
- op 指定要执行的操作
    - `EPOLL_CTL_ADD`：添加一个新的文件描述符 `fd` 到 `epoll` 实例 `epollfd` 中进行监视。
    - `EPOLL_CTL_MOD`：修改 `epoll` 实例 `epollfd` 中已经存在的、与文件描述符 `fd` 关联的事件。
    - `EPOLL_CTL_DEL`：从 `epoll` 实例 `epollfd` 中删除文件描述符 `fd`。
- fd 要操作的文件描述符
- event epoll_event结构体指针。如果是ADD和MOD操作，必须指向一个有效的结构体。

返回值：
- 0 成功
- -1 失败，并设置errno

## epoll_wait函数
epoll_wait函数用于等待与 epoll 实例关联的文件描述符上的事件发生。 

函数原型：
```cpp
int epoll_wait (int __epfd, struct epoll_event *__events,
		       int __maxevents, int __timeout);
```

参数：
- epfd epoll实例的文件描述符
- events \[out\] 用来接收就绪的事件。epoll_wait会将就绪的文件描述符的事件信息填充到这个数组中，(有效的全部在数组的前面，无需全部遍历)。
- maxevents events数组的最大大小。如果超过了这个数量的fd就绪。只会返回前maxevents个事件。(如果水平触发，下一次还会返回。但是如果是边沿触发的事件，本次忽略之后，这个事件就会丢失！！！)
- timeout 超时事件 单位为ms。设为0立即返回，-1无限等待。

返回值：
- \>0 返回就绪的事件数量。
- 0 超时
- -1 失败，并设置errno。常见的相关errno有
    - EBADF：epollfd 不是一个有效的文件描述符。
    - EFAULT：events 指针指向的内存地址无效。
    - EINTR：在等待事件期间，被信号中断。

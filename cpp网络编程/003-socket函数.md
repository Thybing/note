# 003 socket

```cpp
int socket (int __domain, int __type, int __protocol);
```

## domain 通信协议族

通信协议先按协议族划分
- PF_INET IPv4互联网协议族
- PF_INET6 IPv6互联网协议族
- PF_LOCAL 本地通信协议族
- PF_PACKET 内核底层的协议族
- PF_IPX IPX Novell协议族

一般最常用的是IPv4协议族
IPv6普及率不是很高

注：AF_INET是地址组(Address family)，PF是协议族(Protocol family)。现在主要作为历史遗留的产物。
在现代操作系统中，它与 AF_INET 是等价的。
所以在调用socket时填写AF_INET/AF_INET6即可

## type 数据传输的类型
SOCK_STREAM 是面向连接的socket
SOCK_DGRAM 是无连接的

## protocal 最终使用的协议
IPPROTO_TCP 在PF_INET协议族中，数据传输类型为SOCK_STREAM的协议只有它
IPPROTO_UDP 在PF_INET协议族中，数据传输类型为SOCK_DGRAM的协议只有它
也可以写0，根据前面两个自动选择。

## 返回值
* 成功返回一个sockfd
* 失败返回一个-1并设置errno
一般不会失败，失败请检查:参数填错/打开的fd达到限制

## TCP UDP 相关
TCP是可靠的，有超时重传的数据校验。保证数据无差错不丢失不失序不重复到达。

TCP 首部:
TCP有至少20字节的首部(最大60字节)。其中有源端口2B，目的端口2B，序号4B,
确认号4B，数据偏移4bit（TCP首部长度），6bit保留，6bit Flag，窗口大小2B,
校验和2B，紧急指针2B。
以上是20字节固定的数据，后续可有最大40字节的选项。并且要填充到4字节对齐。

UDP 是无连接的不可靠信道

UDP 首部:
8字节：2B源端口，2B目的端口，2B长度，2B校验和。
另，UDP计算校验和时前面会加上一个12字节长的伪首部

TCP 只能一对一，UDP可以一对一，一对多，多对多通信

### UDP可靠传输？

例如已经应用于HTTP3的QUIC协议，在应用层实现可靠传输。

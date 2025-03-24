# 005

## struct sockaddr

这是glibc中对于struct sockaddr的实现，2字节的协议族，14字节的地址和端口信息。
```c
typedef unsigned short int sa_family_t;

#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

struct sockaddr
  {
    __SOCKADDR_COMMON (sa_);	/* Common data: address family and length.  */
    char sa_data[14];		/* Address data.  */
  };
```

由于该结构体使用起来并不友好。所以我们通常先去使用以下对sockaddr兼容结构体，然后通过指针转换将其内存用sockaddr重新解释。

## struct sockaddr_in
这是glibc中的实现
```c
// 用到的定义的宏和类型别名
typedef unsigned short int sa_family_t;

typedef uint16_t in_port_t;

typedef uint32_t in_addr_t;
struct in_addr
  {
    in_addr_t s_addr;
  };

#define __SOCKADDR_COMMON_SIZE	(sizeof (unsigned short int))

#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

// 结构体定义

/* Structure describing an Internet socket address.  */
struct sockaddr_in
  {
    __SOCKADDR_COMMON (sin_);
    in_port_t sin_port;			/* Port number.  */
    struct in_addr sin_addr;		/* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof (struct sockaddr)
			   - __SOCKADDR_COMMON_SIZE
			   - sizeof (in_port_t)
			   - sizeof (struct in_addr)];
  };
```
2字节的地址族

2字节的端口
4字节的地址
8字节的填充

```cpp
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET; // 指定地址族（这里设置为IPv4地址族）
    servaddr.sin_port = htons(8080); 
    if (inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr) <= 0) {
            perror("Invalid address/ Address not supported");
            close(sockfd);
            exit(EXIT_FAILURE);
    } 
```
设置协议族为AF_INET
设置端口，用htons函数保证网络字节序
设置地址，用inet_pton函数(协议，地址字符串，转换后要拷贝到的buffer)，将ip地址转换为大端序的地址并赋值。返回值：1为成功，0说明ip格式有误，-1错误。
当然，如果提前知道二进制形式的ip地址，可以直接:servaddr.sin_addr = htonl(ip);

## struct sockaddr_in6
虽然IPv6现在普及度还不是非常高。但是还是顺便在这里贴一下glibc
```c
/* Ditto, for IPv6.  */
struct sockaddr_in6
  {
    __SOCKADDR_COMMON (sin6_);
    in_port_t sin6_port;	/* Transport layer port # */
    uint32_t sin6_flowinfo;	/* IPv6 flow information */
    struct in6_addr sin6_addr;	/* IPv6 address */
    uint32_t sin6_scope_id;	/* IPv6 scope-id */
  };
```
可以看到，依旧是2个字节的协议族+14个字节的端口、地址信息

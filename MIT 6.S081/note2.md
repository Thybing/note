## 隔离性

隔离性是操作系统组织结构的驱动力

我们希望在操作系统下各个程序的内存是相互隔离的

我们希望在操作系统上可以管理程序实现Multiplexing

实现方法为抽象硬件资源 abstracts the HW resources
例如，进程抽象了cpu资源，操作系统向用户提供进程，而不直接提供cpu资源，用户只能与进程交互。这样os才可以对cpu进行时分复用。
exec抽象了内存资源，调用exec时传入的文件相当于一个内存镜像，包含了指令，数据等，也可以通过其它系统调用来扩展内存如sbrk。但是进程不能去直接与物理内存交互。
files抽象了磁盘资源，程序不能去直接读写硬盘，但是可以通过files读写文件。操作系统会决定如何将文件与磁盘中的块对应起来，并且设置不同用户的访问权限。


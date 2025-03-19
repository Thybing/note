# Page Tables

隔离性是我们需要讨论虚拟内存的主要原因

## 地址空间

我们给所有的进程专属的地址空间
如何做到？

使用page tables

需要硬件支持，例如MMU（memory management unit） 

cpu执行指令时，例如从地址进行加载，其中的地址是虚拟地址，它会被转到mmu内，mmu将它翻译成物理地址，然后从物理地址拿回数据或写入数据。

MMU会查看一个表单，这个表单一边存储虚拟地址，一边存储物理地址。

通常这个表单也存储在内存中，所以cpu需要有寄存器来存放表单在物理内存的地址。在riscv上，这个由SATP寄存器负责。

每一个程序都会有自己的表单，负责将虚拟地址翻译成物理地址。（mmu负责查看并翻译）

cpu在进行进程调度时，也要切换SATP寄存器（kernel负责且用户无权限）。

## page tables

显然的，如果地址映射的表单中为每一个地址都进行一个映射，那么这个表单将会巨大无比。

首先：不要为每一个地址都创建条目，而是为每一个page创建。
在riscv中，每一个page是4KB（大部分处理器都是）

对于虚拟内存，其中有（下面的对于riscV）
EXT[63:39]，25位暂时没用到，目前能表示512G内存（补充对于riscV的物理内存，最大可以支持56bit的物理地址）
index[38,12]，27位，用来查找PPN（Physical Page Number）（riscv物理内存中有44位，所以一个PPN也是44位的，我们可以看到，虚拟内存能表示的地址数量是小于物理内存的）
offset[11,0]，12位，用来表示page中的偏移，与page大小4KB对应。（并且虚拟和物理的offset相同 ）

然而，依然有非常多的条目需要存储

所以，我们采用一种分级的条目索引方法。

对于27位的虚拟地址，将其分为L2，L1，L0三段，一段9位

首先我们有一个page，是最高级的Page Directory
其中可以存储4KB/8Byte就是512个PTE（Page Table Entry，包含44位PPN，10位标志位，10位保留）
我们用L2拿到一个PPN，这个PPN指向的是中级的Page Directory
然后用L1在中级PageDirectory中拿到指向低级PageDirectory的PPN
然后用L0在低级PageDirectory中拿到真实地址的PPN

最高级的PageDirectory哪里来呢？ SATP寄存器会指向最高级的PageDirectory

这样，我们就无需创建很多没有用到的条目了。
例如，原先我们需要对每一个page创建一个PPN条目。并且如果为空就valid为0。
一个valid只能表示一个page为空。
但是现在，高级page Directory中一个条目为空，表示了512 * 512个page为空。无需创建它们的中级和低级的page Directory，就可以得知它们是空的。
这便是它可以在进程占用很小时，可以非常高效率的存储PPN条目的关键。

关于PTE的10位flag
valid：为1表示合法，告诉MMU可以用来翻译。
例如某个page Directory中没有存满PTE，没有用到的PTE的valid置0

readable
writable

executable表明你可以从这个page执行指令

user表明用户模式可以被访问

后五个比较不常见这里不介绍。


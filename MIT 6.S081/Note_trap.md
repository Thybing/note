# trap章节代码笔记

## trampolines.S
**控制寄存器sscratch**: The kernel places a value here that comes in handy at the very start of a trap
handler.

### uservec
在进入uservec时，sscratch指向被map到user space的trapframe。这时虽然运行在kernel，但是页表还是trap之前运行的user proc对应的user space的页表。

swap a0和sscratch  
a0指向了trapframe，再将其它的寄存器保存，最后保存原始的a0  
```riscv
	# swap a0 and sscratch
    # so that a0 is TRAPFRAME
    csrrw a0, sscratch, a0

    # save the user registers in TRAPFRAME
    sd ra, 40(a0)
    sd sp, 48(a0)
    sd gp, 56(a0)
    sd tp, 64(a0)
    sd t0, 72(a0)
    .........
    sd t5, 272(a0)
    sd t6, 280(a0)

	# save the user a0 in p->trapframe->a0
    csrr t0, sscratch
    sd t0, 112(a0)
```

拿到进程对应的内核栈kernel stack,hartid,usertrap的地址。然后切换kernel pagetable。最后跳转到usertrap(trap.c中定义)
```riscv
    # restore kernel stack pointer from p->trapframe->kernel_sp
    ld sp, 8(a0)

    # make tp hold the current hartid, from p->trapframe->kernel_hartid
    ld tp, 32(a0)

    # load the address of usertrap(), p->trapframe->kernel_trap
    ld t0, 16(a0)

    # restore kernel page table from p->trapframe->kernel_satp
    ld t1, 0(a0)
    csrw satp, t1
    sfence.vma zero, zero

    # a0 is no longer valid, since the kernel page
    # table does not specially map p->tf.

    # jump to usertrap(), which does not return
    jr t0

```

### userret
在进入时，寄存器已经被设置好如下状态。(下面trap.c的地方有说明。)
```
    # userret(TRAPFRAME, pagetable)
    # switch from kernel to user.
    # usertrapret() calls here.
    # a0: TRAPFRAME, in user page table.
    # a1: user page table, for satp.
```

利用ld reg, offset(a0)，将trapframe中保存的值恢复。  
同时，我们也要保证sscrach也要被恢复。所以我们和uservec类似，使用sscratch提前保存好a0要恢复的值，然后利用csrrw进行一次交换操作。

```riscv
    # switch to the user page table.
    csrw satp, a1
    sfence.vma zero, zero

    # put the saved user a0 in sscratch, so we
    # can swap it with our a0 (TRAPFRAME) in the last step.
    ld t0, 112(a0)
    csrw sscratch, t0

    # restore all but a0 from TRAPFRAME
    ld ra, 40(a0)
    ...
    ld s1, 104(a0)
    # NOTE: dont restore trapframe->a0(offset:112)
    ld a1, 120(a0)
    ...
    ld t6, 280(a0)

    # restore user a0, and save TRAPFRAME in sscratch
    csrrw a0, sscratch, a0
    
    # return to user mode and user pc.
    # usertrapret() set up sstatus and sepc.
    sret

```
执行 sret 时，RISC-V CPU 会：

步骤	寄存器	行为
①	sstatus.SPP	判断 trap 前是 U-mode 还是 S-mode  
②	sstatus.SPIE → sstatus.SIE	恢复中断使能状态（是否允许中断）  
③	sepc	作为跳转地址（程序跳回用户代码）  
④	privilege level	切换 CPU 特权级（从 S-mode → U-mode）  

## trap.c

定义了ticks和用于保护它的自旋锁。声明了trampoline.S中的外部标签。声明了内核中断向量。声明设备中断处理函数。
定义初始化函数，其中初始化了全局锁。在启动系统时调用。
trapinithart：初始化当前hart的stvec。在每个cpu启动时调用。
```c
    struct spinlock tickslock;
    uint ticks;

    extern char trampoline[], uservec[], userret[];

    // in kernelvec.S, calls kerneltrap().
    void kernelvec();

    extern int devintr();

    void
    trapinit(void)
    {
        initlock(&tickslock, "time");
    }

    // set up to take exceptions and traps while in the kernel.
    void
    trapinithart(void)
    {
        w_stvec((uint64)kernelvec);
    }
```

**stvec寄存器**
• stvec: The kernel writes the address of its trap handler here; the RISC-V jumps to the
address in stvec to handle a trap.
发生trap时pc会跳到其中存储的代码地址。

内联汇编。设置stvec寄存器。risc.c中定义
```c
    // Supervisor Trap-Vector Base Address
    // low two bits are mode.
    static inline void 
    w_stvec(uint64 x)
    {
    asm volatile("csrw stvec, %0" : : "r" (x));
    }
```

### 核心的usertrap函数，由trampoline.S的uservec进入

    sstatus寄存器
    • sstatus: The SIE bit in sstatus controls whether device interrupts are enabled. If the
    kernel clears SIE, the RISC-V will defer device interrupts until the kernel sets SIE. The SPP
    bit indicates whether a trap came from user mode or supervisor mode, and controls to what
    mode sret returns.  
    sstatus寄存器包含了一些重要的状态信息，这里我们用到的是其中的SPP位。
    SPP位表示上次 trap 前 CPU 的运行模式。
    SPP = 0 表示来自用户态。
    SPP = 1 表示来自内核态

判断了trap之前的模式，如果不是用户态，说明系统出现了错乱。

    sstatus.SIE:这一位后面也有用到
    SIE – Supervisor Interrupt Enable 位
    作用：是否允许中断进入 supervisor 模式（内核态）。
    值为 1：允许中断打断当前执行。
    值为 0：即使中断发生，也会被挂起（推迟处理）。

w_stvec设置新的中断向量，因为当前已经从转变到内核态，所以将中断向量设置为kernelvec

    sepc寄存器
    • sepc: When a trap occurs, RISC-V saves the program counter here (since the pc is then
    overwritten with the value in stvec). The sret (return from trap) instruction copies sepc
    to the pc. The kernel can write sepc to control where sret goes.
拿到sepc寄存器中保存的trap之前的PC，保存起来。

    scause寄存器
    • scause: RISC-V puts a number here that describes the reason for the trap
    记录了trap发生的原因。

判断scause寄存器是否==8，即判断是否是系统调用。

如果是系统调用，先判断其是否killed。然后将存储的trap之前的pc+4(因为要跳过ecall)

开启中断（执行到这里之前不允许中断，因为可能会改变寄存器的值），内部实际上改变了sstatus的SIE位。

执行真正的syscall

如果cause!=8，调用devintr()，判断是否是设备中中断并记录。

否则是未知异常。set当前进程killed标志。

检查是否被kill掉。

从刚刚记录的设备中断(devintr的返回值)检查是否是定时器设备造成的中断，如果是，要让出cpu。

执行usertrapret。从trap中返回。

```c
//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");


  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  
  // save user program counter.
  p->trapframe->epc = r_sepc();
  
  if(r_scause() == 8){
    // system call

    if(p->killed)
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    intr_on();

    syscall();
  } else if((which_dev = devintr()) != 0){
    // ok
  } else {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }

  if(p->killed)
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  usertrapret();
}
```

### usertrapret函数，usertrap函数返回时调用。

关闭内核中断直到完全返回至user mode。

设置stvec，因为是返回到user mode，所以恢复为uservec。
注意这里不能直接用uservec。因为trampoline实际上在内核空间中被map了两次。
链接时一般使用的为direct map的地址。这个地址在内核页表中没问题，但是放到user space就会出问题。
所以我们要去显式的映射到高位的地址 TRAMPOLINE + (uservec - trampoline)

设置好trapframe中的几个值，以便下次trap时使用。

复位sstatus中的SPP位，
置位SPIE位，允许下次来自用户mode的中断。

将保存的trap前的PC加载到sepc寄存器。

jump到userret。利用ABI将trapframe和user pagetable放到a0和a1中。
注意，jump的函数地址也不能直接用userret，因为这里的链接到的也是direct map的那个地址，不是高位的地址。
否则在切换页表之后，pc指向的地址就失去了映射，会出现段错误。

```c
//
// return to user space
//
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to trampoline.S
  w_stvec(TRAMPOLINE + (uservec - trampoline));

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 fn = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64,uint64))fn)(TRAPFRAME, satp);
}
```


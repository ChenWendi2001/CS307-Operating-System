## Chapter 12 Homework
陈文迪 519021910071

*作业中的引用内容均已标出*

#### 12.5 DMA是如何增加系统的并发性的？它又是如何使硬件设计变得复杂的？

**问题解答：**

> CPU将这个命令块的地址写到DMA控制器，然后继续执行其他工作。DMA控制器继续直接操作内存总线，将地址放到总线，在没有主CPU的帮助的情况下执行传输。

使用DMA让CPU不必再观察状态位并按字节发送数据到控制寄存器，可以在传输大量数据的同时执行其他任务，这就增加了系统整体的并发性能。

硬件设计变得更复杂是因为我们增加了DMA控制器这一原件，并且将其分别连接至CPU内存总线和PCI总线。硬件的增加让我们需要设计一个合理的机制来让DMA控制器与设备控制器进行握手，这又增加了设备的复杂性。此外，我们需要避免DMA和CPU同时访问内存，因此还需要设计周期窃取策略，这同样需要硬件支持。

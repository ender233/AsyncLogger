# AsyncLogger

之前使用`muduo`的日志库, 功能很多了, 但是需要链接muduo的静态库. 

故抽取出简单的日志功能自用, 功能比较简陋, 只支持单个进程输出一个文件, 重启会覆盖, 以后有需要再扩展.

当前异步日志库前端是抽取的[Muduo日志库](https://github.com/chenshuo/muduo/tree/master/muduo/base), 核心是`FixedBuffer`的写入和转移.

后端并没有采用`Muduo Logger`一样多个缓存, 而是使用两个[阻塞队列](https://github.com/qicosmos/cosmos)+一个缓存的方式. 不需要关注阻塞队列的日志写入(阻塞队列会处理多个日志写入线程的枷锁).

而定时写入日志的时候,仅仅是交换了两个阻塞队列, 加锁区间非常小, 写的时候独占阻塞队列,不需要枷锁.

### Simple_SkipListDB

#### Redis's zset (skiplist)
```c++
#define ZSKIPLIST_MAXLEVEL 32
#define ZSKIPLIST_P 0.25

typedef struct zskiplistNode {
  robj *obj;    //存放该节点的数据
  double score; //数据对应的分数值，zset通过分数对数据升序排列
  struct zskiplistNode *backward; //指向链表上一个节点的指针，即后向指针
  struct zskiplistLevel {
    struct zskiplistNode *forward; //指向链表下一个节点的指针，即前向指针
    unsigned int span; //表示这个前向指针跳跃过了多少个节点（不包括当前节点）————用于实现zrange等操作
  } level[]; //结构体zskiplistLevel的数组，表示跳表中的一层
} zskiplistNode;

typedef struct zskiplist {
  struct zskiplistNode *header, *tail; //头指针和尾指针
  unsigned long length;                //跳表的长度，不包括头指针
  int level;                           //跳表的层数
} zskiplist;
```

- 一个zset结构同时包含**一个字典**和**一个跳跃表**。跳跃表按score从小到大保存所有集合元素，而字典则保存着从member到score的映射。
```c++
typedef struct zset {
    dict *dict;
    zskiplist *zsl;
} zset;
```

- Redis基于Reactor模式开发了网络事件处理器，图示如下：
![img](https://imgconvert.csdnimg.cn/aHR0cHM6Ly9tbWJpei5xcGljLmNuL21tYml6X3BuZy9PcVRBbDNXVEM3RmsyQmxOc3MzcmliRlA4NklWak5yTmc3VkVOODZWNVpsRHppYjlOODlib1BQQTZPSjd4NzFPcGljU1hlTXZPbHEyMGFzUXZWaWFOQTU1UHcvNjQwP3d4X2ZtdD1wbmc)

参考：[Skiplist及Redis中的实现](https://www.jianshu.com/p/09c3b0835ba6)


#### Interface：

- insertElement
- deleteElement
- searchElement
- displayList
- dumpFile
- loadFile
- size

#### Test:

|operator    | thread num   |  count   | consume(s) |
|---------|---------|-------|--------|
|set | 2 | 100000 | 8.80163|
|get | 2 | 100000 | 11.2695|

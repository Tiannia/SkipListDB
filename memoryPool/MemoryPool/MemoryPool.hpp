#ifndef _MEMORYPOOL_HPP_
#define _MEMORYPOOL_HPP_

#include <iostream>
#include <mutex>

////一个内存块的最大内存大小,可以扩展
#define MAX_MEMORY_SIZE 256

class MemoryPool;

//内存块
struct MemoryBlock {
  MemoryBlock *pNext; //下一块内存块
  bool bUsed;         //是否使用
  bool bBelong;       //是否属于内存池
  MemoryPool *pMem;   //属于哪个池子
};

class MemoryPool {
public:
  MemoryPool(size_t nSize = 128, size_t nBlock = 10) {
    //相当于申请10块内存，每块内存是1024
    _nSize = nSize;
    _nBlock = nBlock;
    _pHeader = NULL;
    _pBuffer = NULL;
  }
  virtual ~MemoryPool() {
    if (_pBuffer != NULL) {
      free(_pBuffer);
    }
  }
  //申请内存
  void *AllocMemory(size_t nSize) {
    std::lock_guard<std::mutex> lock(_mutex);
    //如果首地址为空，说明没有申请空间
    if (_pBuffer == NULL) {
      InitMemory();
    }
    MemoryBlock *pRes = NULL;
    //如果内存池不够用时，需要重新申请内存
    if (_pHeader == NULL) {
      pRes = (MemoryBlock *)malloc(nSize + sizeof(MemoryBlock));
      pRes->bBelong = false;
      pRes->bUsed = false;
      pRes->pNext = NULL;
      pRes->pMem = NULL;
    } else {
      pRes = _pHeader;
      _pHeader = _pHeader->pNext;
      pRes->bUsed = true;
    }
    //返回只返回头后面的信息
    return ((char *)pRes + sizeof(MemoryBlock));
  }

  //释放内存
  void FreeMemory(void *p) {
    std::lock_guard<std::mutex> lock(_mutex);
    //和申请内存刚好相反，这里需要包含头，然后全部释放
    MemoryBlock *pBlock = (MemoryBlock *)((char *)p - sizeof(MemoryBlock));
    std::cout << (void *)pBlock << std::endl;
    if (pBlock->bBelong) {
      std::cout << "pbolck:" << (void *)pBlock << ", " << "pHeader:" << (void *)_pHeader << std::endl;  
      pBlock->bUsed = false;
      pBlock->pNext = _pHeader; //循环链起来
      pBlock = _pHeader;
    } else {
      //不属于内存池直接释放就可以
      free(pBlock);
    }
  }
  //初始化内存块
  void InitMemory() {
    if (_pBuffer)
      return;
    //计算每块的大小
    size_t PoolSize = _nSize + sizeof(MemoryBlock);
    //计算需要申请多少内存
    size_t BuffSize = PoolSize * _nBlock;
    _pBuffer = (char *)malloc(BuffSize);
    //初始化头
    _pHeader = (MemoryBlock *)_pBuffer;
    _pHeader->bUsed = false;
    _pHeader->bBelong = true;
    _pHeader->pMem = this;
    //初始化_nBlock块，并且用链表的形式连接
    //保存头指针
    MemoryBlock *tmp1 = _pHeader;
    for (size_t i = 1; i < _nBlock; i++) {
      MemoryBlock *tmp2 = (MemoryBlock *)(_pBuffer + i * PoolSize);
      tmp2->bUsed = false;
      tmp2->pNext = NULL;
      tmp2->bBelong = true;
      _pHeader->pMem = this;
      tmp1->pNext = tmp2;
      tmp1 = tmp2;
    }
  }

public:
  //内存首地址（第一块内存的地址）
  char *_pBuffer;
  //内存块头（内存池节点）
  MemoryBlock *_pHeader;
  //内存块大小（每个节点多大）
  size_t _nSize;
  //多少块（节点数）
  size_t _nBlock;
  //互斥锁
  std::mutex _mutex;
};

//可以使用模板传递参数
template <size_t nSize, size_t nBlock> class MemoryPoolor : public MemoryPool {
public:
  MemoryPoolor() {
    _nSize = nSize;   // 128 or 256 (块大小)
    _nBlock = nBlock; // 1000  (块数目)
  }
};

//需要重新对内存池就行管理
class ManagerPool {
public:
  static ManagerPool &Instance() //单例模式
  {
    static ManagerPool memPool;
    return memPool;
  }

  void *AllocMemory(size_t nSize) {
    if (nSize < MAX_MEMORY_SIZE) {
      return _Alloc[nSize]->AllocMemory(nSize);
    } else {
      MemoryBlock *pRes = (MemoryBlock *)malloc(nSize + sizeof(MemoryBlock));
      pRes->bBelong = false;
      pRes->bUsed = true;
      pRes->pMem = NULL;
      pRes->pNext = NULL;
      return ((char *)pRes + sizeof(MemoryBlock));
    }
  }

  //释放内存
  void FreeMemory(void *p) {
    MemoryBlock *pBlock = (MemoryBlock *)((char *)p - sizeof(MemoryBlock));
    //释放内存池
    if (pBlock->bBelong) {
      pBlock->pMem->FreeMemory(p);
    } else {
      free(pBlock);
    }
  }

private:
  ManagerPool() {
    InitArray(0, 128, &_memory128);
    InitArray(129, 256, &_memory256);
  }

  ~ManagerPool() {}

  void InitArray(int nBegin, int nEnd, MemoryPool *pMemPool) {
    for (int i = nBegin; i <= nEnd; i++) {
      _Alloc[i] = pMemPool;
    }
  }
  //可以根据不同内存块进行分配
  MemoryPoolor<128, 1000> _memory128;
  MemoryPoolor<256, 1000> _memory256;
  //映射数组
  MemoryPool *_Alloc[MAX_MEMORY_SIZE + 1];
};
#endif

#ifndef _OPERATEMEM_HPP_
#define _OPERATEMEM_HPP_

#include <iostream>
#include <stdlib.h>
#include "MemoryPool.hpp"


void* operator new(size_t nSize)
{
 return ManagerPool::Instance().AllocMemory(nSize);
}

void operator delete(void* p)
{
 return ManagerPool::Instance().FreeMemory(p);
}

void* operator new[](size_t nSize)
{
 return ManagerPool::Instance().AllocMemory(nSize);
}

void operator delete[](void* p)
{
 return ManagerPool::Instance().FreeMemory(p);
}

#endif
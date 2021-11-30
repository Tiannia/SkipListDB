### 内存池

存在bug，申请(new)多个内存块时，在一一释放(delete)的时候会报seg fault😅.
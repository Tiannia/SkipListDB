#include "mmpool.h"
#include "stdio.h"

mmp_pool_ptr mmp;

int main(){
    mmpl_create(&mmp);
    char *p = (char *)mmpl_getmem(mmp, 1200);
    scanf("%s", p);
    printf("This is the text you input:%s\n", p);

    //int mmpl_rlsmem(struct mm_pool_s *p_mmpl, void *rls_mmaddr);
    mmpl_rlsmem(mmp, (void *)p);

    mmpl_destroy(mmp);
    printf("Successfully quit!\n");

}
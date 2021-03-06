#include <iostream>
#include "skiplist.h"
#include "Comparator.h"
#define FILE_PATH "../store/dumpFile"
#define TEST


int main() {

#ifdef TEST
    SkipList<std::string, std::string, StringComparator> skipList(6);
	skipList.insert_element("12345", "kejiang"); //score, element (key, value)
	skipList.insert_element("7", "erke"); 
	skipList.insert_element("3", "tian"); 
	skipList.insert_element("100", "zerotrac"); 
	skipList.insert_element("58", "gimi"); 
	skipList.insert_element("77", "qiqi"); 
	skipList.insert_element("66", "liuliu"); 
	skipList.insert_element("77", "yang"); 
	skipList.insert_element("66", "yanuuuuuuuuuuuuuuuuuug"); 

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search_element("66");
    skipList.search_element("88");


    skipList.display_list();

    skipList.delete_element("58");
    skipList.delete_element("100");

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.display_list();

    std::cout << "<------------------->" << std::endl;
    std::cout << "now is going to load file in a new skiplistDB:" << std::endl;
    SkipList<std::string, std::string, StringComparator> skipList_new(6);
    skipList_new.load_file();

    skipList_new.display_list();
#endif

    return 0;
}

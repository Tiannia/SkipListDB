#include "../skiplist.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <time.h>
#define SET
#define GET
#define NUM_THREADS 4
#define TEST_COUNT 100000
SkipList<int, std::string> skipList(18);

int main() {
  srand(time(NULL));
  std::cout << "Cpu core num:" << std::thread::hardware_concurrency()
           << std::endl;

#ifdef SET
  {
    std::thread threads[NUM_THREADS];

    int i;

    auto start = std::chrono::high_resolution_clock::now();

    for (i = 0; i < NUM_THREADS; i++) {
      threads[i] = std::thread([]() {
        int tmp = TEST_COUNT / NUM_THREADS;
        for (int count = 0; count < tmp; count++) {
          skipList.insert_element(rand() % TEST_COUNT, "a");
        }
      });
      std::cout << "main() : creating thread:" << threads[i].get_id()
                << std::endl;
    }

    for (i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    //std::cout << "count for test:" << count_for_test << std::endl;
  }
#endif

#ifdef DISPLAY
  skipList.displayList();

#endif

#ifdef GET

  {
    std::thread threads[NUM_THREADS];

    int i;

    auto start = std::chrono::high_resolution_clock::now();

    for (i = 0; i < NUM_THREADS; i++) {
      threads[i] = std::thread([]() {
        int tmp = TEST_COUNT / NUM_THREADS;
        for (int count = 0; count < tmp; count++) {
          skipList.search_element(rand() % TEST_COUNT);
        }
      });
      std::cout << "main() : creating thread:" << threads[i].get_id()
                << std::endl;
    }

    for (i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "get elapsed:" << elapsed.count() << std::endl;
  }

#endif

  return 0;
}

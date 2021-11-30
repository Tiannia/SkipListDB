#include "OperateMem.hpp"

using namespace std;

int main() {
  int *p = new int[10];
  char *str = new char[120];
  delete[] p;
  delete[] str;
  cout << "Successfully, you quit." << endl;
  return 0;
}

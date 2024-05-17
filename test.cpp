#include <bits/stdc++.h>
using namespace std;
int main() {
  freopen("t4.txt", "w", stdout);
  set<int> text;
  cout << 20000 << std::endl;
  for(int i = 1; i <= 19999; i++) {
    int a = rand() % 2;
    int b = rand() % 10000;
    if(a) {
      while(text.find(b) != text.end()) {
        b = rand() % 10000;
      }
      text.insert(b);
      cout << "insert 1 " << b << std::endl;
    } else {
      text.erase(b);
      cout << "delete 1 " << b << std::endl;
    }
  }
  cout << "find 1" << std::endl;
  return 0;
}
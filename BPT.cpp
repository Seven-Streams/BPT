#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
const unsigned long long exp1 = 13331, exp2 = 131;
template <class W, int info_len = 3> class MemoryRiver {//应当采取3个参数。
//一个存储目前的元素个数，一个存储目前的根节点，一个存储当前的块应该写入到哪里。
private:
  std::fstream file;
  std::string file_name;   // 文件名。
  int sizeofT = sizeof(W); // W的大小。

public:
  MemoryRiver() {}

  MemoryRiver(const std::string &file_name) : file_name(file_name) {
    initialise();
    return;
  }
  void ChangeName(std::string res) {
    file_name = res;
    initialise();
  }

  void
  initialise() { // 如果文件不存在，则会进行初始化操作，建立文件并初始化参数。
    std::filesystem::path test(file_name);
    if (std::filesystem::exists(test)) {
      return;
    }
    file.open(file_name, std::fstream::out);
    int tmp = 0;
    for (int i = 0; i < info_len; ++i)
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  // 读出第n个int的值赋给tmp，1_base
  void get_info(int &tmp, int n) {
    if (n > info_len)
      return;
    file.open(file_name);
    file.seekg((n - 1) * sizeof(int));
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
    return;
  }

  // 将tmp写入第n个int的位置，1_base
  void write_info(int tmp, int n) {
    if (n > info_len)
      return;
    file.open(file_name);
    file.seekp((n - 1) * sizeof(int), std::fstream::beg);
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
    return;
  }

  // 在文件合适位置写入类对象t，并返回写入的位置索引index
  // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  // 位置索引index可以取为对象写入的起始位置,1base
  void write(W &t, int which_node, int size = 1) {
    int place = info_len * 4;
    place += (which_node - 1) * sizeofT;
    file.open(file_name);
    file.seekp(place);
    file.write(reinterpret_cast<char *>(&t), sizeofT * size);
    file.close();
    return;
  }
  void read(W &t, int which_node, int size = 1) {
    int place = info_len * 4;
    place += (which_node - 1) * sizeofT;
    file.open(file_name);
    file.seekg(place);
    file.read(reinterpret_cast<char *>(&t), sizeofT * size);
    file.close();
    return;
  }
};
unsigned long long MyHash(std::string txt, unsigned long long exp) {
  unsigned long long ans = 0;
  for (int i = 0; i < txt.size(); i++) {
    ans *= exp;
    ans += txt[i];
  }
  return ans;
}
template <class Value = int, int size = 70> class BPT {
private:
  std::string file = "database.txt";
  struct MyData {
    unsigned long long hash1 = 0;
    unsigned long long hash2 = 0;
    int value = 0;
    int son = 0;
  };
  struct Node {
    MyData datas[size];
    int parent = 0;
    int left_sibling = 0;
    int right_sibling = 0;
    int now_size = 0;
    int pos = 0;
  };
  MemoryRiver<Node, 3> mydatabase;
public:
  BPT() = default;
  BPT(std::string name) {
    mydatabase.ChangeName(name);
  }
  ~BPT() = default;
  MyData Insert(unsigned long long hash1, unsigned long long hash2, int value, int &flag) {
    int total = 0;
    mydatabase.get_info(total, 1);
    if(total == 0) {
      Node res1;
      res1.datas[0].hash1 = hash1;
      res1.datas[0].hash2 = hash2;
      res1.datas[0].value = value;
      res1.now_size = 1;
      res1.pos = 1;
      mydatabase.write(res1, 1);
      mydatabase.write_info(1, 1);
      mydatabase.write_info(1, 2);
      mydatabase.write_info(1, 3);
      return res1.datas[0];
    }
  }
};

int main() {
  BPT<int> test("database");
  return 0;
}
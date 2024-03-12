#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
const unsigned long long exp1 = 13331, exp2 = 131;
const int minus_max = -2147483648;
template <class W, int info_len = 3> class MemoryRiver { // 应当采取3个参数。
  // 一个存储目前的元素个数，一个存储目前的根节点，一个存储当前的块应该写入到哪里。
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
    bool operator>(const MyData &other) {
      if (hash1 != other.hash1) {
        return hash1 > other.hash1;
      }
      if (hash2 != other.hash2) {
        return hash2 > other.hash2;
      }
      return value > other.value;
    }
    bool operator<(const MyData &other) {
      if (hash1 != other.hash1) {
        return hash1 < other.hash1;
      }
      if (hash2 != other.hash2) {
        return hash2 < other.hash2;
      }
      return value < other.value;
    }
    bool operator==(const MyData &other) {
      if (hash1 != other.hash1) {
        return false;
      }
      if (hash2 != other.hash2) {
        return false;
      }
      return value == other.value;
    }
    bool operator!=(const MyData &other) { return !(*this == other); }
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
  void NodeInsert(MyData to_insert, int pos) {
    Node res;
    mydatabase.read(res, pos);
    int find = 0;
    for (find = 0; find < res.now_size; find++) {
      if (res.datas[find] > to_insert) {
        break;
      }
    }
    if (res.datas[find].son == 0) {
      if (find != res.now_size) {
        std::memmove(&res.datas[find + 1], &res.datas[find],
                     (res.now_size - find) * sizeof(MyData));
      }
      res.datas[find] = to_insert;
      res.now_size++;
      if (res.now_size == (find + 1)) {
        int parent = res.parent;
        if (parent != 0) {
          UpdateIndex(parent, res.datas[find - 1], res.datas[find]);
        }
      } // 说明需要向上更新。
    } else {
      NodeInsert(to_insert, res.datas[find].son); // 并非叶子节点。进一步插入。
      mydatabase.read(res, pos); // 可能该层节点被更新，需要重新读入。
    }
    mydatabase.write(res, pos);
    if (res.now_size >= (size - 3)) {
      // Split
    }
    return;
  }
  void UpdateIndex(int pos, MyData old_data, MyData new_data) {
    if (pos == 0) {
      return;
    }
    Node res;
    mydatabase.read(res, pos);
    for (int i = 0; i < res.now_size; i++) {
      if (res.datas[i] == old_data) {
        new_data.son = res.datas[i].son;
        res.datas[i] = new_data;
        mydatabase.write(res, pos);
        UpdateIndex(res.parent, old_data, new_data);
        return;
      }
    }
    return;
  }
  void Split(int pos) {
    Node res;
    Node new_node;
    mydatabase.read(res, pos);
    int now_size = res.now_size;
    int half = now_size / 2;
    std::memmove(&new_node.datas[0], &res.datas[0], half * sizeof(MyData));
    std::memmove(&res.datas[0], &res[half], (now_size - half) * sizeof(MyData));
    res.now_size = (now_size - half);
    new_node.now_size = half;
    new_node.left_sibling = res.left_sibling;
    new_node.right_sibling = res.pos;
    new_node.parent = res.parent;
    int to_insert_pos;
    mydatabase.get_info(to_insert_pos, 3);
    to_insert_pos++;
    mydatabase.write_info(to_insert_pos, 3);
    new_node.pos = to_insert_pos; // 至此，所有新节点已经准备完毕。
    res.left_sibling = new_node.pos;
    MyData index;
    index = new_node.datas[half - 1];
    index.son = to_insert_pos;
    if (new_node[0].son != 0) {
      for (int i = 0; i < half; i++) {
        Node to_update;
        mydatabase.read(to_update, new_node[i].son);
        to_update.parent = to_insert_pos;
        mydatabase.write(to_update, new_node[i].son);
      }
    } // 更新所有新节点子节点的父亲。
    if (res.parent) {
      OnlyInsert(res.parent, index, res.datas[now_size - half - 1]);
    } else {
      int current;
      mydatabase.get_info(current, 3);
      current++;
      Node new_alloc;
      new_alloc.now_size = 2;
      new_alloc.left_sibling = 0;
      new_alloc.right_sibling = 0;
      new_alloc.parent = 0;
      new_alloc.pos = current;
      new_alloc.datas[0] = res.datas[now_size - half - 1];
      new_alloc.datas[0].son = res.pos;
      new_alloc.datas[1] = index;
      res.parent = current;
      new_node.parent = current;
      mydatabase.write(new_alloc, current);
      mydatabase.write_info(current, 2);
      mydatabase.write_info(current, 3);
    }
    mydatabase.write(res, res.pos);
    mydatabase.write(new_node, new_node.pos);
    return;
  }
  void OnlyInsert(int pos, MyData to_insert, MyData old_index) {
    Node res;
    mydatabase.read(res, pos);
    int find = 0;
    for (find = 0; find < res.now_size; find++) {
      if (old_index == res.datas[find]) {
        break;
      }
    }
    std::memmove(res.datas[find + 1], res.datas[find],
                 (res.now_size - find + 1));
    res.datas[find] = to_insert;
    mydatabase.write(res, pos);
    return;
  }

public:
  BPT() = default;
  BPT(std::string name) { mydatabase.ChangeName(name); }
  ~BPT() = default;
  void Insert(unsigned long long hash1, unsigned long long hash2, int value) {
    int total = 0;
    mydatabase.get_info(total, 1);
    if (total == 0) {
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
    } else {
      int root;
      mydatabase.get_info(root, 2);
      MyData res;
      res.hash1 = hash1;
      res.hash2 = hash2;
      res.value = value;
      NodeInsert(res, root);
      total++;
      mydatabase.write_info(total, 1);
    }
    return;
  }
  bool find(unsigned long long hash_1, unsigned long long hash_2) {
    int root, total;
    mydatabase.get_info(total, 1);
    mydatabase.get_info(root, 2);
    if(total == 0) {
      return false;
    }
    Node res;
    MyData to_find;
    to_find.hash1 = hash_1;
    to_find.hash2 = hash_2;
    to_find.value = minus_max;
    mydatabase.read(res, root);
    while(res.datas[0].son != 0) {
      for(int i = 0; i < res.now_size; i++) {
        if(to_find < res.datas[i]) {
          mydatabase.read(res, res.datas[i].son);
          break;
        }
      }
    }
    int found = 0;
    for(int found = 0; found < res.now_size; found++) {
      if((hash_1 == res.datas[found].hash1) && (hash_2 == res.datas[found].hash2)) {
        break;
      }
    }
    if(found == res.now_size) {
      return 0;
    }
    while((hash_1 == res.datas[found].hash1) && (hash_2 == res.datas[found].hash2)) {
      std::cout << res.datas[found].value << std::endl;
      found++;
      if(found == res.now_size) {
        if(res.right_sibling == 0) {
          return 1;
        }
        mydatabase.read(res, res.right_sibling);
        found = 0;
      }
    }
    return 1;
  }
  void Print() {
    int current;
    mydatabase.get_info(current, 3);
    Node to_print;
    for(int i = 1; i <= 3; i++) {
      mydatabase.read(to_print, i);
      std::cout << to_print.pos << ' ' << to_print.left_sibling << 
      ' ' << to_print.right_sibling << ' ' << to_print.parent << std::endl;
    }
    return;
  }
};

int main() {
  BPT<int> test("database");
  for(int i = 0; i < 1; i++) {
    test.Insert(1, 2, i);
  }
  test.Print();
  return 0;
}
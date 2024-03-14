#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>
const unsigned long long exp1 = 13331, exp2 = 131;
const int minus_max = -2147483648;
const int maxn = 2147483647;
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
template <class Value = int, int size = 10> class BPT {
private:
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
    MyData datas[size + 3];
    int left_sibling = 0;
    int right_sibling = 0;
    int now_size = 0;
    int pos = 0;
  };
  MemoryRiver<Node, 3> mydatabase;
  void NodeInsert(MyData to_insert, int pos, int last_node) {
    Node res;
    mydatabase.read(res, pos);
    int find = 0;
    for (find = 0; find < res.now_size; find++) {
      if (res.datas[find] > to_insert) {
        break;
      }
    }
    if (res.datas[0].son == 0) {
      if (find != res.now_size) {
        std::memmove(&res.datas[find + 1], &res.datas[find],
                     (res.now_size - find) * sizeof(MyData));
      }
      res.datas[find] = to_insert;
      res.now_size++;
      mydatabase.write(res, pos);
    } else {
      if (find == (res.now_size)) {
        find--;
        auto x = to_insert;
        x.son = res.datas[find].son;
        res.datas[find] = x;
        mydatabase.write(res, pos);
      }
      NodeInsert(to_insert, res.datas[find].son, res.pos);
    }
    if (res.now_size >= size) {
      Split(res.pos, last_node);
    }
    return;
  }
  void Split(int pos, int last_node) {
    Node res;
    Node new_node;
    mydatabase.read(res, pos);
    int now_size = res.now_size;
    int half = now_size / 2;
    std::memmove(&new_node.datas[0], &res.datas[0], half * sizeof(MyData));
    std::memmove(&res.datas[0], &res.datas[half],
                 (now_size - half) * sizeof(MyData));
    res.now_size = (now_size - half);
    new_node.now_size = half;
    new_node.left_sibling = res.left_sibling;
    new_node.right_sibling = res.pos;
    int to_insert_pos;
    mydatabase.get_info(to_insert_pos, 3);
    to_insert_pos++;
    mydatabase.write_info(to_insert_pos, 3);
    new_node.pos = to_insert_pos; // 至此，所有新节点已经准备完毕。
    res.left_sibling = new_node.pos;
    Node res1;
    mydatabase.read(res1, new_node.left_sibling);
    res1.right_sibling = to_insert_pos;
    mydatabase.write(res1, res1.pos);
    MyData index;
    index = new_node.datas[half - 1];
    index.son = to_insert_pos;
    if (last_node) {
      OnlyInsert(last_node, index);
    } else {
      int current;
      mydatabase.get_info(current, 3);
      current++;
      Node new_alloc;
      new_alloc.now_size = 2;
      new_alloc.left_sibling = 0;
      new_alloc.right_sibling = 0;
      new_alloc.pos = current;
      new_alloc.datas[1] = res.datas[now_size - half - 1];
      new_alloc.datas[1].son = res.pos;
      new_alloc.datas[0] = index;
      mydatabase.write(new_alloc, current);
      mydatabase.write_info(current, 2);
      mydatabase.write_info(current, 3);
    }
    mydatabase.write(res, res.pos);
    mydatabase.write(new_node, new_node.pos);
    return;
  }
  void OnlyInsert(int pos, MyData to_insert) {
    Node res;
    mydatabase.read(res, pos);
    int find = 0;
    for (find = 0; find < res.now_size; find++) {
      if (res.datas[find] > to_insert) {
        break;
      }
    }
    std::memmove(&res.datas[find + 1], &res.datas[find],
                 (res.now_size - find) * sizeof(MyData));
    res.datas[find] = to_insert;
    res.now_size++;
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
      NodeInsert(res, root, 0);
      total++;
      mydatabase.write_info(total, 1);
    }
    return;
  }
  bool find(unsigned long long hash_1, unsigned long long hash_2) {
    int root, total;
    mydatabase.get_info(total, 1);
    mydatabase.get_info(root, 2);
    if (total == 0) {
      std::cout << "null" << '\n';
      return false;
    }
    Node res;
    MyData to_find;
    to_find.hash1 = hash_1;
    to_find.hash2 = hash_2;
    to_find.value = minus_max;
    mydatabase.read(res, root);
    while (res.datas[0].son != 0) {
      for (int i = 0; i < res.now_size; i++) {
        if (to_find < res.datas[i]) {
          mydatabase.read(res, res.datas[i].son);
          break;
        }
      }
    }
    int found = 0;
    for (found = 0; found < res.now_size; found++) {
      if ((hash_1 == res.datas[found].hash1) &&
          (hash_2 == res.datas[found].hash2)) {
        break;
      }
    }
    if (found == res.now_size) {
      std::cout << "null" << '\n';
      return 0;
    }
    while ((hash_1 == res.datas[found].hash1) &&
           (hash_2 == res.datas[found].hash2)) {
      std::cout << res.datas[found].value << ' ';
      found++;
      if (found == res.now_size) {
        if (res.right_sibling == 0) {
          std::cout << '\n';
          return 1;
        }
        mydatabase.read(res, res.right_sibling);
        found = 0;
      }
    }
    std::cout << '\n';
    return 1;
  }
  void Print() {
    std::cout << "OK" << std::endl;
    int current, total, root;
    mydatabase.get_info(total, 1);
    mydatabase.get_info(root, 2);
    mydatabase.get_info(current, 3);
    std::cout << total << ' ' << root << ' ' << current << '\n';
    Node to_print;
    for (int i = 1; i <= current; i++) {
      mydatabase.read(to_print, i);
      std::cout << to_print.pos << ' ' << to_print.now_size << ' '
                << to_print.left_sibling << ' ' << to_print.right_sibling << ' '
                << '\n';
    }
    return;
  }
  void PrintRoot() {
    int root;
    mydatabase.get_info(root, 2);
    Node res;
    mydatabase.read(res, root);
    for (int i = 0; i < res.now_size; i++) {
      std::cout << res.datas[i].value << ' ' << res.datas[i].son << '\n';
    }
    return;
  }
  void Tranverse() {
    int root;
    mydatabase.get_info(root, 2);
    Node res;
    mydatabase.read(res, root);
    while (res.datas[0].son != 0) {
      mydatabase.read(res, res.datas[0].son);
    }
    int big = -1;
    while (res.right_sibling) {
      std::cout << res.datas[res.now_size - 1].value << ' ' << big << '\n';
      big = res.datas[res.now_size - 1].value;
      mydatabase.read(res, res.right_sibling);
    }
    std::cout << res.datas[res.now_size - 1].value << ' ' << big << '\n';
    return;
  }
  void Output() {
    for (int i = 1; i <= 30; i++) {
      Node res;
      mydatabase.read(res, i);
      std::cout << i << ' ' << res.datas[0].value << '\n';
    }
    return;
  }
  void Check(int pos) {
    Node x;
    mydatabase.read(x, pos);
    std::cout << x.pos << ' ' << x.left_sibling << ' ' << x.right_sibling << ' '
              << x.now_size << '\n';
    for (int i = 0; i < x.now_size; i++) {
      std::cout << x.datas[i].value << ' ' << x.datas[i].son << '\n';
    }
  }
  void Erase(unsigned long long hash_1, unsigned long long hash_2, int value) {
    int root;
    mydatabase.get_info(root, 2); // to get the node.
    int status = 0;
    MyData to_delete;
    to_delete.hash1 = hash_1;
    to_delete.hash2 = hash_2;
    to_delete.value = value;
    NodeErase(root, 0, to_delete, 0, 0);
    if (status != -1) {
      int total;
      mydatabase.get_info(total, 1);
      total--;
      mydatabase.write_info(total, 1); // change the size.
    }
    return;
  }
  bool NodeErase(int pos, int last_pos, MyData to_delete, int where,
                 int how_many) {
    // pos表示当前节点号、父亲节点号、待删除内容，这个节点是父亲节点的多少号元素、父亲节点有多少个元素。
    Node res;
    MyData nothing;
    mydatabase.read(res, pos);
    int found = 0;
    auto last_one = res.datas[res.now_size - 1];
    for (int i = 0; i < res.now_size; i++) {
      if ((res.datas[i] > to_delete) || (res.datas[i] == to_delete)) {
        if (res.datas[i].son == 0) {
          if (res.datas[i] != to_delete) {
            return false; // didn't find~
          } else {
            if (res.now_size == 1) { // 说明删空了。
              if (res.left_sibling != 0) {
                Node left;
                mydatabase.read(left, res.left_sibling);
                left.right_sibling = res.right_sibling;
                mydatabase.write(left, res.left_sibling);
              }
              if (res.right_sibling != 0) {
                Node right;
                mydatabase.read(right, res.right_sibling);
                right.left_sibling = res.left_sibling;
                mydatabase.write(right, res.right_sibling);
              }
              auto x = res.datas[0];
              if (last_pos) {
                Node parent;
                mydatabase.read(parent, last_pos);
                for (int i = 0; i < parent.now_size; i++) {
                  if (parent.datas[i] == x) {
                    if (i != (parent.now_size - 1)) {
                      std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                   (parent.now_size - i - 1) * sizeof(MyData));
                    }
                    parent.now_size--;
                    mydatabase.write(parent, last_pos);
                  }
                }
              }
              res.now_size--;
              mydatabase.write(res, pos);
              return true;
            }
            if (i != (res.now_size - 1)) {
              std::memmove(&res.datas[i], &res.datas[i + 1],
                           (res.now_size - i - 1));
            }
            res.now_size--;
            if (last_pos == 0) {
              mydatabase.write(res, pos);
              return true;
            }
            if (res.now_size < (size / 2)) {
              if ((where == 0) &&
                  (how_many == 1)) { // 说明这个节点没有办法进行调整。
                if (i == res.now_size) {
                  Node parent;
                  mydatabase.read(parent, last_pos);
                  auto to_change = res.datas[res.now_size];
                  auto to_update = res.datas[res.now_size - 1];
                  for (int i = 0; i < parent.now_size; i++) {
                    if (to_change == parent.datas[i]) {
                      to_update.son = parent.datas[i].son;
                      parent.datas[i] = to_update;
                      mydatabase.write(parent, last_pos);
                      mydatabase.write(res, pos);
                      return true;
                    }
                  }
                }
                return true;
              }
              if (where != (how_many - 1)) {
                int right;
                right = res.right_sibling;
                Node right_s;
                mydatabase.read(right_s, res.right_sibling); // 读入右儿子。
                if (right_s.now_size >= (size / 2)) {
                  auto to_update = res.datas[res.now_size];
                  res.datas[res.now_size] = right_s.datas[0];
                  res.now_size++;
                  mydatabase.write(res, pos);
                  std::memmove(&right_s.datas[0], &right_s.datas[1],
                               (right_s.now_size - 1) * sizeof(MyData));
                  right_s.now_size--;
                  mydatabase.write(right_s, right_s.pos);
                  Node parent;
                  mydatabase.read(parent, last_pos);
                  for (int i = 0; i < parent.now_size; i++) {
                    if (parent.datas[i] == to_update) {
                      auto to_change = res.datas[res.now_size - 1];
                      to_change.son = parent.datas[i].son;
                      parent.datas[i] = to_change;
                      mydatabase.write(parent, last_pos);
                      return true; // 借块操作完成。右兄弟节点、本身、父节点均得到更新。
                    }
                  }
                } else { // 说明旁边的节点数目数目已经足够少。
                  auto to_change = res.datas[res.now_size];
                  std::memmove(&res.datas[res.now_size], &right_s.datas[0],
                               right_s.now_size * sizeof(MyData));
                  res.now_size += right_s.now_size;
                  Node parent;
                  mydatabase.read(parent, last_pos);
                  for (int i = 0; i < parent.now_size; i++) {
                    if (parent.datas[i] == to_change) {
                      if (i != (parent.now_size - 1)) {
                        std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                     (parent.now_size - i - 1) *
                                         sizeof(MyData));
                      }
                      parent.now_size--;
                      break;
                    }
                  }
                  auto to_update = res.datas[res.now_size - 1];
                  for (int i = 0; i < parent.now_size; i++) {
                    if (parent.datas[i] == to_update) {
                      parent.datas[i].son = pos;
                      break;
                    }
                  }
                  mydatabase.write(parent, last_pos);
                  res.right_sibling = right_s.right_sibling;
                  if (right_s.right_sibling != 0) {
                    Node double_right;
                    mydatabase.read(double_right, right_s.right_sibling);
                    double_right.left_sibling = pos;
                    mydatabase.write(double_right, right_s.right_sibling);
                  }
                  mydatabase.write(res, pos);
                  return true;
                }
              } else {
                int left;
                left = res.left_sibling;
                Node left_s;
                mydatabase.read(left_s, res.right_sibling); // 读入右儿子。
                if (left_s.now_size >= (size / 2)) {
                  std::memmove(&res.datas[1], &res.datas[0],
                               res.now_size * sizeof(MyData));
                  res.datas[0] = left_s.datas[res.now_size - 1];
                  auto to_change = left_s.datas[left_s.now_size - 1];
                  auto to_update = left_s.datas[left_s.now_size - 2];
                  left_s.now_size--;
                  res.now_size++;
                  mydatabase.write(res, pos);
                  mydatabase.write(left_s, left);
                  Node parent;
                  mydatabase.read(parent, last_pos);
                  for (int i = 0; i < parent.now_size; i++) {
                    if (parent.datas[i] == to_change) {
                      to_update.son = parent.datas[i].son;
                      parent.datas[i] = to_update;
                      mydatabase.write(parent, last_pos);
                      return true;
                    }
                  }
                } else { // 说明旁边的节点数目数目已经足够少。
                  std::memmove(&res.datas[left_s.now_size], &res.datas[0],
                               res.now_size * sizeof(MyData));
                  std::memmove(&res.datas[0], &left_s.datas[0],
                               left_s.now_size);
                  res.now_size += left_s.now_size;
                  res.left_sibling = left_s.left_sibling;
                  mydatabase.write(res, pos);
                  if (left_s.left_sibling != 0) {
                    Node double_left;
                    mydatabase.read(double_left, left_s.left_sibling);
                    left_s.right_sibling = pos;
                    mydatabase.write(double_left, left_s.left_sibling);
                  }
                  auto to_change = res.datas[left_s.left_sibling - 1];
                  Node parent;
                  mydatabase.read(parent, last_pos);
                  for (int i = 0; i < parent.now_size; i++) {
                    if (parent.datas[i] == to_change) {
                      if (i != (parent.now_size - 1)) {
                        std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                     (parent.now_size - i - 1) *
                                         sizeof(MyData));
                      }
                      parent.now_size--;
                      mydatabase.write(parent, last_pos);
                      break;
                    }
                  }
                  return true;
                }
              }
            }
          }
        } else {
          bool ans =
              NodeErase(res.datas[i].son, pos, to_delete, i, res.now_size);
          if (ans == false) {
            return false;
          }
          if (last_pos == 0) {
            return true;
          }
          mydatabase.read(res, pos);
          if (res.now_size == 0) { // 说明删空了。
            if (res.left_sibling != 0) {
              Node left;
              mydatabase.read(left, res.left_sibling);
              left.right_sibling = res.right_sibling;
              mydatabase.write(left, res.left_sibling);
            }
            if (res.right_sibling != 0) {
              Node right;
              mydatabase.read(right, res.right_sibling);
              right.left_sibling = res.left_sibling;
              mydatabase.write(right, res.right_sibling);
            }
            auto x = res.datas[0];
            if (last_pos) {
              Node parent;
              mydatabase.read(parent, last_pos);
              for (int i = 0; i < parent.now_size; i++) {
                if (parent.datas[i] == x) {
                  if (i != (parent.now_size - 1)) {
                    std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                 (parent.now_size - i - 1) * sizeof(MyData));
                  }
                  parent.now_size--;
                  mydatabase.write(parent, last_pos);
                  break;
                }
              }
            }
            mydatabase.write(res, pos);
            return true;
          }
          if (res.datas[res.now_size - 1] != last_one) { // 应当向上修改。
            Node parent;
            mydatabase.read(parent, last_pos);
            for (int i = 0; i < parent.now_size; i++) {
              if (parent.datas[i] == last_one) {
                auto to_update = res.datas[res.now_size - 1];
                to_update.son = parent.datas[i].son;
                parent.datas[i] = to_update;
                mydatabase.write(parent, last_pos);
                break;
              }
              if (res.now_size < (size / 2)) {
                if ((where == 0) &&
                    (how_many == 1)) { // 说明这个节点没有办法进行调整。
                  if (i == res.now_size) {
                    Node parent;
                    mydatabase.read(parent, last_pos);
                    auto to_change = res.datas[res.now_size];
                    auto to_update = res.datas[res.now_size - 1];
                    for (int i = 0; i < parent.now_size; i++) {
                      if (to_change == parent.datas[i]) {
                        to_update.son = parent.datas[i].son;
                        parent.datas[i] = to_update;
                        mydatabase.write(parent, last_pos);
                        mydatabase.write(res, pos);
                        return true;
                      }
                    }
                  }
                  return true;
                }
                if (where != (how_many - 1)) {
                  int right;
                  right = res.right_sibling;
                  Node right_s;
                  mydatabase.read(right_s, res.right_sibling); // 读入右儿子。
                  if (right_s.now_size >= (size / 2)) {
                    auto to_update = res.datas[res.now_size];
                    res.datas[res.now_size] = right_s.datas[0];
                    res.now_size++;
                    mydatabase.write(res, pos);
                    std::memmove(&right_s.datas[0], &right_s.datas[1],
                                 (right_s.now_size - 1) * sizeof(MyData));
                    right_s.now_size--;
                    mydatabase.write(right_s, right_s.pos);
                    Node parent;
                    mydatabase.read(parent, last_pos);
                    for (int i = 0; i < parent.now_size; i++) {
                      if (parent.datas[i] == to_update) {
                        auto to_change = res.datas[res.now_size - 1];
                        to_change.son = parent.datas[i].son;
                        parent.datas[i] = to_change;
                        mydatabase.write(parent, last_pos);
                        return true; // 借块操作完成。右兄弟节点、本身、父节点均得到更新。
                      }
                    }
                  } else { // 说明旁边的节点数目数目已经足够少。
                    auto to_change = res.datas[res.now_size];
                    std::memmove(&res.datas[res.now_size], &right_s.datas[0],
                                 right_s.now_size * sizeof(MyData));
                    res.now_size += right_s.now_size;
                    Node parent;
                    mydatabase.read(parent, last_pos);
                    for (int i = 0; i < parent.now_size; i++) {
                      if (parent.datas[i] == to_change) {
                        if (i != (parent.now_size - 1)) {
                          std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                       (parent.now_size - i - 1) *
                                           sizeof(MyData));
                        }
                        parent.now_size--;
                        break;
                      }
                    }
                    auto to_update = res.datas[res.now_size - 1];
                    for (int i = 0; i < parent.now_size; i++) {
                      if (parent.datas[i] == to_update) {
                        parent.datas[i].son = pos;
                        break;
                      }
                    }
                    mydatabase.write(parent, last_pos);
                    res.right_sibling = right_s.right_sibling;
                    if (right_s.right_sibling != 0) {
                      Node double_right;
                      mydatabase.read(double_right, right_s.right_sibling);
                      double_right.left_sibling = pos;
                      mydatabase.write(double_right, right_s.right_sibling);
                    }
                    mydatabase.write(res, pos);
                    return true;
                  }
                } else {
                  int left;
                  left = res.left_sibling;
                  Node left_s;
                  mydatabase.read(left_s, res.right_sibling); // 读入右儿子。
                  if (left_s.now_size >= (size / 2)) {
                    std::memmove(&res.datas[1], &res.datas[0],
                                 res.now_size * sizeof(MyData));
                    res.datas[0] = left_s.datas[res.now_size - 1];
                    auto to_change = left_s.datas[left_s.now_size - 1];
                    auto to_update = left_s.datas[left_s.now_size - 2];
                    left_s.now_size--;
                    res.now_size++;
                    mydatabase.write(res, pos);
                    mydatabase.write(left_s, left);
                    Node parent;
                    mydatabase.read(parent, last_pos);
                    for (int i = 0; i < parent.now_size; i++) {
                      if (parent.datas[i] == to_change) {
                        to_update.son = parent.datas[i].son;
                        parent.datas[i] = to_update;
                        mydatabase.write(parent, last_pos);
                        return true;
                      }
                    }
                  } else { // 说明旁边的节点数目数目已经足够少。
                    std::memmove(&res.datas[left_s.now_size], &res.datas[0],
                                 res.now_size * sizeof(MyData));
                    std::memmove(&res.datas[0], &left_s.datas[0],
                                 left_s.now_size);
                    res.now_size += left_s.now_size;
                    res.left_sibling = left_s.left_sibling;
                    mydatabase.write(res, pos);
                    if (left_s.left_sibling != 0) {
                      Node double_left;
                      mydatabase.read(double_left, left_s.left_sibling);
                      left_s.right_sibling = pos;
                      mydatabase.write(double_left, left_s.left_sibling);
                    }
                    auto to_change = res.datas[left_s.left_sibling - 1];
                    Node parent;
                    mydatabase.read(parent, last_pos);
                    for (int i = 0; i < parent.now_size; i++) {
                      if (parent.datas[i] == to_change) {
                        if (i != (parent.now_size - 1)) {
                          std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                       (parent.now_size - i - 1) *
                                           sizeof(MyData));
                        }
                        parent.now_size--;
                        mydatabase.write(parent, last_pos);
                        break;
                      }
                    }
                    return true;
                  }
                }
              }
              return true;
            }
          }
        }
      }
    }
    return false;
  };
};
int main() {
  std::ios::sync_with_stdio(false);
  BPT<int> test("database");
  for (int i = 0; i < 10; i++) {
    test.Insert(1, 2, i);
  }
  // test.Erase(1, 2, 6);
  for (int i = 1; i <= 4; i++) {
    test.Check(i);
    std::cout << std::endl;
  }
  // int n;
  // std::cin >> n;
  // std::string op;
  // for (int i = 0; i < n; i++) {
  //   std::cin >> op;
  //   if (op == "insert") {
  //     std::string index;
  //     int value;
  //     std::cin >> index;
  //     std::cin >> value;
  //     unsigned long long hash1, hash2;
  //     hash1 = MyHash(index, exp1);
  //     hash2 = MyHash(index, exp2);
  //     test.Insert(hash1, hash2, value);
  //     continue;
  //   }
  //   if (op == "find") {
  //     std::string index;
  //     std::cin >> index;
  //     unsigned long long hash1, hash2;
  //     hash1 = MyHash(index, exp1);
  //     hash2 = MyHash(index, exp2);
  //     test.find(hash1, hash2);
  //     continue;
  //   }
  //   if (op == "delete") {
  //     std::string index;
  //     int value;
  //     std::cin >> index;
  //     std::cin >> value;
  //     unsigned long long hash1, hash2;
  //     hash1 = MyHash(index, exp1);
  //     hash2 = MyHash(index, exp2);
  //     test.Erase(hash1, hash2, value);
  //     continue;
  //   }
  // }
  // // for (int i = 1; i <= 10; i++) {
  // //   test.Check(i);
  // //   std::cout << std::endl;
  // // }
  // return 0;
}
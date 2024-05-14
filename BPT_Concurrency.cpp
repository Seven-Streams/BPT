#include "utility.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
const unsigned long long exp1 = 13331, exp2 = 131;
const int minus_max = -2147483648;
const int maxn = 2147483647;

namespace sjtu {

/**
 * @tparam T Type of the elements.
 * Be careful that T may not be default constructable.
 *
 * @brief A list that supports operations like std::list.
 *
 * We encourage you to design the implementation yourself.
 * As for the memory management, you may use std::allocator,
 * new/delete, malloc/free or something else.
 */
template <class T> struct node {
  T *value = nullptr;
  node<T> *pre = nullptr;
  node<T> *nxt = nullptr;
  node(node<T> *_pr, T x, node<T> *nx = nullptr) {
    pre = _pr;
    value = new T(x);
    nxt = nx;
  }
  node() {
    value = nullptr;
    pre = nullptr;
    nxt = nullptr;
  }
  ~node() { delete value; }
};
template <typename T> class list {
private:
  size_t total = 0;
  node<T> head;
  node<T> tail;

public:
  class iterator;
  class const_iterator;

public:
  /**
   * Constructs & Assignments
   * At least three: default constructor, copy constructor/assignment
   * Bonus: move/initializer_list constructor/assignment
   */
  list() {
    head.nxt = &tail;
    tail.pre = &head;
    total = 0;
  }
  list(const list &other) {
    node<T> *last = &head;
    total = other.total;
    for (auto it = other.head.nxt; it->nxt != nullptr; it = it->nxt) {
      last->nxt = new node<T>(last, *(it->value));
      last = last->nxt;
    }
    last->nxt = &tail;
    tail.pre = last;
  }
  list &operator=(const list &other) {
    if (&head == &other.head) {
      return *this;
    }
    node<T> *last = nullptr;
    total = other.total;
    for (auto it = head.nxt; it != nullptr; it = it->nxt) {
      delete last;
      last = it;
    }
    last = &head;
    for (auto it = other.head.nxt; it->nxt != nullptr; it = it->nxt) {
      last->nxt = new node<T>(last, *(it->value));
      last = last->nxt;
    }
    last->nxt = &tail;
    tail.pre = last;
    return *this;
  }

  /* Destructor. */
  ~list() {
    node<T> *last = nullptr;
    for (auto it = head.nxt; it != nullptr; it = it->nxt) {
      delete last;
      last = it;
    }
  }

  /* Access the first / last element. */
  T &front() noexcept { return *(head.nxt->value); }
  T &back() noexcept { return *(tail.pre->value); }
  const T &front() const noexcept { return *(head.nxt->value); }
  const T &back() const noexcept { return *(tail.pre->value); }

  /* Return an iterator pointing to the first element. */
  iterator begin() noexcept {
    iterator res(head.nxt);
    return res;
  }
  const_iterator cbegin() const noexcept {
    const_iterator res(head.nxt);
    return res;
  }

  /* Return an iterator pointing to one past the last element. */
  iterator end() noexcept {
    iterator res(&tail);
    return res;
  }
  const_iterator cend() const noexcept {
    const_iterator res(const_cast<node<T> *>(&tail));
    return res;
  }

  /* Checks whether the container is empty. */
  bool empty() const noexcept { return !size(); }
  /* Return count of elements in the container. */
  size_t size() const noexcept { return total; }

  /* Clear the contents. */
  void clear() noexcept {
    node<T> *now = nullptr;
    for (auto it = head.nxt; it != nullptr; it = it->nxt) {
      delete now;
      now = it;
    }
    head.nxt = &tail;
    total = 0;
    return;
  }
  /**
   * @brief Insert value before pos (pos may be the end() iterator).
   * @return An iterator pointing to the inserted value.
   * @throw Throw if the iterator is invalid.
   */
  iterator insert(iterator pos, const T &value) {
    node<T> *res = new node<T>(pos.now->pre, value);
    res->nxt = pos.now;
    res->pre->nxt = res;
    pos.now->pre = res;
    total++;
    iterator x(res);
    return res;
  }

  /**
   * @brief Remove the element at pos (remove end() iterator is invalid).
   * returns an iterator pointing to the following element, if pos pointing to
   * the last element, end() will be returned.
   * @throw Throw if the container is empty, or the iterator is invalid.
   */
  iterator erase(iterator pos) noexcept {
    total--;
    iterator ans(pos.now->nxt);
    pos.now->nxt->pre = pos.now->pre;
    pos.now->pre->nxt = ans.now;
    delete pos.now;
    return ans;
  }

  /* Add an element to the front/back. */
  void push_front(const T &value) {
    node<T> *res = new node<T>(&head, value);
    res->nxt = head.nxt;
    head.nxt->pre = res;
    head.nxt = res;
    total++;
    return;
  }
  void push_back(const T &value) {
    node<T> *res = new node<T>(tail.pre, value);
    res->nxt = &tail;
    tail.pre = res;
    res->pre->nxt = res;
    total++;
    return;
  }

  /* Removes the first/last element. */
  void pop_front() noexcept {
    head.nxt = head.nxt->nxt;
    delete head.nxt->pre;
    head.nxt->pre = &head;
    total--;
    return;
  }
  void pop_back() noexcept {
    tail.pre = tail.pre->pre;
    delete tail.pre->nxt;
    tail.pre->nxt = &tail;
    total--;
    return;
  }

public:
  /**
   * Basic requirements:
   * operator ++, --, *, ->
   * operator ==, != between iterators and const iterators
   * constructing a const iterator from an iterator
   *
   * If your implementation meets these requirements,
   * you may not comply with the following template.
   * You may even move this template outside the class body,
   * as long as your code works well.
   *
   * Contact TA whenever you are not sure.
   */
  class iterator {
  public:
    iterator() = default;
    iterator(const iterator &x) = default;
    ~iterator() {}
    iterator(node<T> *x) { now = x; }
    node<T> *now = nullptr;
    iterator &operator++() {
      now = now->nxt;
      return *this;
    }
    iterator &operator--() {
      now = now->pre;
      return *this;
    }
    iterator operator++(int x) {
      auto res = *this;
      now = now->nxt;
      return res;
    }
    iterator operator--(int x) {
      auto res = *this;
      now = now->pre;
      return res;
    }
    T &operator*() const noexcept { return *(now->value); }
    T *operator->() const noexcept { return (now->value); }

    /* A operator to check whether two iterators are same (pointing to the same
     * memory) */
    friend bool operator==(const iterator &a, const iterator &b) {
      return a.now == b.now;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.now != b.now;
    }
  };

  /**
   * Const iterator should have same functions as iterator, just for a const
   * object. It should be able to construct from an iterator. It should be able
   * to compare with an iterator.
   */
  class const_iterator {
  public:
    const_iterator() = default;
    const_iterator(const const_iterator &x) = default;
    ~const_iterator() {}
    node<T> *now = nullptr;
    const_iterator(const iterator &x) { now = x.now; }
    friend bool operator==(const const_iterator &a, const const_iterator &b) {
      return a.now == b.now;
    }
    friend bool operator!=(const const_iterator &a, const const_iterator &b) {
      return a.now != b.now;
    }
    const_iterator &operator++() {
      now = now->nxt;
      return *this;
    }
    const_iterator &operator--() {
      now = now->pre;
      return *this;
    }
    const_iterator operator++(int x) const {
      auto res = *this;
      now = now->nxt;
      return res;
    }
    const_iterator operator--(int x) const {
      auto res = *this;
      now = now->pre;
      return res;
    }
    T &operator*() const noexcept { return *(now->value); }
    T *operator->() const noexcept { return *(now->value); }
  };
};
} // namespace sjtu
namespace sjtu {
template <typename T> class vector {
private:
  T *array;
  int total = 0;
  int array_size;
  void DoubleArray() {
    if (array_size == 0) {
      array = new T[2];
      array_size = 2;
    } else {
      T *tmp = new T[(total + 1) * 2];
      memmove(tmp, array, sizeof(T *) * total);
      delete[] array;
      array = tmp;
      array_size = 2 * (total + 1);
    }
    return;
  }
  void ShrinkArray() {
    T *tmp = new T[(total + 1) * 2];
    memmove(tmp, array, sizeof(T *) * total);
    delete[] array;
    array = tmp;
    array_size = (total + 1) * 2;
    return;
  }

public:
  class const_iterator;
  class iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

  private:
    int index;
    vector *v;

  public:
    iterator() {
      index = -1;
      v = nullptr;
    }
    iterator &operator=(const iterator &other) {
      index = other.index;
      v = other.v;
    }
    iterator(const const_iterator &other) {
      index = other.index;
      v = other.v;
    }
    iterator(const iterator &other) {
      index = other.index;
      v = other.v;
    }
    int GetIndex() { return index; }
    iterator(int i, vector<T> *id) {
      index = i;
      v = id;
    }
    iterator operator+(const int &n) const { return iterator(index + n, v); }
    iterator operator-(const int &n) const { return iterator(index - n, v); }
    int operator-(const iterator &rhs) const {
      if (v->array != rhs.v->array) {
        throw(1);
      }
      return (index - rhs.index) > 0 ? (index - rhs.index)
                                     : -(index - rhs.index);
    }
    iterator &operator+=(const int &n) {
      index += n;
      return *this;
    }
    iterator &operator-=(const int &n) {
      index -= n;
      return *this;
    }
    iterator operator++(int x) {
      auto tmp = *this;
      index++;
      return tmp;
    }
    iterator &operator++() {
      index++;
      return *this;
    }
    iterator operator--(int) {
      auto res = *this;
      index--;
      return res;
    }
    iterator &operator--() {
      index--;
      return *this;
    }
    T &operator*() const { return (*v)[index]; }
    bool operator==(const iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return false;
      }
      return (index == rhs.index);
    }
    bool operator==(const const_iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return false;
      }
      return (index == rhs.index);
    }
    bool operator!=(const iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return true;
      }
      return (index != rhs.index);
    }
    bool operator!=(const const_iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return true;
      }
      return (index != rhs.index);
    }
  };
  class const_iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;
    const_iterator() {
      index = -1;
      v = nullptr;
    }
    const_iterator(const const_iterator &other) {
      index = other.index;
      v = other.v;
    }
    const_iterator(int i, const vector<T> *id) {
      index = i;
      v = const_cast<vector<T> *>(id);
    }
    const_iterator(const iterator &other) {
      index = other.index;
      v = other.v;
    }
    const_iterator &operator=(const const_iterator &other) {
      index = other.index;
      v = other.v;
    }
    const_iterator operator+(const int &n) const {
      return const_iterator(index + n, *v);
    }
    const_iterator operator-(const int &n) const {
      return const_iterator(index - n, *v);
    }
    int operator-(const const_iterator &rhs) const {
      if (v->array != rhs.v->array) {
        throw(1);
      }
      return (index - rhs.index) > 0 ? (index - rhs.index)
                                     : -(index - rhs.index);
    }
    const_iterator &operator+=(const int &n) {
      index += n;
      return *this;
    }
    const_iterator &operator-=(const int &n) {
      index -= n;
      return *this;
    }
    const_iterator operator++(int x) {
      auto tmp = *this;
      index++;
      return tmp;
    }
    const_iterator &operator++() {
      index++;
      return *this;
    }
    const_iterator operator--(int) {
      auto tmp = *this;
      index--;
      return tmp;
    }
    const_iterator &operator--() {
      index--;
      return *this;
    }
    T &operator*() const { return (*v)[index]; }
    bool operator==(const const_iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return false;
      }
      return (index == rhs.index);
    }
    bool operator==(const iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return false;
      }
      return (index == rhs.index);
    }
    bool operator!=(const iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return true;
      }
      return (index != rhs.index);
    }
    bool operator!=(const const_iterator &rhs) const {
      if (rhs.v->array != v->array) {
        return true;
      }
      return (index != rhs.index);
    }

  private:
    int index;
    vector *v = nullptr;
  };
  vector() {
    total = 0;
    array_size = 0;
    array = nullptr;
  }
  vector(const vector &other) {
    total = other.total;
    array_size = other.array_size;
    array = new T *[array_size];
    for (int i = 0; i < other.total; i++) {
      array[i] = new T(*(other.array[i]));
    }
  }
  ~vector() { clear(); }
  vector &operator=(const vector &other) {
    if (array == other.array) {
      return *this;
    }
    this->clear();
    total = other.total;
    array_size = other.array_size;
    array = new T *[array_size];
    for (int i = 0; i < other.total; i++) {
      array[i] = new T(*(other.array[i]));
    }
    return *this;
  }
  T &at(const size_t &pos) {
    if (pos > (total - 1)) {
      throw(2);
    }
    return (array[pos]);
  }
  const T &at(const size_t &pos) const {
    if (pos > (total - 1)) {
      throw(2);
    }
    return (*array[pos]);
  }
  T &operator[](const size_t &pos) {
    if (pos > (total - 1)) {
      throw(2);
    }
    return (array[pos]);
  }
  const T &operator[](const size_t &pos) const {
    if (pos > (total - 1)) {
      throw(2);
    }
    return *(array[pos]);
  }
  const T &front() const {
    if (total == 0) {
      throw(3);
    }
    return (array[0]);
  }
  const T &back() const { return (array[total - 1]); }
  iterator begin() { return iterator(0, this); }
  const_iterator cbegin() const { return const_iterator(0, this); }
  iterator end() { return iterator(total, this); }
  const_iterator cend() const { return const_iterator(total, this); }
  bool empty() const { return total == 0; }
  size_t size() const { return total; }
  void clear() {
    delete[] array;
    total = 0;
    array_size = 0;
    array = nullptr;
    return;
  }
  iterator insert(iterator pos, const T &value) {
    int id = pos.GetIndex();
    return insert(id, value);
  }
  iterator insert(const size_t &ind, const T &value) {
    if (ind > total) {
      throw(2);
    }
    if (total == 0) {
      this->push_back(value);
      return iterator(0, this);
    }
    if (total + 1 >= array_size) {
      DoubleArray();
    }
    total++;
    array[total - 1] = nullptr;
    std::memmove(array + ind + 1, array + ind, sizeof(T *) * (total - ind - 1));
    array[ind] = new T(value);
    return iterator(ind, this);
  }
  iterator erase(iterator pos) {
    int id = pos.GetIndex();
    return erase(id);
  }
  iterator erase(const size_t &ind) {
    if (ind >= total) {
      throw(2);
    }
    std::memmove(array + ind, array + ind + 1, sizeof(T *) * (total - ind - 1));
    total--;
    if (total <= (array_size / 4)) {
      ShrinkArray();
    }
    return iterator(ind, this);
  }
  void push_back(const T &value) {
    if (array_size <= (total + 1)) {
      DoubleArray();
    }
    array[total] = value;
    total++;
    return;
  }
  void pop_back() {
    if (total == 0) {
      throw(3);
    }
    total--;
    if (total <= (array_size / 4)) {
      ShrinkArray();
    }
    return;
  }
};
} // namespace sjtu
namespace sjtu {
class ReadWriteLock {
private:
  std::mutex my_mutex;
  std::condition_variable my_cv;
  bool is_writing = false;
  sjtu::list<std::thread::id> my_queue;
  int is_reading = 0;

public:
  ReadWriteLock() = default;
  void ReadLock() {
    std::unique_lock lock(my_mutex);
    my_queue.push_back(std::this_thread::get_id());
    my_cv.wait(lock, [&] {
      return (!is_writing) && (std::this_thread::get_id() == my_queue.front());
    });
    is_reading++;
    my_queue.pop_front();
    my_cv.notify_all();
    return;
  }
  void WriteLock() {
    std::unique_lock lock(my_mutex);
    my_queue.push_back(std::this_thread::get_id());
    my_cv.wait(lock, [&] {
      return ((!is_writing) &&
              (std::this_thread::get_id() == my_queue.front()) &&
              (!is_reading));
    });
    is_writing = true;
    my_queue.pop_front();
    return;
  }
  void ReadUnlock() {
    std::unique_lock lock(my_mutex);
    is_reading--;
    my_cv.notify_all();
    return;
  }
  void WriteUnlock() {
    std::unique_lock lock(my_mutex);
    is_writing = false;
    my_cv.notify_all();
    return;
  }
  int Waiting() { return my_queue.size(); }
  bool Empty() { return my_queue.empty(); }
  void CheckStatus() {
    std::cout << "read" << is_reading << ' ' << "write" << is_writing
              << std::endl;
  }
};
} // namespace sjtu
// namespace sjtu {
// class ReadWriteLock {
// private:
//   std::shared_mutex my_mutex;

// public:
//   ReadWriteLock() = default;
//   void ReadLock() {
//     my_mutex.lock_shared();
//     return;
//   }
//   void WriteLock() {
//     my_mutex.lock();
//     return;
//   }
//   void ReadUnlock() {
//     my_mutex.unlock_shared();
//     return;
//   }
//   void WriteUnlock() {
//     my_mutex.unlock();
//     return;
//   }
// };
// } // namespace sjtu
template <class W, int info_len = 3> class MemoryRiver { // 应当采取3个参数。
  // 一个存储目前的元素个数，一个存储目前的根节点，一个存储当前的块应该写入到哪里。
private:
  std::fstream file;
  std::string file_name; // 文件名。
  int config[info_len];
  int sizeofT = sizeof(W); // W的大小。

public:
  MemoryRiver() {}

  MemoryRiver(const std::string &file_name) : file_name(file_name) {
    initialise();
    return;
  }
  ~MemoryRiver() {
    file.open(file_name + "_config");
    for (int i = 0; i < info_len; i++) {
      file.seekp(i * sizeof(int), std::fstream::beg);
      file.write(reinterpret_cast<char *>(&config[i]), sizeof(int));
    }
    file.close();
  }
  void ChangeName(std::string res) {
    file_name = res;
    initialise();
  }

  void
  initialise() { // 如果文件不存在，则会进行初始化操作，建立文件并初始化参数。
    std::filesystem::path test(file_name);
    if (std::filesystem::exists(test)) {
      file.open(file_name + "_config");
      for (int i = 0; i < info_len; i++) {
        file.seekg(i * sizeof(int));
        file.read(reinterpret_cast<char *>(&config[i]), sizeof(int));
      }
      file.close();
      return;
    }
    file.open(file_name, std::fstream::out);
    file.close();
    file.open(file_name + "_config", std::fstream::out);
    int tmp = 0;
    for (int i = 0; i < info_len; ++i)
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
    for (int i = 0; i < info_len; i++) {
      config[i] = 0;
    }
  }

  // 读出第n个int的值赋给tmp，1_base
  void get_info(int &tmp, int n) {
    tmp = config[n - 1];
    return;
  }

  // 将tmp写入第n个int的位置，1_base
  void write_info(int tmp, int n) {
    config[n - 1] = tmp;
    return;
  }

  // 在文件合适位置写入类对象t，并返回写入的位置索引index
  // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  // 位置索引index可以取为对象写入的起始位置,1base
  void write(W &t, int which_node, int size = 1) {
    int place = (which_node - 1) * sizeofT;
    file.open(file_name);
    file.seekp(place);
    file.write(reinterpret_cast<char *>(&t), sizeofT * size);
    file.close();
    return;
  }
  void read(W &t, int which_node, int size = 1) {
    int place = (which_node - 1) * sizeofT;
    file.open(file_name);
    file.seekg(place);
    file.read(reinterpret_cast<char *>(&t), sizeofT * size);
    file.close();
    return;
  }
  void clear() {
    file.open(file_name, std::ios::out);
    file.close();
    for (int i = 0; i < info_len; i++) {
      config[i] = 0;
    }
    return;
  }
};
inline unsigned long long MyHash(const std::string &txt,
                                 const unsigned long long &exp) {
  unsigned long long ans = 0;
  for (int i = 0; i < txt.size(); i++) {
    ans *= exp;
    ans += txt[i];
  }
  return ans;
}
template <class Value = int, int size = 168, int redundency = 6,
          int cachesize = 200>
class BPT {
private:
  struct MyData {
    unsigned long long hash1 = 0;
    unsigned long long hash2 = 0;
    Value value;
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
    int empty[redundency];
    MyData datas[size + 1];
    int left_sibling = 0;
    int right_sibling = 0;
    int now_size = 0;
    int pos = 0;
  }nothing;

private:
  std::mutex change_locks;
  sjtu::ReadWriteLock empty_lock;
  sjtu::vector<int> recycle;
  sjtu::ReadWriteLock busy_lock;
  std::map<MyData, sjtu::ReadWriteLock *> is_using;
  sjtu::ReadWriteLock CheckUsing;
  sjtu::ReadWriteLock recycle_protection;
  std::atomic_int B_total = 0;
  std::atomic_int B_root = 0;
  std::atomic_int B_current = 0;
  sjtu::vector<sjtu::ReadWriteLock *> locks;
  std::atomic_bool root_safe = true;
  sjtu::ReadWriteLock root_protection;
  std::mutex cache_lock;
  sjtu::list<Node> mycache;
  MemoryRiver<Node, 3> mydatabase;
  MemoryRiver<int, 1> myrecycle;
  Node ReadwithCache(int pos) {
    std::unique_lock guard(cache_lock);
    for (auto it = mycache.begin(); it != mycache.end(); it++) {
      if (it->pos == pos) {
        Node res = *it;
        mycache.erase(it);
        mycache.push_front(res);
        return res;
      }
    }
    Node res;
    mydatabase.read(res, pos);
    mycache.push_front(res);
    if (mycache.size() > cachesize) {
      Node the_back = mycache.back();
      mycache.pop_back();
      mydatabase.write(the_back, the_back.pos);
    }
    // Node res;
    // mydatabase.read(res, pos);
    return res;
  }
  void WritewithCache(const Node &to_write) {
    std::unique_lock guard(cache_lock);
    for (auto it = mycache.begin(); it != mycache.end(); it++) {
      if (it->pos == to_write.pos) {
        mycache.erase(it);
        mycache.push_front(to_write);
        return;
      }
    }
    mycache.push_front(to_write);
    if (mycache.size() > cachesize) {
      Node the_back = mycache.back();
      mycache.pop_back();
      mydatabase.write(the_back, the_back.pos);
    }
    // mydatabase.write(to_write, to_write.pos);
    return;
  }
  void ReMoveFromCache(const int &pos) {
    std::unique_lock guard(cache_lock);
    for (auto it = mycache.begin(); it != mycache.end(); it++) {
      if (it->pos == pos) {
        mycache.erase(it);
        break;
      }
    }
    return;
  }
  void NodeInsert(const MyData &to_insert, const int &pos, const int &last_node,
                  const Node &last_parent) {
    locks[pos]->WriteLock();
    // std::cout << "OK" << pos << std::endl;
    Node res;
    res = ReadwithCache(pos);
    if (last_node == 0) {
      if (((res.now_size > (size - 10)) && B_total < size) || (B_total < 10) ||
          (res.now_size == (size - 1)) || (res.now_size == 1)) {
        root_safe = false;
      } else {
        root_safe = true;
      }
    }
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
      WritewithCache(res);
    } else {
      if (find == (res.now_size)) {
        find--;
        auto x = to_insert;
        x.son = res.datas[find].son;
        res.datas[find] = x;
        WritewithCache(res);
      }
      Node to_check;
      locks[res.datas[find].son]->WriteLock();
      to_check = ReadwithCache(res.datas[find].son);
      locks[res.datas[find].son]->WriteUnlock();
      if (to_check.now_size < (size - 1)) {
        locks[pos]->WriteUnlock();
        NodeInsert(to_insert, res.datas[find].son, res.pos, res);
        return;
      }
      NodeInsert(to_insert, res.datas[find].son, res.pos, res);
    }
    if (res.now_size >= size) {
      Split(res.pos, last_node, last_parent);
    }
    locks[pos]->WriteUnlock();
    return;
  }
  void Split(const int &pos, const int &last_node, Node parent) {
    Node res;
    Node new_node;
    res = ReadwithCache(pos);
    int now_size = res.now_size;
    int half = now_size >> 1;
    std::memmove(&new_node.datas[0], &res.datas[half],
                 (now_size - half) * sizeof(MyData));
    res.now_size = half;
    new_node.now_size = now_size - half;
    new_node.right_sibling = res.right_sibling;
    new_node.left_sibling = res.pos;
    recycle_protection.WriteLock();
    if (recycle.empty()) {
      B_current++;
      new_node.pos = B_current; // 至此，所有新节点已经准备完毕。
      sjtu::ReadWriteLock *res = new sjtu::ReadWriteLock();
      std::unique_lock safe_lock(change_locks);
      locks.push_back(res);
      safe_lock.unlock();
    } else {
      new_node.pos = recycle.back();
      recycle.pop_back();
    }
    recycle_protection.WriteUnlock();
    res.right_sibling = new_node.pos;
    Node res1;
    locks[new_node.right_sibling]->WriteLock();
    res1 = ReadwithCache(new_node.right_sibling);
    res1.left_sibling = new_node.pos;
    WritewithCache(res1);
    locks[res1.pos]->WriteUnlock();
    // update_to_here
    MyData index1, index2;
    index1 = res.datas[half - 1];
    index2 = new_node.datas[new_node.now_size - 1];
    index1.son = res.pos;
    index2.son = new_node.pos;
    if (last_node) {
      for (int i = 0; i < parent.now_size; i++) {
        if (parent.datas[i] == index2) {
          memmove(&parent.datas[i + 1], &parent.datas[i],
                  (parent.now_size - i) * sizeof(MyData));
          parent.datas[i] = index1;
          parent.datas[i + 1] = index2;
          parent.now_size++;
          WritewithCache(parent);
          break;
        }
      }
    } else {
      int current;
      recycle_protection.WriteLock();
      if (recycle.empty()) {
        current = B_current;
        current++;
        B_current = current;
        std::unique_lock safe_lock(change_locks);
        sjtu::ReadWriteLock *res = new sjtu::ReadWriteLock();
        locks.push_back(res);
        safe_lock.unlock();
      } else {
        current = recycle.back();
        recycle.pop_back();
      }
      recycle_protection.WriteUnlock();
      Node new_alloc;
      new_alloc.now_size = 2;
      new_alloc.left_sibling = 0;
      new_alloc.right_sibling = 0;
      new_alloc.pos = current;
      new_alloc.datas[1] = index2;
      new_alloc.datas[0] = index1;
      WritewithCache(new_alloc);
      B_root = current;
    }
    WritewithCache(res);
    WritewithCache(new_node);
    return;
  }
  bool NodeErase(const int &pos, const int &last_pos, MyData to_delete,
                 const int &where, const int &how_many) {
    // pos表示当前节点号、父亲节点号、待删除内容，这个节点是父亲节点的多少号元素、父亲节点有多少个元素。
    // std::cout << "ASKFOR" << pos << std::endl;
    locks[pos]->WriteLock();
    // std::cout << to_delete.value << std::endl;
    Node res;
    res = ReadwithCache(pos);
    if (last_pos == 0) {
      if (((res.now_size > (size - 2)) && B_total < size) || (B_total < 10) ||
          (res.now_size == (size - 1)) || (res.now_size == 1)) {
        root_safe = false;
      } else {
        root_safe = true;
      }
    }
    int found = 0;
    auto last_one = res.datas[res.now_size - 1];
    for (int i = 0; i < res.now_size; i++) {
      if ((res.datas[i] > to_delete) || (res.datas[i] == to_delete)) {
        if (res.datas[i].son == 0) {
          if (res.datas[i] != to_delete) {
            locks[pos]->WriteUnlock();
            return false; // didn't find~
          } else {
            if (res.now_size == 1) { // 说明删空了。
              ReMoveFromCache(res.pos);
              recycle_protection.WriteLock();
              recycle.push_back(res.pos);
              recycle_protection.WriteUnlock();
              if (res.left_sibling != 0) {
                Node left;
                locks[res.left_sibling]->WriteLock();
                left = ReadwithCache(res.left_sibling);
                left.right_sibling = res.right_sibling;
                WritewithCache(left);
                locks[res.left_sibling]->WriteUnlock();
              }
              if (res.right_sibling != 0) {
                Node right;
                locks[res.right_sibling]->WriteLock();
                right = ReadwithCache(res.right_sibling);
                right.left_sibling = res.left_sibling;
                WritewithCache(right);
                locks[res.right_sibling]->WriteUnlock();
              }
              auto x = res.datas[0];
              if (last_pos) {
                Node parent;
                parent = ReadwithCache(last_pos);
                for (int i = 0; i < parent.now_size; i++) {
                  if (parent.datas[i] == x) {
                    if (i != (parent.now_size - 1)) {
                      std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                   (parent.now_size - i - 1) * sizeof(MyData));
                    }
                    parent.now_size--;
                    WritewithCache(parent);
                  }
                }
              }
              res.now_size--;
              WritewithCache(res);
              locks[pos]->WriteUnlock();
              return true;
            }
            if (i != (res.now_size - 1)) {
              std::memmove(&res.datas[i], &res.datas[i + 1],
                           (res.now_size - i - 1) * sizeof(MyData));
            }
            res.now_size--;
            if (last_pos == 0) {
              WritewithCache(res);
              locks[pos]->WriteUnlock();
              return true;
            }
            if (res.now_size < ((size >> 1) - 1)) {
              if ((where == 0) && (how_many == 1) ||
                  (where == how_many - 1)) { // 说明这个节点没有办法进行调整。
                if (i == res.now_size) {
                  Node parent;
                  parent = ReadwithCache(last_pos);
                  auto to_change = res.datas[res.now_size];
                  auto to_update = res.datas[res.now_size - 1];
                  for (int i = 0; i < parent.now_size; i++) {
                    if (to_change == parent.datas[i]) {
                      to_update.son = parent.datas[i].son;
                      parent.datas[i] = to_update;
                      WritewithCache(parent);
                      WritewithCache(res);
                      locks[pos]->WriteUnlock();
                      return true;
                    }
                  }
                }
                WritewithCache(res);
                locks[pos]->WriteUnlock();
                return true;
              }
              if (where != (how_many - 1)) {
                int right;
                right = res.right_sibling;
                Node right_s;
                locks[right]->WriteLock();
                right_s = ReadwithCache(res.right_sibling);
                if (right_s.now_size >= (size >> 1)) {
                  auto to_update = res.datas[res.now_size];
                  res.datas[res.now_size] = right_s.datas[0];
                  res.now_size++;
                  WritewithCache(res);
                  std::memmove(&right_s.datas[0], &right_s.datas[1],
                               (right_s.now_size - 1) * sizeof(MyData));
                  right_s.now_size--;
                  WritewithCache(right_s);
                  locks[right]->WriteUnlock();
                  Node parent;
                  parent = ReadwithCache(last_pos);
                  for (int i = 0; i < parent.now_size; i++) {
                    if (parent.datas[i] == to_update) {
                      auto to_change = res.datas[res.now_size - 1];
                      to_change.son = parent.datas[i].son;
                      parent.datas[i] = to_change;
                      WritewithCache(parent);
                      locks[pos]->WriteUnlock();
                      return true; // 借块操作完成。右兄弟节点、本身、父节点均得到更新。
                    }
                  }
                } else { // 说明旁边的节点数目数目已经足够少。
                  recycle_protection.WriteLock();
                  recycle.push_back(right);
                  recycle_protection.WriteUnlock();
                  ReMoveFromCache(right);
                  auto to_change = res.datas[res.now_size];
                  std::memmove(&res.datas[res.now_size], &right_s.datas[0],
                               right_s.now_size * sizeof(MyData));
                  res.now_size += right_s.now_size;
                  Node parent;
                  parent = ReadwithCache(last_pos);
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
                  WritewithCache(parent);
                  res.right_sibling = right_s.right_sibling;
                  if (right_s.right_sibling != 0) {
                    Node double_right;
                    locks[right_s.right_sibling]->WriteLock();
                    double_right = ReadwithCache(right_s.right_sibling);
                    double_right.left_sibling = pos;
                    WritewithCache(double_right);
                    locks[right_s.right_sibling]->WriteUnlock();
                  }
                  WritewithCache(res);
                  locks[right]->WriteUnlock();
                  locks[pos]->WriteUnlock();
                  return true;
                }
              }
            } else {
              if (i == res.now_size) {
                auto to_erase = res.datas[res.now_size];
                auto to_update = res.datas[res.now_size - 1];
                Node parent;
                parent = ReadwithCache(last_pos);
                for (int i = 0; i < parent.now_size; i++) {
                  if (parent.datas[i] == to_erase) {
                    to_update.son = parent.datas[i].son;
                    parent.datas[i] = to_update;
                    WritewithCache(parent);
                    break;
                  }
                }
              }
              WritewithCache(res);
              locks[pos]->WriteUnlock();
              return true;
            }
          }
        } else {
          locks[res.datas[i].son]->WriteLock();
          Node to_check;
          to_check = ReadwithCache(res.datas[i].son);
          locks[res.datas[i].son]->WriteUnlock();
          if (res.datas[i] != to_delete &&
              (to_check.now_size > (size / 2 + 3))) {
            locks[pos]->WriteUnlock();
            bool ans =
                NodeErase(res.datas[i].son, pos, to_delete, i, res.now_size);
            return ans;
          } // 说明不可能产生影响。
          bool ans =
              NodeErase(res.datas[i].son, pos, to_delete, i, res.now_size);
          if (ans == false) {
            locks[pos]->WriteUnlock();
            return false;
          }
          if (last_pos == 0) {
            locks[pos]->WriteUnlock();
            return true;
          }
          res = ReadwithCache(pos);
          if (res.now_size == 0) { // 说明删空了。
            ReMoveFromCache(res.pos);
            recycle_protection.WriteLock();
            recycle.push_back(res.pos);
            recycle_protection.WriteUnlock();
            if (res.left_sibling != 0) {
              Node left;
              locks[res.left_sibling]->WriteLock();
              left = ReadwithCache(res.left_sibling);
              left.right_sibling = res.right_sibling;
              WritewithCache(left);
              locks[res.left_sibling]->WriteUnlock();
            }
            if (res.right_sibling != 0) {
              Node right;
              locks[res.right_sibling]->WriteLock();
              right = ReadwithCache(res.right_sibling);
              right.left_sibling = res.left_sibling;
              WritewithCache(right);
              locks[res.right_sibling]->WriteUnlock();
            }
            auto x = res.datas[0];
            if (last_pos) {
              Node parent;
              parent = ReadwithCache(last_pos);
              for (int i = 0; i < parent.now_size; i++) {
                if (parent.datas[i] == x) {
                  if (i != (parent.now_size - 1)) {
                    std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                 (parent.now_size - i - 1) * sizeof(MyData));
                  }
                  parent.now_size--;
                  WritewithCache(parent);
                  break;
                }
              }
            }
            WritewithCache(res);
            locks[pos]->WriteUnlock();
            return true;
          }
          if (res.datas[res.now_size - 1] != last_one) { // 应当向上修改。
            Node parent;
            parent = ReadwithCache(last_pos);
            for (int i = 0; i < parent.now_size; i++) {
              if (parent.datas[i] == last_one) {
                auto to_update = res.datas[res.now_size - 1];
                to_update.son = parent.datas[i].son;
                parent.datas[i] = to_update;
                WritewithCache(parent);
                break;
              }
            }
          }
          if (res.now_size < ((size >> 1) - 1)) {
            if (((where == 0) && (how_many == 1)) ||
                (where == how_many - 1)) { // 说明这个节点没有办法进行调整。
              if (i == res.now_size) {
                Node parent;
                parent = ReadwithCache(last_pos);
                auto to_change = res.datas[res.now_size];
                auto to_update = res.datas[res.now_size - 1];
                for (int i = 0; i < parent.now_size; i++) {
                  if (to_change == parent.datas[i]) {
                    to_update.son = parent.datas[i].son;
                    parent.datas[i] = to_update;
                    WritewithCache(parent);
                    WritewithCache(res);
                    locks[pos]->WriteUnlock();
                    return true;
                  }
                }
              }
              WritewithCache(res);
              locks[pos]->WriteUnlock();
              return true;
            }
            if (where != (how_many - 1)) {
              int right;
              right = res.right_sibling;
              Node right_s;
              locks[right]->WriteLock();
              right_s = ReadwithCache(right);
              if (right_s.now_size >= (size >> 1)) {
                auto to_update = res.datas[res.now_size];
                res.datas[res.now_size] = right_s.datas[0];
                res.now_size++;
                WritewithCache(res);
                std::memmove(&right_s.datas[0], &right_s.datas[1],
                             (right_s.now_size - 1) * sizeof(MyData));
                right_s.now_size--;
                WritewithCache(right_s);
                locks[right]->WriteUnlock();
                Node parent;
                parent = ReadwithCache(last_pos);
                for (int i = 0; i < parent.now_size; i++) {
                  if (parent.datas[i] == to_update) {
                    auto to_change = res.datas[res.now_size - 1];
                    to_change.son = parent.datas[i].son;
                    parent.datas[i] = to_change;
                    WritewithCache(parent);
                    locks[pos]->WriteUnlock();
                    return true; // 借块操作完成。右兄弟节点、本身、父节点均得到更新。
                  }
                }
              } else { // 说明旁边的节点数目数目已经足够少。
                recycle_protection.WriteLock();
                recycle.push_back(right);
                recycle_protection.WriteUnlock();
                ReMoveFromCache(right);
                auto to_change = res.datas[res.now_size];
                std::memmove(&res.datas[res.now_size], &right_s.datas[0],
                             right_s.now_size * sizeof(MyData));
                res.now_size += right_s.now_size;
                Node parent;
                parent = ReadwithCache(last_pos);
                for (int i = 0; i < parent.now_size; i++) {
                  if (parent.datas[i] == to_change) {
                    if (i != (parent.now_size - 1)) {
                      std::memmove(&parent.datas[i], &parent.datas[i + 1],
                                   (parent.now_size - i - 1) * sizeof(MyData));
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
                WritewithCache(parent);
                res.right_sibling = right_s.right_sibling;
                if (right_s.right_sibling != 0) {
                  Node double_right;
                  locks[right_s.right_sibling]->WriteLock();
                  double_right = ReadwithCache(right_s.right_sibling);
                  double_right.left_sibling = pos;
                  WritewithCache(double_right);
                  locks[right_s.right_sibling]->WriteUnlock();
                }
                WritewithCache(res);
                locks[right]->WriteUnlock();
                locks[pos]->WriteUnlock();
                return true;
              }
            }
          } else {
            if (i == res.now_size) {
              auto to_erase = res.datas[res.now_size];
              auto to_update = res.datas[res.now_size - 1];
              Node parent;
              parent = ReadwithCache(last_pos);
              for (int i = 0; i < parent.now_size; i++) {
                if (parent.datas[i] == to_erase) {
                  to_update.son = parent.datas[i].son;
                  parent.datas[i] = to_update;
                  WritewithCache(parent);
                  break;
                }
              }
            }
            WritewithCache(res);
            locks[pos]->WriteUnlock();
            return true;
          }
        }
      }
    }
    locks[pos]->WriteUnlock();
    return false;
  }

public:
  BPT() = delete;
  BPT(std::string name) {
    mydatabase.ChangeName(name);
    int res1, res2, res3;
    mydatabase.get_info(res1, 1);
    mydatabase.get_info(res2, 2);
    mydatabase.get_info(res3, 3);
    B_total = res1;
    B_root = res2;
    B_current = res3;
    std::string recyle_name = name + "_recycle";
    myrecycle.ChangeName(recyle_name);
    int recycle_total;
    myrecycle.get_info(recycle_total, 1);
    for (int i = 1; i <= recycle_total; i++) {
      int res;
      myrecycle.read(res, i);
      recycle.push_back(res);
    }
    for (int i = 0; i <= B_current; i++) {
      sjtu::ReadWriteLock *res = new sjtu::ReadWriteLock();
      locks.push_back(res);
    }
  }
  ~BPT() {
    Node to_check;
    to_check = ReadwithCache(B_root);
    while ((to_check.now_size == 1) && (to_check.datas[0].son != 0)) {
      recycle.push_back(B_root);
      B_root = to_check.datas[0].son;
      to_check = ReadwithCache(B_root);
    }
    mydatabase.write_info(B_total, 1);
    mydatabase.write_info(B_root, 2);
    mydatabase.write_info(B_current, 3);
    myrecycle.write_info(recycle.size(), 1);
    for (int i = 0; i < recycle.size(); i++) {
      myrecycle.write(recycle[i], i + 1);
    }
    for (int i = 0; i < locks.size(); i++) {
      delete locks[i];
    }
    for (auto it = mycache.begin(); it != mycache.end(); it++) {
      mydatabase.write(*it, it->pos);
    }
  }
  void Insert(const unsigned long long &hash1, unsigned long long hash2,
              const int &value) {
    MyData a;
    a.hash1 = hash1;
    a.hash2 = hash2;
    a.value = value;
    if (B_total == 0) {
      empty_lock.WriteLock();
      if (!B_total) {
        root_safe = false;
        sjtu::ReadWriteLock *res = new sjtu::ReadWriteLock();
        locks.push_back(res);
        locks[1]->WriteLock();
        B_root = 1;
        B_total = 1;
        B_current = 1;
        Node res1;
        recycle.clear();
        res1.datas[0].hash1 = hash1;
        res1.datas[0].hash2 = hash2;
        res1.datas[0].value = value;
        res1.now_size = 1;
        res1.pos = 1;
        WritewithCache(res1);
        locks[1]->WriteUnlock();
        empty_lock.WriteUnlock();
        return;
      }
      empty_lock.WriteUnlock();
    }
    MyData res;
    res.hash1 = hash1;
    res.hash2 = hash2;
    res.value = value;
    if (!root_safe) {
      root_protection.WriteLock();
      NodeInsert(res, B_root, 0, nothing);
      B_total++;
      root_protection.WriteUnlock();
    } else {
      NodeInsert(res, B_root, 0, nothing);
      B_total++;
    }
    // locks[1]->CheckStatus();
    return;
  }
  void find(const unsigned long long &hash_1,
            const unsigned long long &hash_2) {
    sjtu::list<int> over_claim;
    if (B_total == 0) {
      empty_lock.ReadLock();
      if (!B_total) {
        std::cout << "null" << '\n';
        empty_lock.ReadUnlock();
        return;
      }
      empty_lock.ReadUnlock();
    }
    Node res;
    MyData to_find;
    to_find.hash1 = hash_1;
    to_find.hash2 = hash_2;
    to_find.value = minus_max;
    int own = B_root;
    locks[B_root]->ReadLock();
    res = ReadwithCache(B_root);
    while (res.datas[0].son != 0) {
      for (int i = 0; i < res.now_size; i++) {
        if (to_find < res.datas[i]) {
          for (int j = i + 1; j < res.now_size; j++) {
            if (to_find > res.datas[j]) {
              break;
            }
            locks[res.datas[j].son]->ReadLock();
            over_claim.push_back(res.datas[j].son);
          }
          locks[res.datas[i].son]->ReadLock();
          locks[own]->ReadUnlock();
          own = res.datas[i].son;
          res = ReadwithCache(own);
          break;
        }
        if (i == (res.now_size - 1)) {
          std::cout << "null" << '\n';
          locks[own]->ReadUnlock();
          while (!over_claim.empty()) {
            locks[over_claim.back()]->ReadUnlock();
            over_claim.pop_back();
          }
          return;
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
      locks[own]->ReadUnlock();
      while (!over_claim.empty()) {
        locks[over_claim.back()]->ReadUnlock();
        over_claim.pop_back();
      }
      return;
    }
    while ((hash_1 == res.datas[found].hash1) &&
           (hash_2 == res.datas[found].hash2)) {
      std::cout << res.datas[found].value << ' ';
      found++;
      if (found == res.now_size) {
        if (res.right_sibling == 0) {
          std::cout << '\n';
          locks[own]->ReadUnlock();
          while (!over_claim.empty()) {
            locks[over_claim.back()]->ReadUnlock();
            over_claim.pop_back();
          }
          return;
        }
        bool flag = false;
        for (auto it = over_claim.begin(); it != over_claim.end(); it++) {
          if (*it == res.right_sibling) {
            flag = true;
            over_claim.erase(it);
            break;
          }
        }
        if (!flag) {
          locks[res.right_sibling]->ReadLock();
        }
        locks[own]->ReadUnlock();
        own = res.right_sibling;
        res = ReadwithCache(own);
        found = 0;
      }
    }
    std::cout << '\n';
    locks[own]->ReadUnlock();
    while (!over_claim.empty()) {
      locks[over_claim.back()]->ReadUnlock();
      over_claim.pop_back();
    }
    return;
  }

  void Erase(const unsigned long long &hash_1, const unsigned long long &hash_2,
             const int &value) {
    MyData to_delete;
    MyData to_delete_valid;
    to_delete.hash1 = hash_1;
    to_delete.hash2 = hash_2;
    to_delete.value = value;
    // std::cout << "ROOT" << B_root << std::endl;
    if (!B_total) {
      empty_lock.WriteLock();
      if (!B_total) {
        empty_lock.WriteUnlock();
        return;
      }
      empty_lock.WriteUnlock();
    }
    if (!root_safe) {
      root_protection.WriteLock();
      if (NodeErase(B_root, 0, to_delete, 0, 0) != false) {
        B_total--;
      }
      root_protection.WriteUnlock();
      return;
    }
    if (NodeErase(B_root, 0, to_delete, 0, 0) != false) {
      B_total--;
    }
    return;
  }
};
std::string ProcessTxt(std::string &txt) {
  while (txt[0] == ' ') {
    txt.erase(0, 1);
  }
  while (txt[txt.size() - 1] == ' ') {
    txt.erase(txt.size() - 1, 1);
  }
  std::string tmp = "";
  while ((!txt.empty()) && txt[0] != ' ') {
    tmp += txt[0];
    txt.erase(0, 1);
  }
  while ((!txt.empty()) && txt[0] == ' ') {
    txt.erase(0, 1);
  }
  return tmp;
}
BPT<int> test("database");
std::thread threads[7];
std::atomic_bool is_free[7];
sjtu::list<std::string> commands;
std::mutex commands_protection;
std::string GetCommand() {
  commands_protection.lock();
  if (commands.empty()) {
    commands_protection.unlock();
    return "No";
  }
  std::string task = commands.front();
  commands.pop_front();
  commands_protection.unlock();
  return task;
}
void Listen() {
  while (true) {
    std::string op;
    std::string txt = GetCommand();
    op = ProcessTxt(txt);
    if (op == "insert") {
      std::string index;
      int value;
      index = ProcessTxt(txt);
      value = std::stoi(ProcessTxt(txt));
      unsigned long long hash1, hash2;
      hash1 = MyHash(index, exp1);
      hash2 = MyHash(index, exp2);
      test.Insert(hash1, hash2, value);
      continue;
    }
    if (op == "find") {
      std::string index;
      index = ProcessTxt(txt);
      unsigned long long hash1, hash2;
      hash1 = MyHash(index, exp1);
      hash2 = MyHash(index, exp2);
      test.find(hash1, hash2);
      continue;
    }
    if (op == "delete") {
      std::string index;
      int value;
      index = ProcessTxt(txt);
      value = std::stoi(ProcessTxt(txt));
      unsigned long long hash1, hash2;
      hash1 = MyHash(index, exp1);
      hash2 = MyHash(index, exp2);
      test.Erase(hash1, hash2, value);
      continue;
    }
    if (op == "No") {
      return;
    }
  }
}
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  auto start = std::chrono::high_resolution_clock::now();
  int hardware = std::thread::hardware_concurrency();
  hardware = hardware ? hardware - 1 : 1;
  freopen("t4.txt", "r", stdin);
  freopen("multi.txt", "w", stdout);
  std::string first;
  std::getline(std::cin, first);
  int num = std::stoi(first);
  for (int i = 1; i <= num; i++) {
    std::string res;
    std::getline(std::cin, res);
    commands.push_back(res);
  }
  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = now - start;
  std::cout << "Time to execute: " << diff.count() << " s\n";
  for (int i = 0; i < 7; i++) {
    threads[i] = std::thread(Listen);
  }
  now = std::chrono::high_resolution_clock::now();
  diff = now - start;
  std::cout << "Time to execute: " << diff.count() << " s\n";
  for (int i = 0; i < 7; i++) {
    if (threads[i].joinable()) {
      threads[i].join();
    }
    now = std::chrono::high_resolution_clock::now();
    diff = now - start;
    std::cout << "Time to execute: " << diff.count() << " s\n";
  }
  auto end = std::chrono::high_resolution_clock::now();
  diff = end - start;
  std::cout << "Time to execute: " << diff.count() << " s\n";
  return 0;
}

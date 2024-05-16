#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace sjtu {

class exception {
protected:
  const std::string variant = "";
  std::string detail = "";

public:
  exception() {}
  exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
  virtual std::string what() { return variant + " " + detail; }
};

class index_out_of_bound : public exception {
  /* __________________________ */
};

class runtime_error : public exception {
  /* __________________________ */
};

class invalid_iterator : public exception {
  /* __________________________ */
};

class container_is_empty : public exception {
  /* __________________________ */
};
} // namespace sjtu
namespace sjtu {

template <class T1, class T2> class pair {
public:
  T1 first;
  T2 second;
  constexpr pair() : first(), second() {}
  pair(const pair &other) = default;
  pair(pair &&other) = default;
  pair(const T1 &x, const T2 &y) : first(x), second(y) {}
  template <class U1, class U2> pair(U1 &&x, U2 &&y) : first(x), second(y) {}
  template <class U1, class U2>
  pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
  template <class U1, class U2>
  pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
};

} // namespace sjtu

const unsigned long long exp1 = 13331, exp2 = 131;
const int minus_max = -2147483648;
const int maxn = 2147483647;
namespace sjtu {
struct my_true_type {
  bool value = true;
};
struct my_false_type {
  bool value = false;
};
template <typename T> struct my_type_traits {
  using iterator_assignable = typename T::iterator_assignable;
  bool operator()() {
    iterator_assignable t;
    return t.value;
  }
};
template <class Key, class T, class Compare = std::less<Key>> class map;
template <class Key, class T, class Compare> class map {
  class Node;

private:
  Node *root = nullptr;
  int total = 0;
  class Node {
  public:
    typedef pair<const Key, T> value_type;

  private:
    value_type *data = nullptr;
    Node *left_son = nullptr;
    Node *right_son = nullptr;
    Node *parent = nullptr;
    int level = 1;

  public:
    Node() = default;
    Node(Key k, T v, Node *p = nullptr) {
      data = new value_type(k, v);
      left_son = nullptr;
      right_son = nullptr;
      parent = p;
      level = 1;
    }
    Node(const Node &other) = delete;
    ~Node() { delete data; }
    inline Node *Split() {
      if (right_son == nullptr) {
        return this;
      }
      if (right_son->right_son == nullptr) {
        return this;
      }
      if (right_son->level != level || right_son->right_son->level != level) {
        return this;
      }
      Node *tmp = this->right_son;
      tmp->parent = parent;
      right_son = tmp->left_son;
      if (right_son != nullptr) {
        right_son->parent = this;
      }
      tmp->left_son = this;
      parent = tmp;
      (tmp->level)++;
      return tmp;
    }
    inline Node *Skew() {
      if (left_son == nullptr) {
        return this;
      }
      if (left_son->level != level) {
        return this;
      }
      Node *tmp = this->left_son;
      left_son = left_son->right_son;
      if (left_son != nullptr) {
        left_son->parent = this;
      }
      tmp->parent = parent;
      tmp->right_son = this;
      parent = tmp;
      return tmp;
    }
    Node *FindNode(const Key &k) {
      Compare tool;
      if ((!tool((data->first), k)) && (!tool(k, data->first))) {
        return this;
      }
      if (tool(k, data->first)) {
        if (left_son == nullptr) {
          return nullptr;
        }
        return left_son->FindNode(k);
      } else {
        if (right_son == nullptr) {
          return nullptr;
        }
        return right_son->FindNode(k);
      }
    }
    Node *InsertNode(const Key &k, const T &v, bool &tag) {
      Compare tool;
      if (tool(k, data->first)) {
        if (left_son == nullptr) {
          left_son = new Node(k, v, this);
          return left_son;
        } else {
          Node *tmp = nullptr;
          tmp = left_son->InsertNode(k, v, tag);
          if (left_son != nullptr) {
            left_son = left_son->Skew();
            left_son = left_son->Split();
          }
          return tmp;
        }
      } else {
        if (!tool(data->first, k)) {
          tag = false;
          return this;
        }
        if (right_son == nullptr) {
          right_son = new Node(k, v, this);
          return right_son;
        } else {
          Node *tmp = nullptr;
          tmp = right_son->InsertNode(k, v, tag);
          right_son = right_son->Skew();
          right_son = right_son->Split();
          return tmp;
        }
      }
    }
    void Copy(const Node *other, Node *p) {
      data = new value_type(other->data->first, other->data->second);
      level = other->level;
      parent = p;
      if (other->left_son != nullptr) {
        left_son = new Node();
        left_son->Copy(other->left_son, this);
      } else {
        left_son = nullptr;
      }
      if (other->right_son != nullptr) {
        right_son = new Node();
        right_son->Copy(other->right_son, this);
      } else {
        right_son = nullptr;
      }
      return;
    }
    void Destroy() {
      if (left_son != nullptr) {
        left_son->Destroy();
        delete left_son;
      }
      if (right_son != nullptr) {
        right_son->Destroy();
        delete right_son;
      }
      return;
    }
    friend class map<Key, T, Compare>;
  };
  void RemoveNode(const Key &k, Node *&now) {
    Compare tool;
    if (now == nullptr) {
      throw(sjtu::invalid_iterator());
    }
    if ((!(tool(k, (now->data->first))) && (!(tool((now->data->first), k))))) {
      if (now->left_son != nullptr && now->right_son != nullptr) {
        Node *tmp = now->right_son;
        while (tmp->left_son != nullptr) {
          tmp = tmp->left_son;
        }
        bool is_son = false;
        if (now->right_son == tmp) {
          is_son = true;
        }
        if (tmp->parent->left_son == tmp) {
          tmp->parent->left_son =
              new Node((tmp->data->first), (tmp->data->second), tmp->parent);
          auto new_node = tmp->parent->left_son;
          new_node->left_son = tmp->left_son;
          new_node->right_son = tmp->right_son;
          new_node->level = tmp->level;
        } else {
          tmp->parent->right_son =
              new Node((tmp->data->first), (tmp->data->second), tmp->parent);
          auto new_node = tmp->parent->right_son;
          new_node->left_son = tmp->left_son;
          new_node->right_son = tmp->right_son;
          new_node->level = tmp->level;
        }
        RemoveNode((tmp->data->first), now->right_son);
        if (!is_son) {
          tmp->right_son = now->right_son;
        }
        tmp->left_son = now->left_son;
        tmp->parent = now->parent;
        if (now->left_son != nullptr) {
          tmp->left_son->parent = tmp;
        }
        if (now->right_son != nullptr) {
          tmp->right_son->parent = tmp;
        }
        delete now;
        now = tmp;
      } else {
        Node *tmp = now;
        if (now->left_son == nullptr && now->right_son == nullptr) {
          if (now->parent == nullptr) {
            delete now;
            now = nullptr;
            return;
          } else {
            if (now->parent->left_son == now) {
              now->parent->left_son = nullptr;
              delete tmp;
              now = nullptr;
            } else {
              now->parent->right_son = nullptr;
              delete tmp;
              now = nullptr;
            }
            return;
          }
        }
        auto res = now;
        if (now->left_son == nullptr) {
          now->right_son->parent = now->parent;
          now = now->right_son;
          delete res;
        } else {
          now->left_son->parent = now->parent;
          now = now->left_son;
          delete res;
        }
        return;
      }
    } else {
      if (tool(k, (now->data->first))) {
        RemoveNode(k, now->left_son);
      } else {
        RemoveNode(k, now->right_son);
      }
    }
    if (now->left_son != nullptr) {
      now->left_son = now->left_son->Skew();
      now->left_son = now->left_son->Split();
    }
    if (now->right_son != nullptr) {
      now->right_son = now->right_son->Skew();
      now->right_son = now->right_son->Split();
    }
    if (now->left_son == nullptr || now->right_son == nullptr) {
      now->level = 1;
    } else {
      now->level = std::min(now->left_son->level, now->right_son->level) + 1;
    }
    if (now->right_son != nullptr && now->right_son->level > now->level) {
      now->level++;
    }
    return;
  }

public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  class iterator {
  private:
    Node *ptr;
    map<Key, T, Compare> *which;

  public:
    using my_type = my_true_type;
    using iterator_assignable = my_true_type;
    friend struct my_type_traits<iterator>;
    friend class map;
    iterator() {
      ptr = nullptr;
      which = nullptr;
    }
    iterator(const iterator &other) {
      ptr = other.ptr;
      which = other.which;
    }
    iterator(Node *v, map<Key, T, Compare> *m) {
      ptr = v;
      which = m;
    }
    ~iterator() = default;
    /**
     * TODO iter++
     */
    iterator operator++(int x) {
      auto res = *this;
      Node *tmp = ptr;
      if (ptr == nullptr) {
        throw(invalid_iterator());
      } // end.
      if (tmp->right_son != nullptr) {
        tmp = tmp->right_son;
        while (tmp->left_son != nullptr) {
          tmp = tmp->left_son;
        }
        ptr = tmp;
        return res;
      } else {
        if (tmp->parent == nullptr) {
          ptr = nullptr;
          return res;
        } // to the end.
        while (tmp->parent->right_son == tmp) {
          Node *last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->right_son == last) {
            ptr = nullptr;
            return res;
          }
        }
        ptr = tmp->parent;
        if (ptr != nullptr && ptr->right_son != nullptr) {
        }
        return res;
      }
    }
    /**
     * TODO ++iter
     */
    iterator &operator++() {
      Node *tmp = ptr;
      if (ptr == nullptr) {
        throw(invalid_iterator());
      } // end.
      if (tmp->right_son != nullptr) {
        tmp = tmp->right_son;
        while (tmp->left_son != nullptr) {
          tmp = tmp->left_son;
        }
        ptr = tmp;
        return *this;
      } else {
        if (tmp->parent == nullptr) {
          ptr = nullptr;
          return *this;
        } // to the end.
        Node *last = nullptr;
        while (tmp->parent->right_son == tmp) {
          auto last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->right_son == last) {
            ptr = nullptr;
            return *this;
          }
        }
        ptr = tmp->parent;
        return *this;
      }
    }
    /**
     * TODO iter--
     */
    iterator operator--(int) {
      auto res = *this;
      Node *tmp = ptr;
      if (tmp == nullptr) {
        tmp = which->root;
        if (tmp == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return res;
      } // end.
      if (tmp->left_son != nullptr) {
        tmp = tmp->left_son;
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return res;
      } else {
        if (tmp->parent == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->parent->left_son == tmp) {
          auto last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->left_son == last) {
            throw(invalid_iterator());
          }
        }
        ptr = tmp->parent;
        return res;
      }
    }
    /**
     * TODO --iter
     */
    iterator &operator--() {
      Node *tmp = ptr;
      if (tmp == nullptr) {
        tmp = which->root;
        if (tmp == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return *this;
      } // end.
      if (tmp->left_son != nullptr) {
        tmp = tmp->left_son;
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return *this;
      } else {
        if (tmp->parent == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->parent->left_son == tmp) {
          auto last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->left_son == last) {
            throw(invalid_iterator());
          }
        }
        ptr = tmp->parent;
        return *this;
      }
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    value_type &operator*() const { return *(ptr->data); }
    iterator &operator=(const iterator &other) {
      if (this->ptr == other.ptr && this->which == other.which) {
        return *this;
      } else {
        this->ptr = other.ptr;
        this->which = other.which;
        return *this;
      }
    }
    bool operator==(const iterator &rhs) const {
      return rhs.ptr == ptr && rhs.which == which;
    }
    bool operator==(const const_iterator &rhs) const {
      return rhs.ptr == ptr && rhs.which == which;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return rhs.ptr != ptr || rhs.which != which;
    }
    bool operator!=(const const_iterator &rhs) const {
      return rhs.ptr != ptr || rhs.which != which;
    }

    /**
     * for the support of it->first.
     * See
     * <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
     * for help.
     */
    value_type *operator->() const noexcept { return ptr->data; }
  };
  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
  private:
    Node *ptr;
    map<Key, T, Compare> *which;

  public:
    using my_type = my_true_type;
    using iterator_assignable = my_false_type;
    friend struct my_type_traits<const_iterator>;
    friend class map;
    const_iterator() {
      ptr = nullptr;
      which = nullptr;
    }
    const_iterator(const const_iterator &other) {
      ptr = other.ptr;
      which = other.which;
    }
    const_iterator(const iterator &other) {
      ptr = other.ptr;
      which = other.which;
    }
    const_iterator(Node *v, map<Key, T, Compare> *m) {
      ptr = v;
      which = m;
    }
    ~const_iterator() = default;
    const value_type &operator*() const { return *(ptr->data); }
    const value_type *operator->() const noexcept { return ptr->data; }
    const_iterator operator++(int x) {
      auto res = *this;
      Node *tmp = ptr;
      if (ptr == nullptr) {
        throw(invalid_iterator());
      } // end.
      if (tmp->right_son != nullptr) {
        tmp = tmp->right_son;
        while (tmp->left_son != nullptr) {
          tmp = tmp->left_son;
        }
        ptr = tmp;
        return res;
      } else {
        if (tmp->parent == nullptr) {
          ptr = nullptr;
          return res;
        } // to the end.
        Node *last = nullptr;
        while (tmp->parent->right_son == tmp) {
          auto last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->right_son == last) {
            ptr = nullptr;
            return res;
          }
        }
        ptr = tmp->parent;
        return res;
      }
    }
    /**
     * TODO ++iter
     */
    const_iterator &operator++() {
      Node *tmp = ptr;
      if (ptr == nullptr) {
        throw(invalid_iterator());
      } // end.
      if (tmp->right_son != nullptr) {
        tmp = tmp->right_son;
        while (tmp->left_son != nullptr) {
          tmp = tmp->left_son;
        }
        ptr = tmp;
        return *this;
      } else {
        if (tmp->parent == nullptr) {
          ptr = nullptr;
          return *this;
        } // to the end.
        Node *last = nullptr;
        while (tmp->parent->right_son == tmp) {
          auto last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->right_son == last) {
            ptr = nullptr;
            return *this;
          }
        }
        ptr = tmp->parent;
        return *this;
      }
    }
    /**
     * TODO iter--
     */
    const_iterator operator--(int) {
      auto res = *this;
      Node *tmp = ptr;
      if (tmp == nullptr) {
        tmp = which->root;
        if (tmp == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return res;
      } // end.
      if (tmp->left_son != nullptr) {
        tmp = tmp->left_son;
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return res;
      } else {
        if (tmp->parent == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->parent->left_son == tmp) {
          auto last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->left_son == last) {
            throw(invalid_iterator());
          }
        }
        ptr = tmp->parent;
        return res;
      }
    }
    /**
     * TODO --iter
     */
    const_iterator &operator--() {
      Node *tmp = ptr;
      if (tmp == nullptr) {
        tmp = which->root;
        if (tmp == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return *this;
      } // end.
      if (tmp->left_son != nullptr) {
        tmp = tmp->left_son;
        while (tmp->right_son != nullptr) {
          tmp = tmp->right_son;
        }
        ptr = tmp;
        return *this;
      } else {
        if (tmp->parent == nullptr) {
          throw(invalid_iterator());
        }
        while (tmp->parent->left_son == tmp) {
          auto last = tmp;
          tmp = tmp->parent;
          if (tmp->parent == nullptr && tmp->left_son == last) {
            throw(invalid_iterator());
          }
        }
        ptr = tmp->parent;
        return *this;
      }
    }
    const_iterator &operator=(const const_iterator &other) {
      if (this->ptr == other.ptr && this->which == other.which) {
        return *this;
      } else {
        this->ptr = other.ptr;
        this->which = other.which;
        return *this;
      }
    }
    bool operator==(const iterator &rhs) const {
      return rhs.ptr == ptr && rhs.which == which;
    }
    bool operator==(const const_iterator &rhs) const {
      return rhs.ptr == ptr && rhs.which == which;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return rhs.ptr != ptr || rhs.which != which;
    }
    bool operator!=(const const_iterator &rhs) const {
      return rhs.ptr != ptr || rhs.which != which;
    }
  };
  /**
   * TODO two constructors
   */
  map() = default;
  map(const map &other) {
    total = other.total;
    if (other.root == nullptr) {
      root = nullptr;
    } else {
      root = new Node();
      root->Copy(other.root, nullptr);
    }
  }
  /**
   * TODO assignment operator
   */
  map &operator=(const map &other) {
    if (other.root == root) {
      return *this;
    }
    clear();
    if (other.root == nullptr) {
      root = nullptr;
      total = 0;
      return *this;
    }
    root = new Node();
    root->Copy(other.root, nullptr);
    total = other.total;
    return *this;
  }
  /**
   * TODO Destructors
   */
  ~map() { clear(); }
  /**
   * TODO
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent
   * to key. If no such element exists, an exception of type
   * `index_out_of_bound'
   */
  T &at(const Key &key) {
    if (root == nullptr) {
      throw(index_out_of_bound());
    }
    Node *tmp;
    tmp = root->FindNode(key);
    if (tmp == nullptr) {
      throw(index_out_of_bound());
    }
    return tmp->data->second;
  }
  const T &at(const Key &key) const {
    if (root == nullptr) {
      throw(index_out_of_bound());
    }
    Node *tmp;
    tmp = root->FindNode(key);
    if (tmp == nullptr) {
      throw(index_out_of_bound());
    }
    return tmp->data->second;
  }
  /**
   * TODO
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    if (root == nullptr) {
      root = new Node(key, T(), nullptr);
      total++;
      return root->data->second;
    }
    bool res = true;
    auto tmp = root->InsertNode(key, T(), res);
    if (res) {
      total++;
    }
    return tmp->data->second;
  }
  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    if (root == nullptr) {
      throw(index_out_of_bound());
    }
    Node *tmp;
    tmp = root->FindNode(key);
    if (tmp == nullptr) {
      throw(index_out_of_bound());
    }
    return (tmp->data->second);
  }
  /**
   * return a iterator to the beginning
   */
  iterator begin() {
    iterator x;
    x.which = const_cast<map *>(this);
    if (root == nullptr) {
      x.ptr = nullptr;
      return x;
    }
    auto tmp = root;
    while (tmp->left_son != nullptr) {
      tmp = tmp->left_son;
    }
    x.ptr = tmp;
    return x;
  }
  const_iterator cbegin() const {
    const_iterator x;
    x.which = const_cast<map *>(this);
    if (root == nullptr) {
      x.ptr = nullptr;
      return x;
    }
    auto tmp = root;
    while (tmp->left_son != nullptr) {
      tmp = tmp->left_son;
    }
    x.ptr = tmp;
    return x;
  }
  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    iterator x(nullptr, const_cast<map *>(this));
    return x;
  }
  const_iterator cend() const {
    const_iterator x(nullptr, const_cast<map *>(this));
    return x;
  }
  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const { return total == 0; }
  /**
   * returns the number of elements.
   */
  size_t size() const { return total; }
  /**
   * clears the contents
   */
  void clear() {
    if (root == nullptr) {
      return;
    }
    root->Destroy();
    total = 0;
    delete root;
    root = nullptr;
    return;
  }
  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the
   * insertion), the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    bool flag = false;
    Node *ans = nullptr;
    if (root == nullptr) {
      root = new Node(value.first, value.second, nullptr);
      iterator x(root, this);
      root->parent = nullptr;
      total++;
      return pair<iterator, bool>(x, true);
    }
    bool success = true;
    ans = root->InsertNode(value.first, value.second, success);
    if (success) {
      flag = true;
      total++;
    }
    iterator x;
    x.ptr = ans;
    x.which = this;
    if (root != nullptr) {
      root->parent = nullptr;
      root = root->Skew();
      root = root->Split();
    }
    return pair<iterator, bool>(x, flag);
  }
  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an
   * element out of this)
   */
  void erase(iterator pos) {
    if (pos.ptr == nullptr || pos.which != this) {
      throw(sjtu::invalid_iterator());
    }
    RemoveNode(pos.ptr->data->first, root);
    total--;
    if (root != nullptr) {
      root->parent = nullptr;
      root = root->Skew();
      root = root->Split();
    }
    return;
  }
  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    if (root == nullptr) {
      return 0;
    }
    Node *tmp = nullptr;
    tmp = root->FindNode(key);
    if (tmp == nullptr) {
      return 0;
    }
    return 1;
  }
  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is
   * returned.
   */
  iterator find(const Key &key) {
    if (root == nullptr) {
      iterator x;
      x.which = const_cast<map<Key, T, Compare> *>(this);
      x.ptr = nullptr;
      return x;
    }
    auto tmp = root->FindNode(key);
    iterator x;
    x.which = const_cast<map<Key, T, Compare> *>(this);
    x.ptr = tmp;
    return x;
  }
  const_iterator find(const Key &key) const {
    if (root == nullptr) {
      const_iterator x(cend());
      x.which = const_cast<map<Key, T, Compare> *>(this);
      x.ptr = nullptr;
      return x;
    }
    auto tmp = root->FindNode(key);
    const_iterator x(cend());
    x.which = const_cast<map<Key, T, Compare> *>(this);
    x.ptr = tmp;
    return x;
  }
};
} // namespace sjtu

namespace sjtu {
template <typename T, typename = void> struct is_std_iterator {
  static constexpr bool value = 0;
};
template <typename T>
struct is_std_iterator<
    T, std::void_t<typename std::iterator_traits<T>::difference_type>> {
  static constexpr bool value = 1;
};
template <typename T, typename = void> struct is_my_iterator {
  static constexpr int value = 0;
};
template <typename T>
struct is_my_iterator<T, std::void_t<typename T::my_type>> {
  static constexpr bool value = 1;
};
} // namespace sjtu
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
  iterator erase(const int &ind) {
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
          int cachesize = 500>
class BPT {
private:
  struct MyData {
    unsigned long long hash1 = 0;
    unsigned long long hash2 = 0;
    Value value;
    int son = 0;
    bool operator>(const MyData &other) const {
      if (hash1 != other.hash1) {
        return hash1 > other.hash1;
      }
      if (hash2 != other.hash2) {
        return hash2 > other.hash2;
      }
      return value > other.value;
    }
    bool operator<(const MyData &other) const {
      if (hash1 != other.hash1) {
        return hash1 < other.hash1;
      }
      if (hash2 != other.hash2) {
        return hash2 < other.hash2;
      }
      return value < other.value;
    }
    bool operator==(const MyData &other) const {
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
    int BiggerorEqual(const int &l, const int &r, const MyData &to_find) const {
      if ((r - l) <= 2) {
        if (!(to_find > datas[l])) {
          return l;
        } else {
          if(!(to_find > datas[l + 1]) && ((l + 1) < r)) {
            return l + 1;
          }
          return now_size;
        }
      }
      int mid = (l + r) >> 1;
      if (datas[mid] < to_find) {
        return BiggerorEqual(mid + 1, r, to_find);
      } else {
        return BiggerorEqual(l, mid + 1, to_find);
      }
    }
    int Bigger(const int &l, const int &r, const MyData &to_find) const {
      if ((r - l) <= 2) {
        if ((datas[l] > to_find) || (datas[l] == to_find)) {
          return l;
        } else {
          if (((datas[l + 1] > to_find) || (datas[l + 1] == to_find)) &&
              ((l + 1) < r)) {
            return l + 1;
          }
        }
        return now_size;
      }
      int mid = (l + r) >> 1;
      if (datas[mid] > to_find) {
        return Bigger(l, mid + 1, to_find);
      } else {
        return Bigger(mid + 1, r, to_find);
      }
    }
    int Equal(const int &l, const int &r, const MyData &to_find) const {
      if ((r - l) <= 1) {
        if (!(to_find == datas[l])) {
          return -1;
        }
        return l;
      }
      int mid = (l + r) >> 1;
      if (datas[mid] > to_find) {
        return Equal(l, mid, to_find);
      } else {
        return Equal(mid, r, to_find);
      }
    }
  } nothing;
  sjtu::vector<int> recycle;
  int B_total = 0;
  int B_root = 0;
  int B_current = 0;
  MemoryRiver<Node, 3> mydatabase;
  MemoryRiver<int, 1> myrecycle;
  sjtu::list<Node> mycache;
  sjtu::map<int, typename sjtu::list<Node>::iterator> mycache_map;
  void ReadwithCache(Node &res, int pos) {
    if (mycache_map.count(pos)) {
      res = *(mycache_map[pos]);
      mycache.erase(mycache_map[pos]);
      mycache.push_front(res);
      mycache_map[pos] = mycache.begin();
      return;
    }
    mydatabase.read(res, pos);
    mycache.push_front(res);
    mycache_map[pos] = mycache.begin();
    if (mycache.size() > cachesize) {
      Node the_back = mycache.back();
      mycache_map.erase(mycache_map.find(the_back.pos));
      mydatabase.write(the_back, the_back.pos);
      mycache.pop_back();
    }
    // Node res;
    // mydatabase.read(res, pos);
    return;
  }
  void WritewithCache(Node to_write, int nothing) {
    if (mycache_map.count(to_write.pos)) {
      mycache.erase(mycache_map[to_write.pos]);
      mycache.push_front(to_write);
      mycache_map[to_write.pos] = mycache.begin();
      return;
    }
    mycache.push_front(to_write);
    mycache_map[to_write.pos] = mycache.begin();
    if (mycache.size() > cachesize) {
      Node the_back = mycache.back();
      mycache_map.erase(mycache_map.find(the_back.pos));
      mydatabase.write(the_back, the_back.pos);
      mycache.pop_back();
    }
    // mydatabase.write(to_write, to_write.pos);
    return;
  }
  void NodeInsert(const MyData &to_insert, const int &pos, const int &last_node,
                  const Node &last_parent) {
    Node res;
    ReadwithCache(res, pos);
    int find = 0;
    find = res.Bigger(0, res.now_size, to_insert);
    if (res.datas[0].son == 0) {
      if (find != res.now_size) {
        std::memmove(&res.datas[find + 1], &res.datas[find],
                     (res.now_size - find) * sizeof(MyData));
      }
      res.datas[find] = to_insert;
      res.now_size++;
      WritewithCache(res, pos);
    } else {
      if (find == (res.now_size)) {
        find--;
        auto x = to_insert;
        x.son = res.datas[find].son;
        res.datas[find] = x;
        WritewithCache(res, pos);
      }
      NodeInsert(to_insert, res.datas[find].son, res.pos, res);
    }
    if (res.now_size >= size) {
      Split(res.pos, last_node, last_parent);
    }
    return;
  }
  void Split(const int &pos, const int &last_node, Node parent) {
    Node res;
    Node new_node;
    ReadwithCache(res, pos);
    int now_size = res.now_size;
    int half = now_size >> 1;
    std::memmove(&new_node.datas[0], &res.datas[0], half * sizeof(MyData));
    std::memmove(&res.datas[0], &res.datas[half],
                 (now_size - half) * sizeof(MyData));
    res.now_size = (now_size - half);
    new_node.now_size = half;
    new_node.left_sibling = res.left_sibling;
    new_node.right_sibling = res.pos;
    if (recycle.empty()) {
      B_current++;
      new_node.pos = B_current; // 至此，所有新节点已经准备完毕。
    } else {
      new_node.pos = recycle.back();
      recycle.pop_back();
    }
    res.left_sibling = new_node.pos;
    Node res1;
    ReadwithCache(res1, new_node.left_sibling);
    res1.right_sibling = new_node.pos;
    WritewithCache(res1, res1.pos);
    MyData index;
    index = new_node.datas[half - 1];
    index.son = new_node.pos;
    if (last_node) {
      OnlyInsert(last_node, index, parent);
    } else {
      int current;
      if (recycle.empty()) {
        current = B_current;
        current++;
        B_current = current;
      } else {
        current = recycle.back();
        recycle.pop_back();
      }
      Node new_alloc;
      new_alloc.now_size = 2;
      new_alloc.left_sibling = 0;
      new_alloc.right_sibling = 0;
      new_alloc.pos = current;
      new_alloc.datas[1] = res.datas[now_size - half - 1];
      new_alloc.datas[1].son = res.pos;
      new_alloc.datas[0] = index;
      WritewithCache(new_alloc, current);
      B_root = current;
    }
    WritewithCache(res, res.pos);
    WritewithCache(new_node, new_node.pos);
    return;
  }
  void OnlyInsert(const int &pos, const MyData &to_insert, const Node &parent) {
    Node res = parent;
    int find = 0;
    find = res.Bigger(0, res.now_size, to_insert);
    std::memmove(&res.datas[find + 1], &res.datas[find],
                 (res.now_size - find) * sizeof(MyData));
    res.datas[find] = to_insert;
    res.now_size++;
    WritewithCache(res, pos);
    return;
  }
  bool NodeErase(const int &pos, const int &last_pos, MyData to_delete,
                 const int &where, const int &how_many) {
    // pos表示当前节点号、父亲节点号、待删除内容，这个节点是父亲节点的多少号元素、父亲节点有多少个元素。
    Node res;
    ReadwithCache(res, pos);
    int found = 0;
    auto last_one = res.datas[res.now_size - 1];
    int i = res.BiggerorEqual(0, res.now_size, to_delete);

    if (res.datas[i].son == 0) {
      if (res.datas[i] != to_delete) {
        return false; // didn't find~
      } else {
        if (res.now_size == 1) { // 说明删空了。
          recycle.push_back(res.pos);
          if (res.left_sibling != 0) {
            Node left;
            ReadwithCache(left, res.left_sibling);
            left.right_sibling = res.right_sibling;
            WritewithCache(left, res.left_sibling);
          }
          if (res.right_sibling != 0) {
            Node right;
            ReadwithCache(right, res.right_sibling);
            right.left_sibling = res.left_sibling;
            WritewithCache(right, res.right_sibling);
          }
          auto x = res.datas[0];
          if (last_pos) {
            Node parent;
            ReadwithCache(parent, last_pos);
            int i = parent.Equal(0, parent.now_size, x);
            if (i >= 0) {
              if (i != (parent.now_size - 1)) {
                std::memmove(&parent.datas[i], &parent.datas[i + 1],
                             (parent.now_size - i - 1) * sizeof(MyData));
              }
              parent.now_size--;
              WritewithCache(parent, last_pos);
            }
          }
          res.now_size--;
          WritewithCache(res, pos);
          return true;
        }
        if (i != (res.now_size - 1)) {
          std::memmove(&res.datas[i], &res.datas[i + 1],
                       (res.now_size - i - 1) * sizeof(MyData));
        }
        res.now_size--;
        if (last_pos == 0) {
          WritewithCache(res, pos);
          return true;
        }
        if (res.now_size < ((size >> 2) - 1)) {
          if ((where == 0) &&
              (how_many == 1)) { // 说明这个节点没有办法进行调整。
            if (i == res.now_size) {
              Node parent;
              ReadwithCache(parent, last_pos);
              auto to_change = res.datas[res.now_size];
              auto to_update = res.datas[res.now_size - 1];
              int i = parent.Equal(0, parent.now_size, to_change);
              if (i >= 0) {
                to_update.son = parent.datas[i].son;
                parent.datas[i] = to_update;
                WritewithCache(parent, last_pos);
                WritewithCache(res, pos);
                return true;
              }
            }
            WritewithCache(res, pos);
            return true;
          }
          if (where != (how_many - 1)) {
            int right;
            right = res.right_sibling;
            Node right_s;
            ReadwithCache(right_s, res.right_sibling); // 读入右儿子。
            if (right_s.now_size >= (size >> 1)) {
              auto to_update = res.datas[res.now_size];
              res.datas[res.now_size] = right_s.datas[0];
              res.now_size++;
              WritewithCache(res, pos);
              std::memmove(&right_s.datas[0], &right_s.datas[1],
                           (right_s.now_size - 1) * sizeof(MyData));
              right_s.now_size--;
              WritewithCache(right_s, right_s.pos);
              Node parent;
              ReadwithCache(parent, last_pos);
              int i = parent.Equal(0, parent.now_size, to_update);
              if (i >= 0) {
                auto to_change = res.datas[res.now_size - 1];
                to_change.son = parent.datas[i].son;
                parent.datas[i] = to_change;
                WritewithCache(parent, last_pos);
                return true; // 借块操作完成。右兄弟节点、本身、父节点均得到更新。
              }
            } else { // 说明旁边的节点数目数目已经足够少。
              recycle.push_back(right);
              auto to_change = res.datas[res.now_size];
              std::memmove(&res.datas[res.now_size], &right_s.datas[0],
                           right_s.now_size * sizeof(MyData));
              res.now_size += right_s.now_size;
              Node parent;
              ReadwithCache(parent, last_pos);
              int i = parent.Equal(0, parent.now_size, to_change);
              if (parent.datas[i] == to_change) {
                if (i != (parent.now_size - 1)) {
                  std::memmove(&parent.datas[i], &parent.datas[i + 1],
                               (parent.now_size - i - 1) * sizeof(MyData));
                }
                parent.now_size--;
              }
              auto to_update = res.datas[res.now_size - 1];
              i = parent.Equal(0, parent.now_size, to_update);
              if (i >= 0) {
                parent.datas[i].son = pos;
              }
              WritewithCache(parent, last_pos);
              res.right_sibling = right_s.right_sibling;
              if (right_s.right_sibling != 0) {
                Node double_right;
                ReadwithCache(double_right, right_s.right_sibling);
                double_right.left_sibling = pos;
                WritewithCache(double_right, right_s.right_sibling);
              }
              WritewithCache(res, pos);
              return true;
            }
          } else {
            if (i == res.now_size) {
              auto to_erase = res.datas[res.now_size];
              auto to_update = res.datas[res.now_size - 1];
              Node parent;
              ReadwithCache(parent, last_pos);
              int i = parent.Equal(0, parent.now_size, to_erase);
              if (i >= 0) {
                to_update.son = parent.datas[i].son;
                parent.datas[i] = to_update;
                WritewithCache(parent, last_pos);
              }
            }
            int left;
            left = res.left_sibling;
            Node left_s;
            ReadwithCache(left_s, res.left_sibling); // 读入右儿子。
            if (left_s.now_size >= (size >> 1)) {
              std::memmove(&res.datas[1], &res.datas[0],
                           res.now_size * sizeof(MyData));
              res.datas[0] = left_s.datas[left_s.now_size - 1];
              auto to_change = left_s.datas[left_s.now_size - 1];
              auto to_update = left_s.datas[left_s.now_size - 2];
              left_s.now_size--;
              res.now_size++;
              WritewithCache(res, pos);
              WritewithCache(left_s, left);
              Node parent;
              ReadwithCache(parent, last_pos);
              int i = parent.Equal(0, parent.now_size, to_change);
              if (i >= 0) {
                to_update.son = parent.datas[i].son;
                parent.datas[i] = to_update;
                WritewithCache(parent, last_pos);
                return true;
              }
            } else { // 说明旁边的节点数目数目已经足够少。
              recycle.push_back(left);
              // std::cout << "CHECK IT" << to_delete.value <<  std::endl;
              std::memmove(&res.datas[left_s.now_size], &res.datas[0],
                           res.now_size * sizeof(MyData));
              std::memmove(&res.datas[0], &left_s.datas[0],
                           left_s.now_size * sizeof(MyData));
              res.now_size += left_s.now_size;
              res.left_sibling = left_s.left_sibling;
              WritewithCache(res, pos);
              if (left_s.left_sibling != 0) {
                Node double_left;
                ReadwithCache(double_left, left_s.left_sibling);
                double_left.right_sibling = pos;
                WritewithCache(double_left, left_s.left_sibling);
              }
              auto to_change = res.datas[left_s.now_size - 1];
              Node parent;
              ReadwithCache(parent, last_pos);
              int i = parent.Equal(0, parent.now_size, to_change);
              if (i >= 0) {
                if (i != (parent.now_size - 1)) {
                  std::memmove(&parent.datas[i], &parent.datas[i + 1],
                               (parent.now_size - i - 1) * sizeof(MyData));
                }
                parent.now_size--;
                WritewithCache(parent, last_pos);
              }
              WritewithCache(res, pos);
              return true;
            }
          }
        } else {
          if (i == res.now_size) {
            auto to_erase = res.datas[res.now_size];
            auto to_update = res.datas[res.now_size - 1];
            Node parent;
            ReadwithCache(parent, last_pos);
            int i = parent.Equal(0, parent.now_size, to_erase);
            if (i >= 0) {
              to_update.son = parent.datas[i].son;
              parent.datas[i] = to_update;
              WritewithCache(parent, last_pos);
            }
          }
          WritewithCache(res, pos);
          return true;
        }
      }
    } else {
      bool ans = NodeErase(res.datas[i].son, pos, to_delete, i, res.now_size);
      if (ans == false) {
        return false;
      }
      if (last_pos == 0) {
        return true;
      }
      ReadwithCache(res, pos);
      if (res.now_size == 0) { // 说明删空了。
        recycle.push_back(res.pos);
        if (res.left_sibling != 0) {
          Node left;
          ReadwithCache(left, res.left_sibling);
          left.right_sibling = res.right_sibling;
          WritewithCache(left, res.left_sibling);
        }
        if (res.right_sibling != 0) {
          Node right;
          ReadwithCache(right, res.right_sibling);
          right.left_sibling = res.left_sibling;
          WritewithCache(right, res.right_sibling);
        }
        auto x = res.datas[0];
        if (last_pos) {
          Node parent;
          ReadwithCache(parent, last_pos);
          int i = parent.Equal(0, parent.now_size, x);
          if (i >= 0) {
            if (i != (parent.now_size - 1)) {
              std::memmove(&parent.datas[i], &parent.datas[i + 1],
                           (parent.now_size - i - 1) * sizeof(MyData));
            }
            parent.now_size--;
            WritewithCache(parent, last_pos);
          }
        }
        WritewithCache(res, pos);
        return true;
      }
      if (res.datas[res.now_size - 1] != last_one) { // 应当向上修改。
        Node parent;
        ReadwithCache(parent, last_pos);
        int i = parent.Equal(0, parent.now_size, last_one);
        if (i >= 0) {
          auto to_update = res.datas[res.now_size - 1];
          to_update.son = parent.datas[i].son;
          parent.datas[i] = to_update;
          WritewithCache(parent, last_pos);
        }
      }
      if (res.now_size < ((size >> 1) - 1)) {
        if ((where == 0) && (how_many == 1)) { // 说明这个节点没有办法进行调整。
          if (i == res.now_size) {
            Node parent;
            ReadwithCache(parent, last_pos);
            auto to_change = res.datas[res.now_size];
            auto to_update = res.datas[res.now_size - 1];
            int i = parent.Equal(0, parent.now_size, to_change);
            if (i >= 0) {
              to_update.son = parent.datas[i].son;
              parent.datas[i] = to_update;
              WritewithCache(parent, last_pos);
              WritewithCache(res, pos);
              return true;
            }
          }
          WritewithCache(res, pos);
          return true;
        }
        if (where != (how_many - 1)) {
          int right;
          right = res.right_sibling;
          Node right_s;
          ReadwithCache(right_s, res.right_sibling); // 读入右儿子。
          if (right_s.now_size >= (size >> 1)) {
            auto to_update = res.datas[res.now_size];
            res.datas[res.now_size] = right_s.datas[0];
            res.now_size++;
            WritewithCache(res, pos);
            std::memmove(&right_s.datas[0], &right_s.datas[1],
                         (right_s.now_size - 1) * sizeof(MyData));
            right_s.now_size--;
            WritewithCache(right_s, right_s.pos);
            Node parent;
            ReadwithCache(parent, last_pos);
            int i = parent.Equal(0, parent.now_size, to_update);
            if (i >= 0) {
              auto to_change = res.datas[res.now_size - 1];
              to_change.son = parent.datas[i].son;
              parent.datas[i] = to_change;
              WritewithCache(parent, last_pos);
              return true; // 借块操作完成。右兄弟节点、本身、父节点均得到更新。
            }
          } else { // 说明旁边的节点数目数目已经足够少。
            recycle.push_back(right);
            auto to_change = res.datas[res.now_size];
            std::memmove(&res.datas[res.now_size], &right_s.datas[0],
                         right_s.now_size * sizeof(MyData));
            res.now_size += right_s.now_size;
            Node parent;
            ReadwithCache(parent, last_pos);
            int i = parent.Equal(0, parent.now_size, to_change);
            if (i >= 0) {
              if (i != (parent.now_size - 1)) {
                std::memmove(&parent.datas[i], &parent.datas[i + 1],
                             (parent.now_size - i - 1) * sizeof(MyData));
              }
              parent.now_size--;
            }
            auto to_update = res.datas[res.now_size - 1];
            i = parent.Equal(0, parent.now_size, to_update);
            if (i >= 0) {
              parent.datas[i].son = pos;
            }
            WritewithCache(parent, last_pos);
            res.right_sibling = right_s.right_sibling;
            if (right_s.right_sibling != 0) {
              Node double_right;
              ReadwithCache(double_right, right_s.right_sibling);
              double_right.left_sibling = pos;
              WritewithCache(double_right, right_s.right_sibling);
            }
            WritewithCache(res, pos);
            return true;
          }
        } else {
          if (i == res.now_size) {
            auto to_erase = res.datas[res.now_size];
            auto to_update = res.datas[res.now_size - 1];
            Node parent;
            ReadwithCache(parent, last_pos);
            int i = parent.Equal(0, parent.now_size, to_erase);
            if (i >= 0) {
              to_update.son = parent.datas[i].son;
              parent.datas[i] = to_update;
              WritewithCache(parent, last_pos);
            }
          }
          int left;
          left = res.left_sibling;
          Node left_s;
          ReadwithCache(left_s, res.left_sibling); // 读入右儿子。
          if (left_s.now_size >= (size >> 1)) {
            std::memmove(&res.datas[1], &res.datas[0],
                         res.now_size * sizeof(MyData));
            res.datas[0] = left_s.datas[left_s.now_size - 1];
            auto to_change = left_s.datas[left_s.now_size - 1];
            auto to_update = left_s.datas[left_s.now_size - 2];
            left_s.now_size--;
            res.now_size++;
            WritewithCache(res, pos);
            WritewithCache(left_s, left);
            Node parent;
            ReadwithCache(parent, last_pos);
            int i = parent.Equal(0, parent.now_size, to_change);
            if (i >= 0) {
              to_update.son = parent.datas[i].son;
              parent.datas[i] = to_update;
              WritewithCache(parent, last_pos);
              return true;
            }
          } else { // 说明旁边的节点数目数目已经足够少。
            std::memmove(&res.datas[left_s.now_size], &res.datas[0],
                         res.now_size * sizeof(MyData));
            std::memmove(&res.datas[0], &left_s.datas[0],
                         left_s.now_size * sizeof(MyData));
            res.now_size += left_s.now_size;
            res.left_sibling = left_s.left_sibling;
            WritewithCache(res, pos);
            if (left_s.left_sibling != 0) {
              Node double_left;
              ReadwithCache(double_left, left_s.left_sibling);
              double_left.right_sibling = pos;
              WritewithCache(double_left, left_s.left_sibling);
            }
            auto to_change = res.datas[left_s.now_size - 1];
            Node parent;
            ReadwithCache(parent, last_pos);
            int i = parent.Equal(0, parent.now_size, to_change);
            if (i >= 0) {
              if (i != (parent.now_size - 1)) {
                std::memmove(&parent.datas[i], &parent.datas[i + 1],
                             (parent.now_size - i - 1) * sizeof(MyData));
              }
              parent.now_size--;
              WritewithCache(parent, last_pos);
            }
            recycle.push_back(left);
            WritewithCache(res, pos);
            return true;
          }
        }
      } else {
        if (i == res.now_size) {
          auto to_erase = res.datas[res.now_size];
          auto to_update = res.datas[res.now_size - 1];
          Node parent;
          ReadwithCache(parent, last_pos);
          int i = parent.Equal(0, parent.now_size, to_erase);
          if (i >= 0) {
            to_update.son = parent.datas[i].son;
            parent.datas[i] = to_update;
            WritewithCache(parent, last_pos);
          }
        }
        WritewithCache(res, pos);
        return true;
      }
    }
    return false;
  }

public:
  BPT() = delete;
  BPT(std::string name) {
    mydatabase.ChangeName(name);
    mydatabase.get_info(B_total, 1);
    mydatabase.get_info(B_root, 2);
    mydatabase.get_info(B_current, 3);
    std::string recyle_name = name + "_recycle";
    myrecycle.ChangeName(recyle_name);
    int recycle_total;
    myrecycle.get_info(recycle_total, 1);
    for (int i = 1; i <= recycle_total; i++) {
      int res;
      myrecycle.read(res, i);
      recycle.push_back(res);
    }
  }
  ~BPT() {
    mydatabase.write_info(B_total, 1);
    mydatabase.write_info(B_root, 2);
    mydatabase.write_info(B_current, 3);
    myrecycle.write_info(recycle.size(), 1);
    for (int i = 0; i < recycle.size(); i++) {
      myrecycle.write(recycle[i], i + 1);
    }
    for (auto it = mycache.begin(); it != mycache.end(); it++) {
      mydatabase.write(*it, it->pos);
    }
    return;
  }
  void Insert(const unsigned long long &hash1, unsigned long long hash2,
              const int &value) {
    int total = B_total;
    if (total == 0) {
      Node res1;
      recycle.clear();
      res1.datas[0].hash1 = hash1;
      res1.datas[0].hash2 = hash2;
      res1.datas[0].value = value;
      res1.now_size = 1;
      res1.pos = 1;
      WritewithCache(res1, 1);
      B_root = 1;
      B_total = 1;
      B_current = 1;
    } else {
      int root;
      root = B_root;
      MyData res;
      res.hash1 = hash1;
      res.hash2 = hash2;
      res.value = value;
      NodeInsert(res, root, 0, nothing);
      total++;
      B_total = total;
    }
    return;
  }
  void find(const unsigned long long &hash_1,
            const unsigned long long &hash_2) {
    if (B_total == 0) {
      std::cout << "null" << '\n';
      return;
    }
    Node res;
    MyData to_find;
    to_find.hash1 = hash_1;
    to_find.hash2 = hash_2;
    to_find.value = minus_max;
    ReadwithCache(res, B_root);
    while (res.datas[0].son != 0) {
      for (int i = 0; i < res.now_size; i++) {
        if (to_find < res.datas[i]) {
          ReadwithCache(res, res.datas[i].son);
          break;
        }
        if (i == (res.now_size - 1)) {
          std::cout << "null" << '\n';
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
      return;
    }
    while ((hash_1 == res.datas[found].hash1) &&
           (hash_2 == res.datas[found].hash2)) {
      std::cout << res.datas[found].value << ' ';
      found++;
      if (found == res.now_size) {
        if (res.right_sibling == 0) {
          std::cout << '\n';
          return;
        }
        ReadwithCache(res, res.right_sibling);
        found = 0;
      }
    }
    std::cout << '\n';
    return;
  }

  void Erase(const unsigned long long &hash_1, const unsigned long long &hash_2,
             const int &value) {
    MyData to_delete;
    to_delete.hash1 = hash_1;
    to_delete.hash2 = hash_2;
    to_delete.value = value;
    if (NodeErase(B_root, 0, to_delete, 0, 0) != false) {
      B_total--;
      Node to_check;
      ReadwithCache(to_check, B_root);
      if (to_check.now_size == 1 && (B_total != 1)) {
        recycle.push_back(B_root);
        B_root = to_check.datas[0].son;
      }
    }
    return;
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  // auto start = std::chrono::high_resolution_clock::now();
  // freopen("t4.txt", "r", stdin);
  // freopen("single.txt", "w", stdout);
  BPT<int> test("database");
  int n;
  std::cin >> n;
  std::string op;
  for (int i = 0; i < n; i++) {
    std::cin >> op;
    if (op == "insert") {
      std::string index;
      int value;
      std::cin >> index;
      std::cin >> value;
      unsigned long long hash1, hash2;
      hash1 = MyHash(index, exp1);
      hash2 = MyHash(index, exp2);
      test.Insert(hash1, hash2, value);
      continue;
    }
    if (op == "find") {
      std::string index;
      std::cin >> index;
      unsigned long long hash1, hash2;
      hash1 = MyHash(index, exp1);
      hash2 = MyHash(index, exp2);
      test.find(hash1, hash2);
      continue;
    }
    if (op == "delete") {
      std::string index;
      int value;
      std::cin >> index;
      std::cin >> value;
      unsigned long long hash1, hash2;
      hash1 = MyHash(index, exp1);
      hash2 = MyHash(index, exp2);
      test.Erase(hash1, hash2, value);
      continue;
    }
  }
  // auto end = std::chrono::high_resolution_clock::now();
  // std::chrono::duration<double> diff = end - start;
  // std::cout << "Time to execute: " << diff.count() << " s\n";
  return 0;
}
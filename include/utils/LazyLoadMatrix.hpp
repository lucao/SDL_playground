#ifndef LAZY_LOAD_MATRIX_H
#define LAZY_LOAD_MATRIX_H

#include <future>
#include <unordered_map>

class ElementNotFountException : std::exception {
 public:
  char *what() {
    return const_cast<char *>(
        "Game API Error, Element not found in LazyLoadMatrix class");
  }
};

template <typename Key, typename Value, typename Hash, typename Equal>
class LazyLoadMatrix {
 private:
  std::unordered_map<Key, std::future<Value>, Hash, Equal> loadMatrix;
  std::unordered_map<Key, Value, Hash, Equal> valueMatrix;

 public:
  Value getElement(Key key);
  bool contains(Key key);
  bool addElement(Key key, std::function<Value(Key)> lambda);
  Value operator[](Key key) { return this->getElement(key); }
};

template <typename Key, typename Value, typename Hash, typename Equal>
Value LazyLoadMatrix<Key, Value, Hash, Equal>::getElement(Key key) {
  auto it = this->valueMatrix.find(key);
  if (it != this->valueMatrix.end()) {
    return it->second;
  }
  auto loadIterator = this->loadMatrix.find(key);
  if (loadIterator != this->loadMatrix.end()) {
    if (loadIterator->second.valid()) {
      Value value = loadIterator->second.get();
      this->valueMatrix.emplace(key, value);
      return value;
    }
  }

  throw ElementNotFountException();
}

template <typename Key, typename Value, typename Hash, typename Equal>
bool LazyLoadMatrix<Key, Value, Hash, Equal>::contains(Key key) {
  return (this->valueMatrix.find(key) != this->valueMatrix.end()) ||
         (this->loadMatrix.find(key) != this->loadMatrix.end());
}

template <typename Key, typename Value, typename Hash, typename Equal>
bool LazyLoadMatrix<Key, Value, Hash, Equal>::addElement(
    Key key, std::function<Value(Key)> lambda) {
  auto emplacedElement = this->loadMatrix.try_emplace(
      key, std::async(std::launch::async, lambda, key));
  return emplacedElement.second;
}

#endif
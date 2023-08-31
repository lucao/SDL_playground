#ifndef CUSTOM_UTILS_H
#define CUSTOM_UTILS_H

#include <future>
#include <unordered_map>

class ElementNotFountException : std::exception {
 public:
  char *what() {
    return const_cast<char *>(
        "Game API Error, Element not found in LazyLoadMatrix class");
  }
};

template <typename K, typename V, typename H, typename E>
class LazyLoadMatrix {
 private:
  std::unordered_map<K, std::future<V>, H, E> loadMatrix;
  std::unordered_map<K, V, H, E> valueMatrix;

 public:
  V getElement(K key) {
    std::unordered_map<K, V, H, E>::iterator iterator =
        this->valueMatrix.find(key);
    if (iterator != this->valueMatrix.end()) {
      return iterator->second;
    }
    std::unordered_map<K, std::future<V>, H, E>::iterator loadIterator =
        this->loadMatrix.find(key);
    if (loadIterator != this->loadMatrix.end()) {
      if (loadIterator->second.valid()) {
        V v = loadIterator->second.get();
        this->valueMatrix.emplace(key, v);
        return v;
      }
    }

    throw new ElementNotFountException();
  }
  bool LazyLoadMatrix<K, V, H, E>::addElement(K key,
                                              std::function<V(K)> lambda) {
    std::pair<std::unordered_map<K, std::future<V>, H, E>::iterator, bool>
        emplacedElement = this->loadMatrix.try_emplace(
            key, std::async(std::launch::async, lambda, key));
    return emplacedElement.second;
  }
};

#endif
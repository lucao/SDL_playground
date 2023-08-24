#ifndef CUSTOM_UTILS_H
#define CUSTOM_UTILS_H

#include <unordered_map>
#include <future>

template<typename K, class V, typename H, typename E>
class LazyLoadMatrix {
    private:
        std::unordered_map<K, std::future<V>, H, E> loadMatrix;
        std::unordered_map<K, V, H, E> valueMatrix;
    public:
        V getElement(K key);
        template<typename L>
        std::pair<std::unordered_map<K, std::future<V>, H, E>::iterator, bool> addElement(K key, const L lambda);
};

class ElementNotFountException: std::exception {

 public:
  char *what() {
    return const_cast<char *>("Game API Error, Element not found in LazyLoadMatrix class");
  }
};

#endif
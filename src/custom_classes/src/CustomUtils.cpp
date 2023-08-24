#include <CustomUtils.hpp>

template <typename K, class V, typename H, typename E>
V LazyLoadMatrix<K, V, H, E>::getElement(K key) {
  std::unordered_map<K, V, H, E>::iterator iterator =
      this->valueMatrix.find(key);
  if (iterator != this->valueMatrix.end()) {
    return iterator->second;
  }
  std::unordered_map<K, std::future<V>, H, E>::iterator loadIterator =
      this->loadMatrix.find(key);
  if (loadIterator != this->loadMatrix.end()) {
    if (loadIterator->second.valid()) {
      V* v = loadIterator->second.get();
      this->valueMatrix.emplace(key, v);
      return v;
    }
  }

  throw new ElementNotFountException();
}

template <typename K, class V, typename H, typename E>
template <typename L>
std::pair<std::unordered_map<K, std::future<V>, H, E>::iterator, bool>
LazyLoadMatrix<K, V, H, E>::addElement(K key, const L lambda) {
  return this->loadMatrix.try_emplace(key,
                                      std::async(std::launch::async, lamda));
}
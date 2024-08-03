#ifndef CYCLIC_ITERATOR_H
#define CYCLIC_ITERATOR_H

template <typename Iterator>
class CyclicIterator {
 private:
  Iterator begin_;
  Iterator end_;
  Iterator current_;

 public:
  // Default constructor
  CyclicIterator() : begin_(), end_(), current_() {}
  CyclicIterator(Iterator begin, Iterator end)
      : begin_(begin), end_(end), current_(begin) {}

 // Dereference operator
  typename Iterator::reference operator*() const { return *current_; }

  // Increment operator
  CyclicIterator& operator++() {
    ++current_;
    if (current_ == end_) {
      current_ = begin_;  // Wrap around to the beginning
    }
    return *this;
  }

  // Equality comparison
  bool operator==(const CyclicIterator& other) const {
    return current_ == other.current_;
  }

  // Inequality comparison
  bool operator!=(const CyclicIterator& other) const {
    return !(*this == other);
  }

  // Method to initialize iterators after default construction
  void set_range(Iterator begin, Iterator end) {
    begin_ = begin;
    end_ = end;
    current_ = begin_;
  }
};

#endif
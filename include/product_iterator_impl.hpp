#ifndef __PRODUCT_ITERATOR_IMPL_HPP__
#define __PRODUCT_ITERATOR_IMPL_HPP__

#include "iterator.hpp"

namespace cartesian_product {

template <class... Containers>
iterator<Containers...>::iterator():
  current_tuple_(nullptr) { }

template <class... Containers>
iterator<Containers...>::iterator(
    iterator const& other):
  current_tuple_(nullptr) {
    begin_ = other.begin_;
    end_ = other.end_;
    current_ = other.current_;
}

template <class... Containers>
iterator<Containers...>::iterator(
    Containers const&... containers):
  current_tuple_(nullptr) {
    copy_iterator<0>(containers...);
}

template <class... Containers>
iterator<Containers...>::~iterator() {
  if (current_tuple_ != nullptr)
    delete current_tuple_;
}

template <class... Containers>
iterator<Containers...> const&
iterator<Containers...>::operator=(iterator const& other) {
  if (current_tuple_ != nullptr) {
    delete current_tuple_;
    current_tuple_ = nullptr;
  }
  begin_ = other.begin_;
  end_ = other.end_;
  current_ = other.current_;

  return *this;
}

template <class... Containers>
iterator<Containers...>
iterator<Containers...>::get_end() const {
  iterator<Containers...> ret(*this);
  ret.current_ = begin_;
  std::get<0>(ret.current_) = std::get<0>(ret.end_);
  return ret;
}

template <class... Containers>
template <size_t I, class T1, class... Types>
void iterator<Containers...>::copy_iterator(T1 const& container,
    Types const&... containers) {
  std::get<I>(current_) = container.cbegin();
  std::get<I>(begin_) = container.cbegin();
  std::get<I>(end_) = container.cend();
  copy_iterator<I+1>(containers...);
}

template <class... Containers>
template <size_t I, class T1>
void iterator<Containers...>::copy_iterator(T1 const& container) {
  std::get<I>(current_) = container.cbegin();
  std::get<I>(begin_) = container.cbegin();
  std::get<I>(end_) = container.cend();
}

template <class... Containers>
void iterator<Containers...>::increment() {
  // Avoids incrementing if we have already reached the end.
  if (std::get<0>(current_) == std::get<0>(end_))
    return;

  advance<sizeof...(Containers)>();

  if (current_tuple_ != nullptr) {
    delete current_tuple_;
    current_tuple_ = nullptr;
  }
}

template <class... Containers>
bool iterator<Containers...>::equal(
    iterator<Containers...> const& other) const {
  return current_ == other.current_;
}

template <class... Containers>
typename iterator<Containers...>::value_type const&
iterator<Containers...>::dereference() const {
  if (current_tuple_ == nullptr)
    current_tuple_ = new value_type(make_value_type(
          typename tuple_sequence_generator<value_type>::type()));
  return *current_tuple_;
}

template <class... Containers>
template <size_t... S>
typename iterator<Containers...>::value_type
iterator<Containers...>::make_value_type(sequence<S...>) const {
  return value_type(*std::get<S>(this->current_)...);
}

template <class... Containers>
template <size_t I>
void iterator<Containers...>::advance(
    typename std::enable_if<(I == 0), int>::type) {
}

template <class... Containers>
template <size_t I>
void iterator<Containers...>::advance(
    typename std::enable_if<(I > 0), int>::type) {
  ++std::get<I-1>(current_);

  if (std::get<I-1>(current_) == std::get<I-1>(end_)) {
    if (I == 1)
      return;

    std::get<I-1>(current_) = std::get<I-1>(begin_);
    advance<I-1>();
  }
}

}

#endif

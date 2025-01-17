// This file defines an useful iterator over the cartesian product of iterable
// containers, that is, provides a single iterator that iterates over all
// combinations of elements given in the arguments.
//
// This allows integration with methods that require iterators and reduces the
// number of loops in the code.
//
// As writing the class type may be hard, a helper function is provided. The
// iterator returned iterates over the containers, left to right, and provides a
// method get_end() to get the end iterator associated with it.
//
// Example:
// vector<int> c1({1,2});
// vector<char> c2({'a','b'});
// auto it = make_iterator(c1, c2);
// auto end = it.get_end();
// for (; it != end; ++it) {
//   // Provides a std::tuple<int,char> with the values from the constructor
//   *it;
//   // Access to a single element of the tuple is optimized and the following
//   // comparison always holds
//   it.get<0>() == std::get<0>(*it);
// }

#ifndef __PRODUCT_ITERATOR_HPP__
#define __PRODUCT_ITERATOR_HPP__

#include <boost/iterator/iterator_facade.hpp>
#include <tuple>
#include <type_traits>

namespace cartesian_product {

template <class... Containers>
class iterator:
  public boost::iterator_facade<
    iterator<Containers...>,
    std::tuple<
      typename std::remove_reference<
        typename Containers::value_type
      >::type const&...
    > const,
    boost::forward_traversal_tag
  > {
    private:
      typedef std::tuple<
        typename std::remove_reference<
          typename Containers::value_type
        >::type const&...
      > value_type;

      typedef std::tuple<
        typename Containers::const_iterator...
      > iterator_type;

      // Helper structs for tuple expanding
      template <size_t...> struct sequence {};
      template <size_t N, size_t... S>
      struct sequence_generator: sequence_generator<N-1, N-1, S...> {};
      template <size_t... S>
      struct sequence_generator<0, S...> {
        typedef sequence<S...> type;
      };
      template <class T>
      struct tuple_sequence_generator {
        typedef typename sequence_generator<std::tuple_size<T>::value>::type
          type;
      };

    public:
      iterator();

      iterator(iterator const& other);

      iterator(Containers const&... containers);

      ~iterator();

      iterator const& operator=(iterator const& other);

      // Gets the end iterator to be compared
      iterator<Containers...> get_end() const;

      // See public get() member bellow private section

    private:
      // Boost stuff due to facade
      friend class boost::iterator_core_access;
      void increment();
      bool equal(iterator<Containers...> const& other) const;
      value_type const& dereference() const;

      // Helper method to dereference. Just get references from each iterator
      // and builds a tuple of constant references.
      template <size_t... S> value_type make_value_type(sequence<S...>) const;

      // Auxiliary methods to constructor to copy iterators to tuple
      template <size_t I, class T1, class... Types>
      void copy_iterator(T1 const& container, Types const&... containers);
      template <size_t I, class T1>
      void copy_iterator(T1 const& container);

      // Auxiliary method to increment() to increment each iterator
      template <size_t I>
      void advance(typename std::enable_if<(I == 0), int>::type = 0);
      template <size_t I>
      void advance(typename std::enable_if<(I > 0), int>::type = 0);

      // Holds iterators to each container used at construction
      iterator_type current_, begin_, end_;

      // Holds the last tuple created by operator*, so that it can be returned
      // in multiple calls if the iterator is the same. This variable if freed
      // in operator++ and the destructor.
      // We must use this so that we can return a const& at operator* to comply
      // with the range-based for
      // for (auto& it : product)
      // If it could be "auto it", then we wouldn't need to keep reallocating
      // this thing. We also can't keep a single copy because each element of
      // the tuple is a const&, so it can't be constructed by itself.
      mutable value_type* current_tuple_;

    public:
      // Enables access to a single value instead of the whole tuple. This
      // method is preffered to the operator* as it doesn't require tuple
      // construction. Behaves like std::get<I>(*iterator).
      template <size_t I>
      auto get() const -> decltype(*std::get<I>(current_)) {
        // This function must come after the private section as it needs
        // `current_` to be defined before it for use in the trailing return
        // type.

        // As only a single value is needed, gathers it from its iterator.
        return *std::get<I>(current_);
      }
};

template <class... Containers>
iterator<Containers...>
make_iterator(Containers const&... containers) {
  return iterator<Containers...>(containers...);
}

}

#include "product_iterator_impl.hpp"

#endif

#pragma once
#ifndef BE_UTIL_CHUNKED_LIST_CONST_ITERATOR_HPP_
#define BE_UTIL_CHUNKED_LIST_CONST_ITERATOR_HPP_

#include <cassert>

namespace be::util {
namespace detail {

template <typename C>
class ChunkedListConstIterator {
   using iterator = ChunkedListConstIterator<C>;
   using node_ptr = typename C::pointer;
   friend typename C;
public:
   using iterator_category = std::random_access_iterator_tag;
   using value_type = typename C::value_type;
   using difference_type = typename C::difference_type;
   using pointer = typename C::const_pointer;
   using reference = typename C::const_reference;

   ChunkedListConstIterator() { }

   reference operator*() const;
   pointer operator->() const;

   iterator& operator++();
   iterator& operator--();
   iterator operator++(int);
   iterator operator--(int);
   iterator& operator+=(difference_type offset);
   iterator& operator-=(difference_type offset);
   iterator operator+(difference_type offset) const;
   iterator operator-(difference_type offset) const;
   difference_type operator-(const iterator& other) const;

   reference operator[](difference_type offset) const;

   bool operator==(const iterator& other) const;
   bool operator!=(const iterator& other) const;
   bool operator<(const iterator& other) const;
   bool operator>(const iterator& other) const;
   bool operator<=(const iterator& other) const;
   bool operator>=(const iterator& other) const;

protected:
   ChunkedListConstIterator(const C* c, difference_type offset);

   difference_type offset_;
   mutable difference_type node_index_;
   mutable node_ptr node_;
   C* container_;
};

template <typename C>
ChunkedListConstIterator<C> operator+(typename ChunkedListConstIterator<C>::difference_type offset, ChunkedListConstIterator<C> iter);

} // be::util::detail
} // be::util

#include "chunked_list_const_iterator.inl"

#endif

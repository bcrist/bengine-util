#pragma once
#ifndef BE_CORE_CHUNKED_LIST_ITERATOR_HPP_
#define BE_CORE_CHUNKED_LIST_ITERATOR_HPP_

#include "chunked_list_const_iterator.hpp"

namespace be {
namespace util {
namespace detail {

template<typename C>
class ChunkedListIterator : public ChunkedListConstIterator<C> {
   using iterator = ChunkedListIterator<C>;
   using const_iterator = ChunkedListConstIterator<C>;
   friend typename C;
public:
   using iterator_category = std::random_access_iterator_tag;
   using value_type = typename C::value_type;
   using difference_type = typename C::difference_type;
   using pointer = typename C::pointer;
   using reference = typename C::reference;

   iterator() { }

   reference operator*() const;
   pointer operator->() const;

   iterator& operator++();
   iterator& operator--();
   iterator operator++(int);
   iterator operator--(int);
   iterator& operator+=(difference_type offset);
   iterator operator+(difference_type offset) const;
   iterator& operator-=(difference_type offset);
   iterator operator-(difference_type offset) const;
   difference_type operator-(const const_iterator& other) const;

   reference operator[](difference_type offset) const;

protected:
   iterator(C* c, difference_type offset);

private:
   explicit iterator(const_iterator& other);
};

template <typename C>
ChunkedListIterator<C> operator+(typename ChunkedListIterator<C>::difference_type offset, ChunkedListIterator<C> iter);

} // be::detail
} // be::util
} // be

#include "chunked_list_iterator.inl"

#endif

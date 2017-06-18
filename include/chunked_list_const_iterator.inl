#if !defined(BE_UTIL_CHUNKED_LIST_CONST_ITERATOR_HPP_) && !defined(DOXYGEN)
#include "chunked_list_const_iterator.hpp"
#elif !defined(BE_UTIL_CHUNKED_LIST_CONST_ITERATOR_INL_)
#define BE_UTIL_CHUNKED_LIST_CONST_ITERATOR_INL_

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListConstIterator<C>::reference ChunkedListConstIterator<C>::operator*() const {
   difference_type node_index = offset_ / C::chunk_size;
   difference_type index = offset_ % C::chunk_size;
   assert(index >= 0);
   
   if (node_index_ != node_index) {
      node_ = container_->get_node_(node_index);
      node_index_ = node_index;
   }
   return node_[index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListConstIterator<C>::pointer ChunkedListConstIterator<C>::operator->() const {
   difference_type node_index = offset_ / C::chunk_size;
   difference_type index = offset_ % C::chunk_size;
   assert(index >= 0);

   if (node_index_ != node_index) {
      node_ = container_->get_node_(node_index);
      node_index_ = node_index;
   }
   return node_ + index;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C>& ChunkedListConstIterator<C>::operator++() {
   ++offset_;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C>& ChunkedListConstIterator<C>::operator--() {
   --offset_;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C> ChunkedListConstIterator<C>::operator++(int) {
   iterator tmp = *this;
   ++*this;
   return tmp;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C> ChunkedListConstIterator<C>::operator--(int) {
   iterator tmp = *this;
   --*this;
   return tmp;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C>& ChunkedListConstIterator<C>::operator+=(difference_type offset) {
   offset_ += offset;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C>& ChunkedListConstIterator<C>::operator-=(difference_type offset) {
   return *this += -offset;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C> ChunkedListConstIterator<C>::operator+(difference_type offset) const {
   iterator tmp = *this;
   return tmp += offset;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C> ChunkedListConstIterator<C>::operator-(difference_type offset) const {
   iterator tmp = *this;
   return tmp -= offset;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListConstIterator<C>::difference_type ChunkedListConstIterator<C>::operator-(const iterator& other) const {
   assert(container_ == other.container_);
   return offset_ - other.offset_;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListConstIterator<C>::reference ChunkedListConstIterator<C>::operator[](difference_type offset) const {
   return *(*this + offset);
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
bool ChunkedListConstIterator<C>::operator==(const iterator& other) const {
   assert(container_ == other.container_);
   return offset_ == other.offset_;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
bool ChunkedListConstIterator<C>::operator!=(const iterator& other) const {
   return !(*this == other);
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
bool ChunkedListConstIterator<C>::operator<(const iterator& other) const {
   assert(container_ == other.container_);
   return offset_ < other.offset_;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
bool ChunkedListConstIterator<C>::operator>(const iterator& other) const {
   return other < *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
bool ChunkedListConstIterator<C>::operator<=(const iterator& other) const {
   return !(other < *this);
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
bool ChunkedListConstIterator<C>::operator>=(const iterator& other) const {
   return !(*this < other);
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C>::ChunkedListConstIterator(const C* c, difference_type offset)
   : offset_(offset),
     node_index_(-1),
     node_(nullptr),
     container_(const_cast<C*>(c))
{ }

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListConstIterator<C> operator+(typename ChunkedListConstIterator<C>::difference_type offset, ChunkedListConstIterator<C> iter) {
   return iter += offset;
}

} // be::detail
} // be::util

#endif

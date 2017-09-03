#if !defined(BE_CORE_CHUNKED_LIST_ITERATOR_HPP_) && !defined(DOXYGEN)
#include "chunked_list_iterator.hpp"
#elif !defined(BE_CORE_CHUNKED_LIST_ITERATOR_INL_)
#define BE_CORE_CHUNKED_LIST_ITERATOR_INL_

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListIterator<C>::reference ChunkedListIterator<C>::operator*() const {
   difference_type node_index = this->offset_ / C::chunk_size;
   difference_type index = this->offset_ % C::chunk_size;
   assert(index >= 0);

   if (this->node_index_ != node_index) {
      this->node_ = this->container_->get_node_(node_index);
      this->node_index_ = node_index;
   }
   return this->node_[index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListIterator<C>::pointer ChunkedListIterator<C>::operator->() const {
   difference_type node_index = this->offset_ / C::chunk_size;
   difference_type index = this->offset_ % C::chunk_size;
   assert(index >= 0);

   if (this->node_index_ != node_index) {
      this->node_ = this->container_->get_node_(node_index);
      this->node_index_ = node_index;
   }
   return this->node_ + index;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C>& ChunkedListIterator<C>::operator++() {
   ++*static_cast<const_iterator*>(this);
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C>& ChunkedListIterator<C>::operator--() {
   --*static_cast<const_iterator*>(this);
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C> ChunkedListIterator<C>::operator++(int) {
   iterator tmp = *this;
   ++*this;
   return tmp;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C> ChunkedListIterator<C>::operator--(int) {
   iterator tmp = *this;
   --*this;
   return tmp;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C>& ChunkedListIterator<C>::operator+=(difference_type offset) {
   *static_cast<const_iterator*>(this) += offset;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C>& ChunkedListIterator<C>::operator-=(difference_type offset) {
   return *this += -offset;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C> ChunkedListIterator<C>::operator+(difference_type offset) const {
   iterator tmp = *this;
   return tmp += offset;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C> ChunkedListIterator<C>::operator-(difference_type offset) const {
   iterator tmp = *this;
   return tmp -= offset;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListIterator<C>::difference_type ChunkedListIterator<C>::operator-(const const_iterator& other) const {
   return *static_cast<const const_iterator*>(this) - other;
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
typename ChunkedListIterator<C>::reference ChunkedListIterator<C>::operator[](difference_type offset) const {
   return *(*this + offset);
}

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C>::ChunkedListIterator(C* c, difference_type offset)
   : const_iterator(c, offset)
{ }

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C>::ChunkedListIterator(const_iterator& other)
   : const_iterator(other)
{ }

///////////////////////////////////////////////////////////////////////////////
template <typename C>
ChunkedListIterator<C> operator+(typename ChunkedListIterator<C>::difference_type offset, ChunkedListIterator<C> iter) {
   return iter += offset;
}

} // be::detail
} // be::util

#endif

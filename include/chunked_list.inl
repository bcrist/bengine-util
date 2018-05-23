#if !defined(BE_UTIL_CHUNKED_LIST_HPP_) && !defined(DOXYGEN)
#include "chunked_list.hpp"
#elif !defined(BE_UTIL_CHUNKED_LIST_INL_)
#define BE_UTIL_CHUNKED_LIST_INL_

namespace be::util {

#pragma region construction/destruction

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList()
   : size_(0)
{ }

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(const allocator_type& alloc)
   : base(alloc),
     size_(0)
{ }

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(size_type count, const value_type& value, const allocator_type& alloc)
   : base(alloc),
     size_(0)
{
   if (count > 1) {
      push_back_n_(count, value);
   } else if (count == 1) {
      push_back(value);
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(size_type count, const allocator_type& alloc)
   : base(alloc),
     size_(0)
{
   if (count > 1) {
      push_back_n_(count);
   } else if (count == 1) {
      emplace_back();
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I, typename>
ChunkedList<T, A, N, M, S>::ChunkedList(I first, I last, const allocator_type& alloc)
   : base(alloc),
     size_(0)
{
   this->insert_back_(first, last, typename std::iterator_traits<I>::iterator_category());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(const container& other)
   : base(other.get_node_alloc_().select_on_container_copy_construction()),
     size_(0)
{
   this->insert_back_(other.begin(), other.end(), std::random_access_iterator_tag());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(const container& other, const allocator_type& alloc)
   : base(alloc),
     size_(0)
{
   this->insert_back_(other.begin(), other.end(), typename std::iterator_traits<I>::iterator_category());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(container&& other)
   : size_(0)
{
   using std::swap;
   this->swap_allocators_(other);
   swap(size_, other.size_);
   swap(this->get_static_metanode_(), other.get_static_metanode_());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(container&& other, const allocator_type& alloc)
   : base(alloc),
     size_(0)
{
   using std::swap;
   swap(size_, other.size_);
   swap(this->get_static_metanode_(), other.get_static_metanode_());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::ChunkedList(std::initializer_list<value_type> il, const allocator_type& alloc)
   : base(alloc),
     size_(0)
{
   using cat = typename std::iterator_traits<typename std::initializer_list<value_type>::iterator>::iterator_category;
   this->insert_back_(il.begin(), il.end(), cat());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>::~ChunkedList() {
   clear();
}

#pragma endregion
#pragma region assignment

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>&
ChunkedList<T, A, N, M, S>::operator=(const container& other) {
   if (&other != this) {
      if (other.get_node_alloc_() != this->get_node_alloc_() && node_alloc::propagate_on_container_copy_assignment::value) {
         clear();
         this->set_allocator_(other.get_node_alloc_());
         this->insert_back_(other.begin(), other.end(), std::random_access_iterator_tag());
      } else {
         this->assign(other.begin(), other.end());
      }
   }
   return *this;
}

#pragma warning(push)
#pragma warning(disable: 4127) // conditional is constant

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>&
ChunkedList<T, A, N, M, S>::operator=(container&& other) {
   using std::swap;
   if (&other != this) {
      if (other.get_node_alloc_() == this->get_node_alloc_()) {
         swap(size_, other.size_);
         swap(this->get_static_metanode_(), other.get_static_metanode_());
      } else if (node_alloc::propagate_on_container_move_assignment::value) {
         this->swap_allocators_(other);
         swap(size_, other.size_);
         swap(this->get_static_metanode_(), other.get_static_metanode_());
      } else {
         // Can't take ownership of other's nodes
         this->move_assign_(other.begin(), other.end());
      }
   }
   return *this;
}

#pragma warning(pop)

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
ChunkedList<T, A, N, M, S>&
ChunkedList<T, A, N, M, S>::operator=(std::initializer_list<value_type> il) {
   using cat = typename std::iterator_traits<typename std::initializer_list<value_type>::iterator>::iterator_category;
   assign_(il.begin(), il.end(), cat());
   return *this;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::assign(size_type count, const value_type& value) {
   if (count == 0) {
      clear();
      return;
   }

   if (size_ > 0) {
      // value might be a reference to a value in this container, so
      // assign it to the first element, then copy that to the rest
      *begin() = value;
      for (iterator i = begin() + 1, e = end(); i != e; ++i)
         (*i).~T();

      size_ = 1;

      push_back_n_(count - 1, *begin());
      cleanup_();
   } else {
      // container is empty, so value couldn't possibly be contained in it!
      for (iterator i = begin(), e = end(); i != e; ++i)
         (*i).~T();

      size_ = 0;
      push_back_n_(count, value);
      cleanup_();
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I, typename>
void ChunkedList<T, A, N, M, S>::assign(I first, I last) {
   assign_(first, last, typename std::iterator_traits<I>::iterator_category());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::assign(std::initializer_list<value_type> il) {
   using cat = typename std::iterator_traits<typename std::initializer_list<value_type>::iterator>::iterator_category;
   assign_(il.begin(), il.end(), cat());
}

#pragma endregion
#pragma region random access

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::reference
ChunkedList<T, A, N, M, S>::at(size_type pos) {
   if (pos >= size_) {
      throw std::out_of_range("ChunkedList index out of range!");
   }

   size_type node_index = pos / chunk_size;
   size_type index = pos % chunk_size;

   pointer node = get_node_(node_index);
   return node[index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reference
ChunkedList<T, A, N, M, S>::at(size_type pos) const {
   if (pos >= size_) {
      throw std::out_of_range("ChunkedList index out of range!");
   }

   size_type node_index = pos / chunk_size;
   size_type index = pos % chunk_size;

   pointer node = get_node_(node_index);
   return node[index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::reference
ChunkedList<T, A, N, M, S>::operator[](size_type pos) {
   assert(size_ > pos);
   size_type node_index = pos / chunk_size;
   size_type index = pos % chunk_size;

   pointer node = get_node_(node_index);
   return node[index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reference
ChunkedList<T, A, N, M, S>::operator[](size_type pos) const {
   assert(size_ > pos);
   size_type node_index = pos / chunk_size;
   size_type index = pos % chunk_size;

   pointer node = get_node_(node_index);
   return node[index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::reference
ChunkedList<T, A, N, M, S>::front() {
   assert(size_ >= 1);
   return *get_node_(0);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reference
ChunkedList<T, A, N, M, S>::front() const {
   assert(size_ >= 1);
   return *get_node_(0);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::reference
ChunkedList<T, A, N, M, S>::back() {
   const size_type pos = size_ - 1;
   assert(pos >= 0);

   return (*this)[pos];
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reference
ChunkedList<T, A, N, M, S>::back() const {
   const size_type pos = size_ - 1;
   assert(pos >= 0);

   return (*this)[pos];
}

#pragma endregion
#pragma region iterators

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::begin() noexcept {
   return iterator(this, difference_type(0));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_iterator
ChunkedList<T, A, N, M, S>::begin() const noexcept {
   return const_iterator(this, difference_type(0));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_iterator
ChunkedList<T, A, N, M, S>::cbegin() const noexcept {
   return begin();
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::end() noexcept {
   return iterator(this, difference_type(size_));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_iterator
ChunkedList<T, A, N, M, S>::end() const noexcept {
   return const_iterator(this, difference_type(size_));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_iterator
ChunkedList<T, A, N, M, S>::cend() const noexcept {
   return end();
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::reverse_iterator
ChunkedList<T, A, N, M, S>::rbegin() noexcept  {
   return reverse_iterator(end());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reverse_iterator
ChunkedList<T, A, N, M, S>::rbegin() const noexcept {
   return const_reverse_iterator(end());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reverse_iterator
ChunkedList<T, A, N, M, S>::crbegin() const noexcept {
   return rbegin();
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::reverse_iterator
ChunkedList<T, A, N, M, S>::rend() noexcept {
   return reverse_iterator(begin());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reverse_iterator
ChunkedList<T, A, N, M, S>::rend() const noexcept {
   return const_reverse_iterator(begin());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::const_reverse_iterator
ChunkedList<T, A, N, M, S>::crend() const noexcept {
   return rend();
}

#pragma endregion
#pragma region size

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool ChunkedList<T, A, N, M, S>::empty() const noexcept {
   return size_ == 0;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::size() const noexcept {
   return size_;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::max_size() const noexcept {
   return std::numeric_limits<size_type>::max();
}

#pragma endregion
#pragma region insertion

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::insert(const_iterator pos, const value_type& value) {
   size_type offset = pos - begin();
   size_type old_size = size_;
   push_back(value);
   std::rotate(iterator(this, difference_type(offset)), iterator(this, difference_type(old_size)), end());
   return iterator(this, difference_type(offset));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::insert(const_iterator pos, value_type&& value) {
   size_type offset = pos - begin();
   size_type old_size = size_;
   push_back(std::forward<value_type>(value));
   std::rotate(iterator(this, difference_type(offset)), iterator(this, difference_type(old_size)), end());
   return iterator(this, difference_type(offset));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::insert(const_iterator pos, size_type count, const value_type& value) {
   if (count > 1) {
      size_type offset = pos - begin();
      size_type old_size = size_;
      push_back_n_(count, value);
      std::rotate(iterator(this, difference_type(offset)), iterator(this, difference_type(old_size)), end());
      return iterator(this, difference_type(offset));
   } else if (count == 1) {
      size_type offset = pos - begin();
      size_type old_size = size_;
      push_back(value);
      std::rotate(iterator(this, difference_type(offset)), iterator(this, difference_type(old_size)), end());
      return iterator(this, difference_type(offset));
   } else {
      return end();
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I, typename>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::insert(const_iterator pos, I first, I last) {
   size_type offset = pos - begin();
   insert_(pos, first, last, std::iterator_traits<I>::iterator_category());
   return iterator(this, difference_type(offset));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::insert(const_iterator pos, std::initializer_list<value_type> il) {
   using cat = typename std::iterator_traits<typename std::initializer_list<value_type>::iterator>::iterator_category;
   size_type offset = pos - begin();
   insert_(pos, il.begin(), il.end(), cat());
   return iterator(this, difference_type(offset));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename... P>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::emplace(const_iterator pos, P&&... args) {
   size_type offset = pos - begin();
   size_type old_size = size_;
   emplace_back(std::forward<P>(args)...);
   std::rotate(iterator(this, difference_type(offset)), iterator(this, difference_type(old_size)), end());
   return iterator(this, difference_type(offset));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::push_back(const value_type& value) {
   size_type index = size_ % chunk_size;
   pointer ptr;
   if (index > 0) {
      size_type node_index = size_ / chunk_size;
      ptr = get_node_(node_index) + index;
   } else {
      // node hasn't been allocated yet
      ptr = get_push_node_();
   }
   new (ptr) value_type(value);
   ++size_;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::push_back(value_type&& value) {
   size_type index = size_ % chunk_size;
   pointer ptr;
   if (index > 0) {
      size_type node_index = size_ / chunk_size;
      ptr = get_node_(node_index) + index;
   } else {
      // node hasn't been allocated yet
      ptr = get_push_node_();
   }
   new (ptr) value_type(std::forward<value_type>(value));
   ++size_;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename... P>
void ChunkedList<T, A, N, M, S>::emplace_back(P&&... args) {
   size_type index = size_ % chunk_size;
   pointer ptr;
   if (index > 0) {
      size_type node_index = size_ / chunk_size;
      ptr = get_node_(node_index) + index;
   } else {
      // node hasn't been allocated yet
      ptr = get_push_node_();
   }
   new (ptr) value_type(std::forward<P>(args)...);
   ++size_;
}

#pragma endregion
#pragma region removal

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::pop_back() {
   size_type new_size = size_ - 1;
   size_type node_index = new_size / chunk_size;
   size_type index = new_size % chunk_size;

   metanode* prev = nullptr;
   metanode* meta = nullptr;
   size_type meta_index;
   pointer& node = get_node_(node_index, prev, meta, meta_index);

   node[index].~T();
   --size_;

   if (index == 0) {
      // destroy node
      dealloc_node_(node);
      node = nullptr;

      if (meta && meta_index == 0) {
         // destroy/dealloc metanode, set previous metanode/staticmetanode next pointer to nullptr
         if (prev) {
            prev->next = delete_meta_(meta);
         } else {
            this->get_static_metanode_().next = delete_meta_(meta);
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Removes all elements from the container.
///
/// \details Invalidates all iterators and references.
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::clear() noexcept {
   // destroy all elements
   for (iterator i = begin(), e = end(); i != e; ++i) {
      (*i).~T();
   }

   deallocate_nodes_(0);
   size_ = 0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Removes a single element at the specified position and shifts
///         all elements remaining after it such that there are no gaps.
///
/// \details All iterators and references to the removed element or any
///         subsequent elements are invalidated.
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::erase(const_iterator pos) {
   return erase(pos, pos + 1);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Removes a range of elements and shifts any remaining after the last
///         removed element such that there are no gaps.
///
/// \details All iterators and references to removed elements or any element
///         after the first removed element will be invalidated.
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::iterator
ChunkedList<T, A, N, M, S>::erase(const_iterator first, const_iterator last) {
   // check that iterators belong to this container and are not out-of-bounds
   assert(first >= begin());
   assert(last <= end());
   assert(first <= last);

   size_type offset = first - begin();

   if (first < last) {
      iterator old_end(end());
      size_type new_size = size_ - (last - first);
      iterator new_end(this, new_size);

      // move [last, end) to [first, first + end - last)
      for (iterator si(last), di(first); si != old_end; ++si, ++di) {
         std::iter_swap(di, si);
      }

      // destroy [new_end, end)
      for (iterator i(new_end); i != old_end; ++i) {
         (*i).~T();
      }

      // deallocate nodes/metanodes used exclusively for [new_end, end)
      size_type index;
      size_type node_index = get_node_index_(new_size, index);
      if (index > 0)
         ++node_index;

      deallocate_nodes_(node_index);
      size_ = new_size;
   }

   return iterator(this, difference_type(offset));
}

#pragma endregion
#pragma region misc

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::allocator_type
ChunkedList<T, A, N, M, S>::get_allocator() const {
   return static_cast<allocator_type>(this->get_node_alloc_());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::resize(size_type count) {
   if (count < size_) {
      // pop back (size_ - count) elements
      this->erase(const_iterator(this, difference_type(count)), end());
   } else if (size_ < count) {
      // add default constructed elements
      this->push_back_n_(count - size_);
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::resize(size_type count, const value_type& value) {
   if (count < size_) {
      // pop back (size_ - count) elements
      this->erase(const_iterator(this, difference_type(size_ - count)), end());
   } else if (size_ < count) {
      // add copies of value
      this->push_back_n_(count - size_, value);
   }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Swaps the contents of two cunked_lists without copying elements.
///
/// \details All references to elements remain valid, but all iterators are
///         invalidated.  This behavior differs from that of standard-library
///         containers.
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::swap(container& other) {
   using std::swap;
   if (this == &other) {
      // nop
   } else if (this->get_node_alloc_() == other.get_node_alloc_()) {
      swap(size_, other.size_);
      swap(this->get_static_metanode_(), other.get_static_metanode_());
   } else if (node_alloc::propagate_on_container_swap::value) {
      this->swap_allocators_();
      swap(size_, other.size_);
      swap(this->get_static_metanode_(), other.get_static_metanode_());
   } else {
      // containers are incompatible
      assert(false);
   }
}

#pragma endregion
#pragma region private

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::get_node_index_(size_type pos) {
   return pos / chunk_size;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::get_node_index_(size_type pos, size_type& index) {
   index = pos % chunk_size;
   return pos / chunk_size;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::pointer
ChunkedList<T, A, N, M, S>::get_node_(size_type node_index) {
   if (node_index < static_chunks) {
      assert(this->get_static_metanode_().nodes[node_index]);
      return this->get_static_metanode_().nodes[node_index];
   }

   node_index -= static_chunks;
   metanode* meta = this->get_static_metanode_().next;
   assert(meta);

   while (node_index >= chunks_per_metanode) {
      assert(meta->next);
      meta = meta->next;
      node_index -= chunks_per_metanode;
   }

   assert(meta->nodes[node_index]);
   return meta->nodes[node_index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::pointer&
ChunkedList<T, A, N, M, S>::get_node_(size_type node_index,
                                      metanode*& prev_meta,
                                      metanode*& meta,
                                      size_type& meta_index) {
   if (node_index < static_chunks) {
      assert(this->get_static_metanode_().nodes[node_index]);
      prev_meta = nullptr;
      meta = nullptr;
      return this->get_static_metanode_().nodes[node_index];
   }

   node_index -= static_chunks;
   prev_meta = nullptr;
   meta = this->get_static_metanode_().next;
   assert(meta);

   while (node_index >= chunks_per_metanode) {
      assert(meta->next);
      prev_meta = meta;
      meta = meta->next;
      node_index -= chunks_per_metanode;
   }

   assert(meta->nodes[node_index]);
   meta_index = node_index;
   return meta->nodes[node_index];
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::metanode*
ChunkedList<T, A, N, M, S>::get_last_meta_(size_type& meta_index) {
   if (size_ == 0) {
      meta_index = static_chunks;
      return nullptr;
   }

   size_type node_index = (size_ - 1) / chunk_size;
   if (node_index < static_chunks) {
      meta_index = node_index;
      return nullptr;
   }

   node_index -= static_chunks;
   metanode* meta = this->get_static_metanode_().next;
   assert(meta);

   while (node_index >= chunks_per_metanode) {
      assert(meta->next);
      meta = meta->next;
      node_index -= chunks_per_metanode;
   }

   meta_index = node_index;
   return meta;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::pointer
ChunkedList<T, A, N, M, S>::alloc_node_() {
   // allocated memory has no effective type so reinterpret_cast shouldn't
   // cause any strict aliasing issues
   return reinterpret_cast<pointer>(this->get_node_alloc_().allocate(1));
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void
ChunkedList<T, A, N, M, S>::dealloc_node_(pointer node) {
   this->get_node_alloc_().deallocate(reinterpret_cast<typename node_alloc::pointer>(node), 1);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::pointer
ChunkedList<T, A, N, M, S>::get_push_node_() {
   size_type meta_index;
   metanode* meta = get_last_meta_(meta_index);
   ++meta_index;
   if (!meta) {
      // no metanodes yet
      if (meta_index < static_chunks) {
         // still space for static nodes, let's make one!
         pointer& ptr = this->get_static_metanode_().nodes[meta_index];
         if (!ptr) ptr = alloc_node_();
         return ptr;
      } else if (size_ == 0) {
         // first element, need to create first node.
         pointer& ptr = this->get_static_metanode_().nodes[0];
         if (!ptr) ptr = alloc_node_();
         return ptr;
      } else {
         // no static node slots free, create first metanode
         metanode*& mptr = this->get_static_metanode_().next;
         if (!mptr) mptr = new_meta_();
         pointer& ptr = mptr->nodes[0];
         if (!ptr) ptr = alloc_node_();
         return ptr;
      }
   } else {
      // there is already at least 1 metanode
      if (meta_index < chunks_per_metanode) {
         // still space for a node in the current meta
         pointer& ptr = meta->nodes[meta_index];
         if (!ptr) ptr = alloc_node_();
         return ptr;
      } else {
         // metanode is full, make another
         metanode*& mptr = meta->next;
         if (!mptr) mptr = new_meta_();
         pointer& ptr = mptr->nodes[0];
         if (!ptr) ptr = alloc_node_();
         return ptr;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::metanode*
ChunkedList<T, A, N, M, S>::new_meta_() {
   metanode* meta = this->get_meta_alloc_().allocate(1);
   return new (meta) metanode();
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::metanode*
ChunkedList<T, A, N, M, S>::delete_meta_(metanode* meta) {
   metanode* next = meta->next;
   (*meta).~metanode();
   this->get_meta_alloc_().deallocate(meta, 1);
   return next;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::cleanup_() {
   size_type index;
   size_type node_index = this->get_node_index_(size_, index);
   if (index > 0) {
      ++node_index;
   }

   deallocate_nodes_(node_index);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Deallocates all nodes with indices >= first_index and cleans up any
///         metanodes that are no longer necessary.
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::deallocate_nodes_(size_type first_index) {
   if (first_index <= static_chunks) {
      // deallocate static nodes & set to null
      for (size_type i = first_index; i < static_chunks; ++i) {
         pointer& ptr = this->get_static_metanode_().nodes[i];

         if (!ptr) {
            return; // static nodes not yet filled; assume get_static_metanode_().next is null already
         }

         dealloc_node_(ptr);
         ptr = nullptr;
      }

      // deallocate nodes & metanodes
      delete_metanodes_(this->get_static_metanode_().next);
      this->get_static_metanode_().next = nullptr;
   } else {
      // first node to deallocate is in a metanode
      first_index -= static_chunks;

      metanode* meta = this->get_static_metanode_().next;
      metanode* prev = nullptr;

      while (first_index >= chunks_per_metanode) {
         if (!meta) {
            return;
         }

         prev = meta;
         meta = meta->next;
         first_index -= chunks_per_metanode;
      }

      if (!meta) {
         return;
      }

      if (first_index == 0) {
         delete_metanodes_(meta);
         prev->next = nullptr;
      } else {
         for (size_type i = first_index; i < chunks_per_metanode; ++i) {
            pointer& ptr = meta->nodes[i];

            if (!ptr) {
               return; // end of nodes found
            }

            dealloc_node_(meta->nodes[i]);
         }

         delete_metanodes_(meta->next);
         meta->next = nullptr;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Deallocates all nodes in the provided metanode and deallocates the
///         metanode itself.  If meta->next is nonnull, it will recursively
///         deallocate that node as well.
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::delete_metanodes_(metanode* meta) {
   while (meta) {
      for (size_type i = 0; i < chunks_per_metanode; ++i) {
         pointer& ptr = meta->nodes[i];

         if (!ptr)
            break; // end of nodes found; just need to delete this metanode now

         dealloc_node_(meta->nodes[i]);
      }

      meta = delete_meta_(meta);
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::get_capacity_() const {
   size_type capacity = 0;

   for (size_type i = 0; i < static_chunks; ++i) {
      if (this->get_static_metanode_().nodes[i]) {
         capacity += chunk_size;
      } else {
         return capacity;
      }
   }

   metanode* meta = this->get_static_metanode_().next;
   while (meta) {
      for (size_type i = 0; i < chunks_per_metanode; ++i) {
         if (meta->nodes[i]) {
            capacity += chunk_size;
         } else {
            return capacity;
         }
      }

      meta = meta->next;
   }

   return capacity;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::get_capacity_(metanode*& last_meta) {
   size_type capacity = 0;

   for (size_type i = 0; i < static_chunks; ++i) {
      if (this->get_static_metanode_().nodes[i]) {
         capacity += chunk_size;
      } else {
         last_meta = nullptr;
         return capacity;
      }
   }

   metanode* meta = this->get_static_metanode_().next;
   if (meta) {
      for (;;) {
         for (size_type i = 0; i < chunks_per_metanode; ++i) {
            if (meta->nodes[i]) {
               capacity += chunk_size;
            } else {
               last_meta = meta;
               return capacity;
            }
         }

         if (meta->next == nullptr) {
            break;
         }
      }
   }

   last_meta = meta;
   return capacity;
}

///////////////////////////////////////////////////////////////////////////////
/// \return A pointer to the
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::ensure_capacity_(size_type count) {
   if (size_ >= count) {
      return size_;
   }

   metanode* last_meta = nullptr;
   size_type capacity = get_capacity_(last_meta);

   if (capacity >= count) {
      return capacity;
   }

   size_type nodes_to_add = (count - capacity + chunk_size - 1) / chunk_size;

   capacity += nodes_to_add * chunk_size;

   metanode** next = nullptr;
   if (last_meta == nullptr) {
      nodes_to_add = alloc_nodes_<staticmetanode, static_chunks>(&this->get_static_metanode_(), nodes_to_add);
      next = &(this->get_static_metanode_().next);
   } else {
      nodes_to_add = alloc_nodes_<metanode, chunks_per_metanode>(last_meta, nodes_to_add);
      next = &(last_meta->next);
   }

   while (nodes_to_add > 0) {
      last_meta = new_meta_();
      *next = last_meta;
      next = &(last_meta->next);
      nodes_to_add = alloc_nodes_<metanode, chunks_per_metanode>(last_meta, nodes_to_add);
   }

   return capacity;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief  Attempts to allocate new nodes and attach them to the provided
///         metanode.
///
/// \details When the metanode is full, the function will return a number > 0
///         indicating how many more nodes were requested to be allocated but
///         could not be.
///
/// \return The number of nodes requested which could not be allocated.
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename Metanode, std::size_t ChunksPerMetanode>
typename ChunkedList<T, A, N, M, S>::size_type
ChunkedList<T, A, N, M, S>::alloc_nodes_(Metanode* metanode, size_type count) {
   for (size_type i = 0; count > 0 && i < ChunksPerMetanode; ++i)    {
      pointer& ptr = metanode->nodes[i];
      if (!ptr) {
         ptr = alloc_node_();
         --count;
      }
   }

   return count;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::push_back_n_(size_type count) {
   if (count == 0) {
      return;
   }

   size_type old_size = size_;
   ensure_capacity_(size_ + count);

   size_type node_index = size_ / chunk_size;
   size_type index = size_ % chunk_size;

   try {
      pointer node = get_node_(node_index);
      for (size_type i = 0;;) {
         new (node + index) value_type();
         ++size_;

         ++i;
         if (i == count) {
            break;
         }

         if (index < chunk_size - 1) {
            ++index;
         } else {
            ++node_index;
            index = 0;
            node = get_node_(node_index);
         }
      }
   } catch (...) {
      erase(iterator(this, difference_type(old_size)), end());
      throw;
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::push_back_n_(size_type count, const value_type& value) {
   if (count == 0) {
      return;
   }

   size_type old_size = size_;
   ensure_capacity_(size_ + count);

   size_type node_index = size_ / chunk_size;
   size_type index = size_ % chunk_size;

   try {
      pointer node = get_node_(node_index);
      for (size_type i = 0;;) {
         new (node + index) value_type(value);
         ++size_;

         ++i;
         if (i == count) {
            break;
         }

         if (index < chunk_size - 1) {
            ++index;
         } else {
            ++node_index;
            index = 0;
            node = get_node_(node_index);
         }
      }
   } catch (...) {
      erase(iterator(this, difference_type(old_size)), end());
      throw;
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I>
void ChunkedList<T, A, N, M, S>::insert_(const_iterator pos, I first, I last, std::input_iterator_tag) {
   if (first != last) {
      size_type old_size = size_;
      size_type offset = pos - begin();
      try {
         while (first != last) {
            push_back(*first);
            ++first;
         }
      } catch (...) {
         erase(iterator(this, difference_type(old_size)), end());
         throw;
      }

      std::rotate(iterator(this, difference_type(offset)), iterator(this, difference_type(old_size)), end());
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I>
void ChunkedList<T, A, N, M, S>::insert_(const_iterator pos, I first, I last, std::forward_iterator_tag) {
   size_type offset = pos - begin();
   size_type old_size = size_;
   size_type count = std::distance(first, last);
   if (count == 0) {
      return;
   }

   ensure_capacity_(size_ + count);

   // construct new entries at back
   size_type node_index = size_ / chunk_size;
   size_type index = size_ % chunk_size;

   try {
      pointer node = get_node_(node_index);
      for (size_type i = 0;;) {
         new (node + index) value_type(*first);
         ++size_;
         ++first;

         ++i;
         if (i == count) {
            break;
         }

         if (index < chunk_size - 1) {
            ++index;
         } else {
            ++node_index;
            index = 0;
            node = get_node_(node_index);
         }
      }
   } catch (...) {
      erase(iterator(this, difference_type(old_size)), end());
      throw;
   }

   // place new entries correctly
   std::rotate(iterator(this, difference_type(offset)), iterator(this, difference_type(old_size)), end());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I>
void ChunkedList<T, A, N, M, S>::insert_back_(I first, I last, std::input_iterator_tag) {
   if (first != last) {
      size_type old_size = size_;
      try {
         while (first != last) {
            push_back(*first);
            ++first;
         }
      } catch (...) {
         erase(iterator(this, difference_type(old_size)), end());
         throw;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I>
void ChunkedList<T, A, N, M, S>::insert_back_(I first, I last, std::forward_iterator_tag) {
   size_type old_size = size_;
   size_type count = std::distance(first, last);
   if (count == 0)
      return;

   ensure_capacity_(size_ + count);

   // construct new entries at back
   size_type node_index = size_ / chunk_size;
   size_type index = size_ % chunk_size;

   try {
      pointer node = get_node_(node_index);
      for (size_type i = 0;;) {
         new (node + index) value_type(*first);
         ++size_;
         ++first;

         ++i;
         if (i == count) {
            break;
         }

         if (index < chunk_size - 1) {
            ++index;
         } else {
            ++node_index;
            index = 0;
            node = get_node_(node_index);
         }
      }
   } catch (...) {
      erase(iterator(this, difference_type(old_size)), end());
      throw;
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I>
void ChunkedList<T, A, N, M, S>::assign_(I first, I last, std::input_iterator_tag) {
   for (iterator i = begin(), e = end(); i != e; ++i) {
      (*i).~T();
   }

   size_ = 0;
   insert_back_(first, last, std::input_iterator_tag());
   cleanup_();
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
template <typename I>
void ChunkedList<T, A, N, M, S>::assign_(I first, I last, std::forward_iterator_tag) {
   for (iterator i = begin(), e = end(); i != e; ++i) {
      (*i).~T();
   }

   size_ = 0;
   insert_back_(first, last, std::forward_iterator_tag());
   cleanup_();
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::assign_(const_iterator first, const_iterator last, std::random_access_iterator_tag) {
   if (first.container_ == this) {
      erase(last, end());
      erase(begin(), first);
   } else {
      // from another container
      assign_(first, last, std::forward_iterator_tag());
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::assign_(iterator first, iterator last, std::random_access_iterator_tag) {
   if (first.container_ == this)    {
      erase(last, end());
      erase(begin(), first);
   } else {
      // from another container
      assign_(first, last, std::forward_iterator_tag());
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
void ChunkedList<T, A, N, M, S>::move_assign_(const_iterator first, const_iterator last) {
   for (iterator i = begin(), e = end(); i != e; ++i) {
      (*i).~T();
   }

   size_ = 0;
   size_type count = last - first;
   if (count == 0) {
      cleanup_();
      return;
   }
   ensure_capacity_(count);

   size_type node_index = 0;
   size_type index = 0;

   try {
      pointer node = get_node_(node_index);
      for (;;) {
         new (node + index) value_type(std::move(*first));
         ++size_;
         ++first;

         if (first == last) {
            break;
         }

         if (index < chunk_size - 1) {
            ++index;
         } else {
            ++node_index;
            index = 0;
            node = get_node_(node_index);
         }
      }
   } catch (...) {
      clear();
      throw;
   }

   cleanup_();
}

#pragma endregion
#pragma region comparison operators

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator==(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right) {
   return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator!=(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right) {
   return !(left == right);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator<(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right) {
   return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator>(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right) {
   return right < left;
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator<=(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right) {
   return !(right < left);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator>=(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right) {
   return !(left < right);
}

/////////////////////////////////////////////////////////////////////////////////
//template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
//void PrintTraits<ChunkedList<T, A, N, M, S>>::name(std::ostream& os, const ChunkedList<T, A, N, M, S>&) const
//{
//   detail::containerName<T>(os, "ChunkedList");
//}

#pragma endregion

} // be::util

#endif

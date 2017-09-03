#pragma once
#ifndef BE_UTIL_CHUNKED_LIST_HPP_
#define BE_UTIL_CHUNKED_LIST_HPP_

#include "chunked_list_iterator.hpp"
#include <be/core/t_container_types.hpp>
#include <be/core/t_is_iterator.hpp>
#include <be/core/small_triplet.hpp>

namespace be::util {
namespace detail {

///////////////////////////////////////////////////////////////////////////////
template <typename T, std::size_t N>
struct ChunkedListNode {
   T v[N];
};

///////////////////////////////////////////////////////////////////////////////
template<typename P, std::size_t M, std::size_t S = M>
struct ChunkedListMetanode {
   using metanode = ChunkedListMetanode<P, M>;
public:
   ChunkedListMetanode() : next(nullptr) {
      for (size_t i = 0; i < S; ++i) {
         nodes[i] = P();
      }
   }

   metanode* next;
   P nodes[S];
};

///////////////////////////////////////////////////////////////////////////////
template<typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
class ChunkedListBase {
   using base = ChunkedListBase<T, A, N, M, S>;
protected:
   using node = ChunkedListNode<T, N>;
   using allocator = typename t::Select<t::IsSimpleAlloc<typename t::ContainerTypes<T, A>::allocator>::value, A, std::allocator<T>>::type;
   using contypes = t::ContainerTypes<T, allocator>;
   using nodetypes = t::ContainerTypes<node, allocator>;
   using types = typename contypes::types;

   using allocator_type = allocator;
   using value_type = typename types::value_type;
   using size_type = typename types::size_type;
   using difference_type = typename types::difference_type;
   using pointer = typename types::pointer;
   using const_pointer = typename types::const_pointer;
   using reference = typename types::reference;
   using const_reference = typename types::const_reference;

   using staticmetanode = ChunkedListMetanode<pointer, M, S>;
   using metanode = ChunkedListMetanode<pointer, M>;
   using metatypes = t::ContainerTypes<metanode, allocator>;
   
   using node_alloc = typename nodetypes::allocator;
   using meta_alloc = typename metatypes::allocator;

   ChunkedListBase(const allocator& alloc = allocator())
      : triplet_(OneArgTag(), OneArgTag(), alloc, alloc)
   { }

   void set_allocator_(const allocator& alloc) {
      get_node_alloc_() = alloc;
      get_meta_alloc_() = alloc;
   }

   void set_node_allocator_(const node_alloc& alloc) {
      get_node_alloc_() = alloc;
   }

   void set_metanode_allocator_(const meta_alloc& alloc) {
      get_meta_alloc_() = alloc;
   }

   void swap_allocators_(base& other) {
      using std::swap;
      swap(get_node_alloc_(), other.get_node_alloc_());
      swap(get_meta_alloc_(), other.get_meta_alloc_());
   }

   node_alloc& get_node_alloc_() noexcept {
      return triplet_.first();
   }

   const node_alloc& get_node_alloc_() const noexcept {
      return triplet_.first();
   }

   meta_alloc& get_meta_alloc_() noexcept {
      return triplet_.second();
   }

   const meta_alloc& get_meta_alloc_() const noexcept {
      return triplet_.second();
   }

   staticmetanode& get_static_metanode_() noexcept {
      return triplet_.third();
   }

   const staticmetanode& get_static_metanode_() const noexcept {
      return triplet_.third();
   }

private:
   SmallTriplet<node_alloc, meta_alloc, staticmetanode> triplet_;
};

} // be::util::detail

///////////////////////////////////////////////////////////////////////////////
/// \brief  A random-access container that always allocates fixed-size blocks.
///
/// \details chunked_list is designed for situations where a large number of
///         containers need to be managed, each typically holding a relatively
///         small number of elements.  It provides a selection of the benefits
///         of vectors, deques, and lists.
///
///         Like a vector, chunked_list provides good cache performance, since
///         consecutive elements are generally laid out in an array (acually,
///         it may be several arrays).  It supports random element access in
///         constant time for small containers, and O(N) in the worst case
///         (very large containers).  Vectors are slightly more efficient since
///         they always store all their data in a single array.
///
///         Like a deque, inserting and erasing elements at the back of a
///         chunked_list will never invalidate any iterators or references
///         except the past-the-end iterator, and have complexity O(1).
///         Unlike a deque, insertion or removal at the beginning of the
///         container is O(N), and will invalidate any iterators or references
///         to removed elements or any elements after the inserted/removed
///         elements.  In this respect it behaves like a vector.
///
///         Like a list, chunked_list always uses its allocator's `allocate(1)`
///         function, facilitating the use of fast, cache-friendly fixed-size
///         block allocators which prevent heap fragmentation.  A second
///         allocator will also be rebind-constructed from the container's
///         allocator for allocating meta-nodes (a singly linked list
///         structure which points to data nodes when the container grows too
///         large to hold pointers to all data nodes itself).
///
///         Template parameters can be used to tune the number of elements
///         each data node holds, the number of data nodes each meta-node
///         points to, and the number of nodes that the chunked_list itself
///         points to.
///
///         chunked_list supports only simple allocators.  In other words
///         std::allocator_traits<A> must define the following member
///         types (where T is first template parameter of chunked_list):
///
///               - `typedef T value_type;`
///               - `typedef std::size_t size_type;`
///               - `typedef ptrdiff_t difference_type;`
///               - `typedef T* pointer;`
///               - `typedef const T* const_pointer;`
///               - `typedef T& reference;`
///               - `typedef const T& const_reference;`
///
///         Attempting to use a non-simple allocator will result in
///         std::allocator being used instead.
///
///         Allocator::construct() is not used to construct objects;
///         placement-new is used directly.  If non-standard construction
///         is required, override operator new (size_t, void*).
///
/// \tparam T The type of objects to store in the container.
/// \tparam A An allocator for constructing nodes and metanodes.
/// \tparam N The number of objects per node.
/// \tparam M The number of nodes tracked per metanode.
/// \tparam S The number of nodes that can be tracked without allocating any
///         metanodes.
template <typename T, typename A = std::allocator<T>, std::size_t N = 16, std::size_t M = 7, std::size_t S = 2>
class ChunkedList : public detail::ChunkedListBase<T, A, N, M, S> {
   using container = ChunkedList<T, A, N, M, S>;
   using base = detail::ChunkedListBase<T, A, N, M, S>;
   using node_alloc = typename base::node_alloc;
   using meta_alloc = typename base::meta_alloc;
   using staticmetanode = typename base::staticmetanode;
   using metanode = typename base::metanode;

   friend class detail::ChunkedListIterator<container>;
   friend class detail::ChunkedListConstIterator<container>;
   friend void swap(container& a, container& b) { a.swap(b); }

public:
   using allocator_type = typename base::allocator_type;
   using value_type = typename base::value_type;
   using size_type = typename base::size_type;
   using difference_type  = typename base::difference_type;
   using pointer = typename base::pointer;
   using const_pointer = typename base::const_pointer;
   using reference = typename base::reference;
   using const_reference = typename base::const_reference;

   static constexpr const std::size_t chunk_size = N;
   static constexpr const std::size_t chunks_per_metanode = M;
   static constexpr const std::size_t static_chunks = S;

   using iterator = typename detail::ChunkedListIterator<container>;
   using const_iterator = typename detail::ChunkedListConstIterator<container>;
   using reverse_iterator = std::reverse_iterator<iterator>;
   using const_reverse_iterator = std::reverse_iterator<const_iterator>;

   ChunkedList();
   explicit ChunkedList(const allocator_type& alloc);
   ChunkedList(size_type count, const value_type& value, const allocator_type& alloc = allocator_type());
   explicit ChunkedList(size_type count, const allocator_type& alloc = allocator_type());
   template <typename I, typename = typename std::enable_if<t::IsIterator<I>::value, void>::type>
   ChunkedList(I first, I last, const allocator_type& alloc = allocator_type());
   ChunkedList(const container& other);
   ChunkedList(const container& other, const allocator_type& alloc);
   ChunkedList(container&& other);
   ChunkedList(container&& other, const allocator_type& alloc);
   ChunkedList(std::initializer_list<value_type> il, const allocator_type& alloc = allocator_type());

   ~ChunkedList();

   container& operator=(const container& other);
   container& operator=(container&& other);
   container& operator=(std::initializer_list<value_type> il);

   void assign(size_type count, const value_type& value);
   template <typename I, typename = typename std::enable_if<t::IsIterator<I>::value, void>::type>
   void assign(I first, I last);
   void assign(std::initializer_list<value_type> il);

   allocator_type get_allocator() const;

   reference at(size_type pos);
   const_reference at(size_type pos) const;
   reference operator[](size_type pos);
   const_reference operator[](size_type pos) const;

   reference front();
   const_reference front() const;

   reference back();
   const_reference back() const;

   iterator begin() noexcept;
   const_iterator begin() const noexcept;
   const_iterator cbegin() const noexcept;

   iterator end() noexcept;
   const_iterator end() const noexcept;
   const_iterator cend() const noexcept;

   reverse_iterator rbegin() noexcept;
   const_reverse_iterator rbegin() const noexcept;
   const_reverse_iterator crbegin() const noexcept;

   reverse_iterator rend() noexcept;
   const_reverse_iterator rend() const noexcept;
   const_reverse_iterator crend() const noexcept;

   bool empty() const noexcept;
   size_type size() const noexcept;
   size_type max_size() const noexcept;

   iterator insert(const_iterator pos, const value_type& value);
   iterator insert(const_iterator pos, value_type&& value);
   iterator insert(const_iterator pos, size_type count, const value_type& value);
   template <typename I, typename = typename std::enable_if<t::IsIterator<I>::value, void>::type>
   iterator insert(const_iterator pos, I first, I last);
   iterator insert(const_iterator pos, std::initializer_list<value_type> il);
   template <class... P>
   iterator emplace(const_iterator pos, P&&... args);

   void push_back(const value_type& value);
   void push_back(value_type&& value);
   template <class... P>
   void emplace_back(P&&... args);
   
   void pop_back();

   void clear() noexcept;

   iterator erase(const_iterator pos);
   iterator erase(const_iterator first, const_iterator last);

   void resize(size_type count);
   void resize(size_type count, const value_type& value);

   void swap(container& other);

private:
   static size_type get_node_index_(size_type pos);
   static size_type get_node_index_(size_type pos, size_type& index);

   pointer get_node_(size_type node_index);
   pointer& get_node_(size_type node_index,
                     metanode*& prev_meta,
                     metanode*& meta,
                     size_type& meta_index);

   metanode* get_last_meta_(size_type& meta_index);

   pointer alloc_node_();
   void dealloc_node_(pointer node);

   pointer get_push_node_();

   metanode* new_meta_();
   metanode* delete_meta_(metanode* meta);

   void cleanup_();

   void deallocate_nodes_(size_type first_index);
   void delete_metanodes_(metanode* meta);

   size_type get_capacity_() const;
   size_type get_capacity_(metanode*& last_meta);
   size_type ensure_capacity_(size_type count);

   template <typename Metanode, std::size_t ChunksPerMetanode>
   size_type alloc_nodes_(Metanode* metanode, size_type count);

   void push_back_n_(size_type count);
   void push_back_n_(size_type count, const value_type& value);

   template <typename I>
   void insert_(const_iterator pos, I first, I last, std::input_iterator_tag);

   template <typename I>
   void insert_(const_iterator pos, I first, I last, std::forward_iterator_tag);

   template <typename I>
   void insert_back_(I first, I last, std::input_iterator_tag);

   template <typename I>
   void insert_back_(I first, I last, std::forward_iterator_tag);

   template <typename I>
   void assign_(I first, I last, std::input_iterator_tag);

   template <typename I>
   void assign_(I first, I last, std::forward_iterator_tag);

   void assign_(const_iterator first, const_iterator last, std::random_access_iterator_tag);
   void assign_(iterator first, iterator last, std::random_access_iterator_tag);

   void move_assign_(const_iterator first, const_iterator last);

   size_type size_;
};

template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator==(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right);

template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator!=(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right);

template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator<=(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right);

template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator>(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right);

template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator<=(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right);

template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
bool operator>(ChunkedList<T, A, N, M, S>& left, ChunkedList<T, A, N, M, S>& right);

//template <typename T, typename A, std::size_t N, std::size_t M, std::size_t S>
//struct PrintTraits<chunked_list<T, A, N, M, S>> : PrintTraits<void>
//{
//   void name(std::ostream& os, const chunked_list<T, A, N, M, S>&) const;
//};

} // be::util

#include "chunked_list.inl"

#endif

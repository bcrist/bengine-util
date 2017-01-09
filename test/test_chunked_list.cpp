#ifdef BE_TEST

#include <catch/catch.hpp>
#include <functional>
#include "chunked_list.hpp"

#define BE_CATCH_TAGS "[util][util:ChunkedList]"

using namespace be;

static std::function<void(void*, size_t, const std::type_info&)> cb_alloc;
static std::function<void(void*, size_t, const std::type_info&)> cb_dealloc;

template <class T, template <class> class A = std::allocator>
class ProxyAlloc : public A<T> {
public:
   using pointer = typename A<T>::pointer;
   using size_type = typename A<T>::size_type;

   template<class O>
   struct rebind {
      using other = ProxyAlloc<O, A>;
   };

   ProxyAlloc() noexcept { }

   ProxyAlloc(const ProxyAlloc<T, A>&) noexcept { }

   template<class O>
   ProxyAlloc(const ProxyAlloc<O, A>&) noexcept { }

   template<class O>
   ProxyAlloc<T, A>& operator=(const ProxyAlloc<O, A>&) {
      return (*this);
   }

   void deallocate(pointer ptr, size_type count) {
      if (cb_dealloc) {
         cb_dealloc(ptr, count, typeid(T));
      }

      static_cast<A<T>*>(this)->deallocate(ptr, count);
   }

   pointer allocate(size_type count) {
      pointer ptr = static_cast<A<T>*>(this)->allocate(count);

      if (cb_alloc) {
         cb_alloc(ptr, count, typeid(T));
      }
      
      return ptr;
   }

   pointer allocate(size_type count, const void * oldptr) {
      return allocate(count);
   }   
};

#ifndef SETUP_ALLOC_CBS
#define SETUP_ALLOC_CBS \
   int total_allocs = 0; \
   int total_deallocs = 0; \
   int active_allocs = 0; \
   \
   cb_alloc = [&](void*, size_t, const std::type_info&) { \
      ++total_allocs; \
      ++active_allocs; \
   }; \
   \
   cb_dealloc = [&](void*, size_t, const std::type_info&) { \
      ++total_deallocs; \
      --active_allocs; \
   };
#endif

#ifndef CLEANUP_ALLOC_CBS
#define CLEANUP_ALLOC_CBS \
   REQUIRE(active_allocs == 0); \
   cb_alloc = cb_dealloc = nullptr;
#endif

TEST_CASE("util::ChunkedList<T> default construction, destruction", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> con;

   REQUIRE(con.size() == 0);
   REQUIRE(con.empty());
   REQUIRE(con.begin() == con.end());
   REQUIRE(con.max_size() >= 1);

   SECTION("clear() should not change size") {
      con.clear();

      REQUIRE(con.size() == 0);
      REQUIRE(con.empty());
      REQUIRE(con.begin() == con.end());
      REQUIRE(con.max_size() >= 1);
   }

   SECTION("assign({}) should not change size") {
      con.clear();

      REQUIRE(con.size() == 0);
      REQUIRE(con.empty());
      REQUIRE(con.begin() == con.end());
      REQUIRE(con.max_size() >= 1);
   }
}

TEST_CASE("util::ChunkedList<T, A> default construction, destruction", BE_CATCH_TAGS) {
   SETUP_ALLOC_CBS

   SECTION("util::ChunkedList<int, ProxyAlloc<int>, 8, 7, 1>") {
      be::util::ChunkedList<int, ProxyAlloc<int>, 8, 7, 1> con;

      REQUIRE(con.chunk_size == 8);
      REQUIRE(con.chunks_per_metanode == 7);
      REQUIRE(con.static_chunks == 1);

      REQUIRE(active_allocs == 0);
      REQUIRE(total_allocs == 0);
      REQUIRE(total_deallocs == 0);

      REQUIRE(con.size() == 0);
      REQUIRE(con.empty());
      REQUIRE(con.begin() == con.end());
      REQUIRE(con.max_size() >= 1);

      SECTION("clear() on empty container should not allocate or deallocate memory.") {
         con.clear();

         REQUIRE(active_allocs == 0);
         REQUIRE(total_allocs == 0);
         REQUIRE(total_deallocs == 0);

         REQUIRE(con.size() == 0);
         REQUIRE(con.empty());
         REQUIRE(con.begin() == con.end());
         REQUIRE(con.max_size() >= 1);
      }

      SECTION("clear() on empty container should not allocate or deallocate memory.") {
         con.assign({ });

         REQUIRE(active_allocs == 0);
         REQUIRE(total_allocs == 0);
         REQUIRE(total_deallocs == 0);

         REQUIRE(con.size() == 0);
         REQUIRE(con.empty());
         REQUIRE(con.begin() == con.end());
         REQUIRE(con.max_size() >= 1);
      }
   }

   REQUIRE(total_allocs == 0);
   REQUIRE(total_deallocs == 0);
   CLEANUP_ALLOC_CBS
}

TEST_CASE("util::ChunkedList push_back()", BE_CATCH_TAGS) {
   SETUP_ALLOC_CBS

   SECTION("util::ChunkedList<be::U64, ProxyAlloc<be::U64>, 16, 1, 1>") {
      be::util::ChunkedList<be::U64, ProxyAlloc<be::U64>, 16, 1, 1> con;

      REQUIRE(active_allocs == 0);
      REQUIRE(total_allocs == 0);
      REQUIRE(total_deallocs == 0);
      REQUIRE(con.empty());

      SECTION("push_back(value_type&) adds the correct value.") {
         con.push_back(47);

         REQUIRE(con.size() == 1);
         REQUIRE(con.front() == 47);
         REQUIRE(con.back() == 47);
         REQUIRE(con.at(0) == 47);
         REQUIRE(con[0] == 47);

         REQUIRE(active_allocs == 1);
         REQUIRE(total_allocs == 1);
         REQUIRE(total_deallocs == 0);

         SECTION("push_back(value_type&) again adds to back.") {
            con.push_back(7);

            REQUIRE(con.size() == 2);
            REQUIRE(con.front() == 47);
            REQUIRE(con.at(0) == 47);
            REQUIRE(con[0] == 47);
            REQUIRE(con.back() == 7);
            REQUIRE(con.at(1) == 7);
            REQUIRE(con[1] == 7);

            REQUIRE(active_allocs == 1);
            REQUIRE(total_allocs == 1);
            REQUIRE(total_deallocs == 0);

            SECTION("clear() removes all values, deallocates node.") {
               con.clear();

               REQUIRE(con.begin() == con.end());
               REQUIRE(con.empty());

               REQUIRE(active_allocs == 0);
               REQUIRE(total_allocs == 1);
               REQUIRE(total_deallocs == 1);
            }
         }

         SECTION("clear() removes value, deallocates node.") {
            con.clear();

            REQUIRE(con.begin() == con.end());
            REQUIRE(con.empty());

            REQUIRE(active_allocs == 0);
            REQUIRE(total_allocs == 1);
            REQUIRE(total_deallocs == 1);

            SECTION("push_back(value_type&) again reallocates, inserts") {
               con.push_back(7);

               REQUIRE(active_allocs == 1);
               REQUIRE(total_allocs == 2);
               REQUIRE(total_deallocs == 1);
               REQUIRE(con.front() == 7);

               SECTION("pushing more values adds them to the end") {
                  con.push_back(8);
                  con.push_back(9);

                  REQUIRE(active_allocs == 1);
                  REQUIRE(total_allocs == 2);
                  REQUIRE(total_deallocs == 1);

                  REQUIRE(con.size() == 3);
                  REQUIRE(!con.empty());
                  REQUIRE(con.front() == 7);
                  REQUIRE(con.at(0) == 7);
                  REQUIRE(con[0] == 7);
                  REQUIRE(con.at(1) == 8);
                  REQUIRE(con[1] == 8);
                  REQUIRE(con.back() == 9);
                  REQUIRE(con.at(2) == 9);
                  REQUIRE(con[2] == 9);

                  REQUIRE(*con.begin() == 7);
                  REQUIRE(*++con.begin() == 8);
                  REQUIRE(*++++con.begin() == 9);
                  REQUIRE(++++++con.begin() == con.end());
               }
            }
         }
      }
   }

   SECTION("util::ChunkedList<std::unique_ptr<int>, ProxyAlloc<std::unique_ptr<int>>, 2, 4, 4>") {
      be::util::ChunkedList<std::unique_ptr<int>, ProxyAlloc<std::unique_ptr<int>>, 2, 4, 4> con;

      REQUIRE(active_allocs == 0);
      REQUIRE(total_allocs == 0);
      REQUIRE(total_deallocs == 0);
      REQUIRE(con.empty());

      SECTION("unique_ptr is non-copyable; push_back must accept r-value reference (move).") {
         con.push_back(std::unique_ptr<int>());

         REQUIRE(con.size() == 1);

         REQUIRE(active_allocs == 1);
         REQUIRE(total_allocs == 1);
         REQUIRE(total_deallocs == 0);

         SECTION("after node is full, metanode should be allocated") {
            con.push_back(std::unique_ptr<int>(new int(47)));
            REQUIRE(con.size() == 2);

            REQUIRE(active_allocs == 1);
            REQUIRE(total_allocs == 1);
            REQUIRE(total_deallocs == 0);

            con.push_back(std::unique_ptr<int>(new int(7)));
            REQUIRE(con.size() == 3);

            REQUIRE(active_allocs == 2);
            REQUIRE(total_allocs == 2);
            REQUIRE(total_deallocs == 0);

            REQUIRE(*con[2] == 7);
         }
      }
   }

   CLEANUP_ALLOC_CBS
}

TEST_CASE("util::ChunkedList 'push back N default constructed elements' construction", BE_CATCH_TAGS) {
   SECTION("util::ChunkedList<int>(1)") {
      be::util::ChunkedList<int> con(1);

      REQUIRE(con.size() == 1);
      REQUIRE(con.front() == 0);
   }

   SECTION("util::ChunkedList<int>(7)") {
      be::util::ChunkedList<int> con(7);
      REQUIRE(con.size() == 7);
   }

   SECTION("util::ChunkedList<int>(0)") {
      be::util::ChunkedList<int> con(0);
      REQUIRE(con.size() == 0);
   }

   SECTION("util::ChunkedList<int>(200)") {
      be::util::ChunkedList<int> con(200);
      REQUIRE(con.size() == 200);
   }
}

TEST_CASE("util::ChunkedList 'push back N copy constructed elements' construction", BE_CATCH_TAGS) {
   SECTION("util::ChunkedList<int>(1, 7)") {
      be::util::ChunkedList<int> con(1, 7);

      REQUIRE(con.size() == 1);
      REQUIRE(con.front() == 7);
   }

   SECTION("util::ChunkedList<int>(0, 7)") {
      be::util::ChunkedList<int> con(0, 7);
      REQUIRE(con.size() == 0);
      REQUIRE(con.begin() == con.end());
      REQUIRE(con.empty());
   }

   SECTION("util::ChunkedList<int>(2, 7)") {
      be::util::ChunkedList<int> con(2, 7);
      REQUIRE(con.size() == 2);
      REQUIRE(con.front() == 7);
      REQUIRE(con.back() == 7);
   }

   SECTION("util::ChunkedList<int>(50, 7)") {
      be::util::ChunkedList<int> con(50, 7);
      REQUIRE(con.size() == 50);

      for (int i : con) {
         REQUIRE(i == 7);
      }
   }

   SECTION("util::ChunkedList<int>(1000, 7)") {
      be::util::ChunkedList<int> con(1000, 47);
      REQUIRE(con.size() == 1000);
      for (auto i(con.begin()), e(con.begin() + 1000); i != e; ++i) {
         REQUIRE(*i == 47);
      }
   }
}

TEST_CASE("util::ChunkedList iterator copy construction", BE_CATCH_TAGS) {
   std::vector<int> vec { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
   be::util::ChunkedList<int> cl(20, 47);

   SECTION("util::ChunkedList<int>(vec.begin(), vec.end())") {
      be::util::ChunkedList<int> con(vec.begin(), vec.end());

      REQUIRE(con.size() == vec.size());

      auto vi = vec.begin();
      auto ci = con.begin();

      while (vi != vec.end() && ci != con.end()) {
         REQUIRE(*vi == *ci);

         ++vi;
         ++ci;
      }

      REQUIRE(vi == vec.end());
      REQUIRE(ci == con.end());
   }

   SECTION("util::ChunkedList<int>(vec.begin(), vec.begin() + 2)") {
      be::util::ChunkedList<int> con(vec.begin(), vec.begin() + 2);

      REQUIRE(con.size() == 2);
      REQUIRE(con.front() == vec.front());
   }

   SECTION("util::ChunkedList<int>(vec.begin(), vec.begin())") {
      be::util::ChunkedList<int> con(vec.begin(), vec.begin());

      REQUIRE(con.size() == 0);
   }

   SECTION("util::ChunkedList<int>(cl.begin(), cl.end())") {
      be::util::ChunkedList<int> con(cl.begin(), cl.end());
      REQUIRE(con.size() == 20);
      REQUIRE(con.front() == 47);
      REQUIRE(con.back() == 47);
   }

   SECTION("util::ChunkedList<int>(int*, int*)") {
      int a[2] = { 7, 47 };

      be::util::ChunkedList<int> con(a + 0, a + 1);
      REQUIRE(con.size() == 1);
      REQUIRE(con.front() == 7);
      REQUIRE(con.back() == 7);
   }

   SECTION("util::ChunkedList<int>(std::istream_iterator<int>, std::istream_iterator<int>)") {
      std::istringstream iss("1 2 3 4 5 6 7");

      be::util::ChunkedList<int> con { std::istream_iterator<int>(iss), std::istream_iterator<int>() }; // these are input_iterators rather than forward_iterators (different codepath)
      REQUIRE(con.size() == 7);
      REQUIRE(con.front() == 1);
      REQUIRE(con.back() == 7);
   }
}

TEST_CASE("util::ChunkedList copy construction", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> cl;
   for (int i = 0; i < 30; ++i) {
      cl.push_back(i);
   }

   REQUIRE(cl.size() == 30);

   SECTION("util::ChunkedList<int>(cl)") {
      be::util::ChunkedList<int> con(cl);

      REQUIRE(con.size() == cl.size());

      for (size_t i = 0; i < cl.size(); ++i) {
         REQUIRE(con[i] == cl[i]);
      }

      SECTION("Modifying original array does not change new array") {
         cl.clear();

         REQUIRE(cl.size() == 0);
         REQUIRE(con.size() == 30);

         for (size_t i = 0; i < con.size(); ++i) {
            REQUIRE(con[i] == (int)i);
         }
      }
   }

   cl.clear();

   REQUIRE(cl.size() == 0);

   SECTION("util::ChunkedList<int>(cl) (empty)") {
      be::util::ChunkedList<int> con(cl);

      REQUIRE(con.size() == 0);
   }
}

TEST_CASE("util::ChunkedList move construction", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> cl;
   for (int i = 0; i < 30; ++i) {
      cl.push_back(i);
   }

   REQUIRE(cl.size() == 30);

   SECTION("util::ChunkedList<int>(std::move(cl))") {
      be::util::ChunkedList<int> con(std::move(cl));

      REQUIRE(cl.size() == 0);
      REQUIRE(con.size() == 30);

      for (size_t i = 0; i < con.size(); ++i) {
         REQUIRE(con[i] == (int)i);
      }
   }
}

TEST_CASE("util::ChunkedList initializer-list construction", BE_CATCH_TAGS) {
   SECTION("util::ChunkedList<int> con = {}") {
      be::util::ChunkedList<int> con = { };

      REQUIRE(con.size() == 0);
   }

   SECTION("util::ChunkedList<int> con = { 1, 2, 3, 4 }") {
      be::util::ChunkedList<int> con = { 1, 2, 3, 4 };

      REQUIRE(con.size() == 4);
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 3);
      REQUIRE(con[3] == 4);
   }

   SECTION("util::ChunkedList<int> con { 1, 2, 3, 4 }") {
      be::util::ChunkedList<int> con { 1, 2, 3, 4 };

      REQUIRE(con.size() == 4);
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 3);
      REQUIRE(con[3] == 4);
   }
}

TEST_CASE("util::ChunkedList copy-assignment", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> cl = { 3, 1, 4, 1, 5, 9 };

   SECTION("copying to empty container") {
      be::util::ChunkedList<int> con;

      REQUIRE(con.size() == 0);

      con = cl;

      REQUIRE(con.size() == cl.size());
      REQUIRE(con.size() == 6);

      for (int i = 0; i < 6; ++i) {
         REQUIRE(cl[i] == con[i]);
      }
   }
   
   SECTION("copying to smaller container") {
      be::util::ChunkedList<int> con = { 1, 2, 3 };

      REQUIRE(con.size() == 3);

      con = cl;

      REQUIRE(con.size() == cl.size());
      REQUIRE(con.size() == 6);

      for (int i = 0; i < 6; ++i) {
         REQUIRE(cl[i] == con[i]);
      }
   }

   SECTION("copying to larger container") {
      be::util::ChunkedList<int> con = { 1, 2, 3, 4, 3, 2, 1, 2, 3, 4 };

      REQUIRE(con.size() == 10);

      con = cl;

      REQUIRE(con.size() == cl.size());
      REQUIRE(con.size() == 6);

      for (int i = 0; i < 6; ++i) {
         REQUIRE(cl[i] == con[i]);
      }
   }
}

TEST_CASE("util::ChunkedList move-assignment", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> cl = { 3, 1, 4, 1, 5, 9 };

   SECTION("moving to empty container") {
      be::util::ChunkedList<int> con;

      REQUIRE(con.size() == 0);

      con = std::move(cl);

      REQUIRE(con.size() == 6);
      REQUIRE(cl.size() == 0);

      REQUIRE(con.front() == 3);
      REQUIRE(con[1] == 1);
      REQUIRE(con.back() == 9);
   }

   SECTION("moving to smaller container") {
      be::util::ChunkedList<int> con = { 1, 2, 3 };

      REQUIRE(con.size() == 3);

      con = std::move(cl);

      REQUIRE(con.size() == 6);
      REQUIRE(con.front() == 3);
      REQUIRE(con[1] == 1);
      REQUIRE(con.back() == 9);
      REQUIRE(con.size() != cl.size());
   }

   SECTION("moving to larger container") {
      be::util::ChunkedList<int> con = { 1, 2, 3, 4, 3, 2, 1, 2, 3, 4 };

      REQUIRE(con.size() == 10);

      con = std::move(cl);

      REQUIRE(con.size() == 6);
      REQUIRE(con.front() == 3);
      REQUIRE(con[1] == 1);
      REQUIRE(con.back() == 9);
      REQUIRE(con.size() != cl.size());
   }
}

TEST_CASE("util::ChunkedList initializer-list assignment", BE_CATCH_TAGS) {
   SECTION("moving to empty container") {
      be::util::ChunkedList<int> con;

      REQUIRE(con.size() == 0);

      con = { 3, 1, 4, 1, 5, 9 };

      REQUIRE(con.size() == 6);

      REQUIRE(con.front() == 3);
      REQUIRE(con[1] == 1);
      REQUIRE(con.back() == 9);
   }

   SECTION("moving to smaller container") {
      be::util::ChunkedList<int> con = { 1, 2, 3 };

      REQUIRE(con.size() == 3);

      con = { 3, 1, 4, 1, 5, 9 };

      REQUIRE(con.size() == 6);
      REQUIRE(con.front() == 3);
      REQUIRE(con[1] == 1);
      REQUIRE(con.back() == 9);
   }

   SECTION("moving to larger container") {
      be::util::ChunkedList<int> con = { 1, 2, 3, 4, 3, 2, 1, 2, 3, 4 };

      REQUIRE(con.size() == 10);

      con = { 3, 1, 4, 1, 5, 9 };

      REQUIRE(con.size() == 6);
      REQUIRE(con.front() == 3);
      REQUIRE(con[1] == 1);
      REQUIRE(con.back() == 9);
   }
}

TEST_CASE("util::ChunkedList 'push back N copy constructed elements' assign", BE_CATCH_TAGS) {
   SECTION("assign(1, 7)") {
      be::util::ChunkedList<int> con;
      
      con.assign(1, 7);

      REQUIRE(con.size() == 1);
      REQUIRE(con.front() == 7);

      con.assign(0, 7);

      REQUIRE(con.size() == 0);
      REQUIRE(con.begin() == con.end());
      REQUIRE(con.empty());
   }

   SECTION("assign(0, 7)") {
      be::util::ChunkedList<int> con;
      con.assign(0, 7);
      REQUIRE(con.size() == 0);
      REQUIRE(con.begin() == con.end());
      REQUIRE(con.empty());
   }

   SECTION("assign(2, 7)") {
      be::util::ChunkedList<int> con;
      
      con.assign(2, 7);
      REQUIRE(con.size() == 2);
      REQUIRE(con.front() == 7);
      REQUIRE(con.back() == 7);
   }

   SECTION("assign(50, 7)") {
      be::util::ChunkedList<int> con;
      
      con.assign(50, 7);
      REQUIRE(con.size() == 50);

      for (int i : con)
         REQUIRE(i == 7);

      con.assign(0, 7);

      REQUIRE(con.size() == 0);
      REQUIRE(con.begin() == con.end());
      REQUIRE(con.empty());
   }
}

TEST_CASE("util::ChunkedList iterator copy assign", BE_CATCH_TAGS) {
   std::vector<int> vec { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24 };
   be::util::ChunkedList<int> cl(20, 47);

   SECTION("assign(vec.begin(), vec.end())") {
      be::util::ChunkedList<int> con;

      con.assign(vec.begin(), vec.end());

      REQUIRE(con.size() == vec.size());

      auto vi = vec.begin();
      auto ci = con.begin();

      while (vi != vec.end() && ci != con.end()) {
         REQUIRE(*vi == *ci);

         ++vi;
         ++ci;
      }

      REQUIRE(vi == vec.end());
      REQUIRE(ci == con.end());
   }

   SECTION("assign(vec.begin(), vec.begin() + 2)") {
      be::util::ChunkedList<int> con;
      
      con.assign(vec.begin(), vec.begin() + 2);

      REQUIRE(con.size() == 2);
      REQUIRE(con.front() == vec.front());
   }

   SECTION("assign(vec.begin(), vec.begin())") {
      be::util::ChunkedList<int> con;
      
      con.assign(vec.begin(), vec.begin());

      REQUIRE(con.size() == 0);
   }

   SECTION("assign(cl.begin(), cl.end())") {
      be::util::ChunkedList<int> con;
      
      con.assign(cl.begin(), cl.end());
      REQUIRE(con.size() == 20);
      REQUIRE(con.front() == 47);
      REQUIRE(con.back() == 47);
   }

   SECTION("assign(int*, int*)") {
      int a[2] = { 7, 47 };

      be::util::ChunkedList<int> con;
      
      con.assign(a + 0, a + 1);
      REQUIRE(con.size() == 1);
      REQUIRE(con.front() == 7);
      REQUIRE(con.back() == 7);
   }

   SECTION("assign(std::istream_iterator<int>, std::istream_iterator<int>)") {
      std::istringstream iss("1 2 3 4 5 6 7");

      be::util::ChunkedList<int> con;
      
      con.assign(std::istream_iterator<int>(iss), std::istream_iterator<int>()); // these are input_iterators rather than forward_iterators (different codepath)
      REQUIRE(con.size() == 7);
      REQUIRE(con.front() == 1);
      REQUIRE(con.back() == 7);
   }
}

TEST_CASE("util::ChunkedList initializer-list assign", BE_CATCH_TAGS) {
   SECTION("util::ChunkedList<int> con = {}") {
      be::util::ChunkedList<int> con { 1, 2 };
      
      con = { };

      REQUIRE(con.size() == 0);
   }

   SECTION("util::ChunkedList<int> con= { 1, 2, 3, 4 }") {
      be::util::ChunkedList<int> con;
      
      con = { 1, 2, 3, 4 };

      REQUIRE(con.size() == 4);
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 3);
      REQUIRE(con[3] == 4);
   }

   SECTION("util::ChunkedList<int> con { 1, 2, 3, 4 }") {
      be::util::ChunkedList<int> con { 22, 2 };
      
      con = { 1, 2, 3, 4 };

      REQUIRE(con.size() == 4);
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 3);
      REQUIRE(con[3] == 4);
   }
}

TEST_CASE("util::ChunkedList at()", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> con { 3,1,4,1,5,9,2,6,5 };

   SECTION("read access") {
      REQUIRE(con.at(0) == 3);
      REQUIRE(con.at(1) == 1);
      REQUIRE(con.at(2) == 4);
      REQUIRE(con.at(3) == 1);
      REQUIRE(con.at(4) == 5);
      REQUIRE(con.at(5) == 9);
      REQUIRE(con.at(6) == 2);
      REQUIRE(con.at(7) == 6);
      REQUIRE(con.at(8) == 5);
   }

   SECTION("OOB throws exception") {
      REQUIRE_THROWS_AS(con.at(9), std::out_of_range);
      REQUIRE_THROWS_AS(con.at((size_t) - 1), std::out_of_range);
   }

   SECTION("write access") {
      con.at(1) = 2;
      using std::swap;
      swap(con.at(0), con.at(4));

      REQUIRE(con.at(0) == 5);
      REQUIRE(con.at(1) == 2);
      REQUIRE(con.at(2) == 4);
      REQUIRE(con.at(3) == 1);
      REQUIRE(con.at(4) == 3);
      REQUIRE(con.at(5) == 9);
      REQUIRE(con.at(6) == 2);
      REQUIRE(con.at(7) == 6);
      REQUIRE(con.at(8) == 5);
   }
}

TEST_CASE("util::ChunkedList operator[]", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> con { 3,1,4,1,5,9,2,6,5 };

   SECTION("read access") {
      REQUIRE(con[0] == 3);
      REQUIRE(con[1] == 1);
      REQUIRE(con[2] == 4);
      REQUIRE(con[3] == 1);
      REQUIRE(con[4] == 5);
      REQUIRE(con[5] == 9);
      REQUIRE(con[6] == 2);
      REQUIRE(con[7] == 6);
      REQUIRE(con[8] == 5);
   }

   SECTION("write access") {
      con[1] = 2;
      using std::swap;
      swap(con[0], con[4]);

      REQUIRE(con[0] == 5);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 4);
      REQUIRE(con[3] == 1);
      REQUIRE(con[4] == 3);
      REQUIRE(con[5] == 9);
      REQUIRE(con[6] == 2);
      REQUIRE(con[7] == 6);
      REQUIRE(con[8] == 5);
   }
}

TEST_CASE("util::ChunkedList front()/back()", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> con { 3,1,4,1,5,9,2,6,5 };

   SECTION("read access") {
      REQUIRE(con.front() == 3);
      REQUIRE(con.back() == 5);
   }

   SECTION("write access") {
      con.front() = 1337;
      con.back() = 0;

      REQUIRE(con[0] == 1337);
      REQUIRE(con.front() == 1337);

      REQUIRE(con[8] == 0);
      REQUIRE(con.back() == 0);
   }
}

TEST_CASE("util::ChunkedList iterators", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> con { 3,1,4,1,5,9,2,6,5 };

   SECTION("increment") {
      int i = 0;
      for (auto it = con.begin(), end = con.end(); it != end; ++it) {
         REQUIRE(*it == con[i]);
         ++i;
      }

      REQUIRE(i == (int)con.size());
   }

   SECTION("decrement") {
      int i = 0;
      for (auto it = con.end(), end = con.begin(); it != end; --it) {
         ++i;
      }

      REQUIRE(i == (int)con.size());
   }

   SECTION("postincrement") {
      auto i = con.begin();

      REQUIRE(i++ == con.begin());
      REQUIRE(i == ++con.begin());
   }

   SECTION("postdecrement") {
      auto i = ++con.begin();

      REQUIRE(i-- == ++con.begin());
      REQUIRE(i == con.begin());
   }

   SECTION("operator +=") {
      auto i = con.begin();
      auto j = i;
      j += 3;
      ++i;
      ++i;
      ++i;

      REQUIRE(i == j);
      REQUIRE(*i == con[3]);

      *i = 0;

      REQUIRE(con[3] == 0);
   }

   SECTION("operator +") {
      auto i = con.begin();
      auto j = i + 3;
      i += 3;

      REQUIRE(i == j);
      REQUIRE(*i == con[3]);
   }

   SECTION("operator -=") {
      auto i = con.begin();
      auto j = i;
      j += 3;
      j -= 3;

      REQUIRE(i == j);
      REQUIRE(*i == con[0]);
   }

   SECTION("operator -") {
      auto i = con.begin() + 3;
      auto j = i - 3;

      REQUIRE(j == con.begin());
      REQUIRE(*j == con[0]);
   }

   SECTION("operator - (iter, iter)") {
      auto i = con.begin();
      auto j = con.end();

      using difft = be::util::ChunkedList<int>::difference_type;

      REQUIRE((j - i) == (difft)con.size());
   }

   SECTION("operator[]") {
      auto i = con.begin();

      REQUIRE(i[0] == con.front());
      REQUIRE(i[4] == con[4]);

      i[4] = 0;

      REQUIRE(con[4] == 0);
   }
}

TEST_CASE("util::ChunkedList reverse iterators", BE_CATCH_TAGS) {
   be::util::ChunkedList<int> con { 3,1,4,1,5,9,2,6,5 };

   std::size_t i = con.size() - 1;
   for (auto it = con.rbegin(), end = con.rend(); it != end; ++it) {
      REQUIRE(con[i] == *it);
      --i;
   }
}

TEST_CASE("util::ChunkedList insert()", BE_CATCH_TAGS) {
   SECTION("const_iterator pos, const value_type& value") {
      be::util::ChunkedList<int> con { 1,2,3,4 };

      int five = 5;
      con.insert(con.begin() + 2, five);
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 5);
      REQUIRE(con[3] == 3);
      REQUIRE(con[4] == 4);
   }

   SECTION("const_iterator pos, const value_type& value") {
      be::util::ChunkedList<std::unique_ptr<int>> con;
      con.push_back(std::unique_ptr<int>(new int(1)));
      con.push_back(std::unique_ptr<int>(new int(2)));
      con.push_back(std::unique_ptr<int>(new int(3)));
      con.push_back(std::unique_ptr<int>(new int(4)));

      
      con.insert(con.begin() + 2, std::unique_ptr<int>(new int(5)));
      REQUIRE(*con[0] == 1);
      REQUIRE(*con[1] == 2);
      REQUIRE(*con[2] == 5);
      REQUIRE(*con[3] == 3);
      REQUIRE(*con[4] == 4);
   }
   
   SECTION("const_iterator pos, size_type count, const value_type& value") {
      be::util::ChunkedList<int> con { 1,2,3,4 };

      int five = 5;
      con.insert(con.begin() + 2, 3, five);
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 5);
      REQUIRE(con[3] == 5);
      REQUIRE(con[4] == 5);
      REQUIRE(con[5] == 3);
      REQUIRE(con[6] == 4);
   }

   SECTION("const_iterator pos, I first, I last") {
      be::util::ChunkedList<int> con { 1,2,3,4 };
      std::vector<int> v { 10, 11, 12, 13 };
      
      con.insert(con.begin() + 2, v.begin(), v.end());
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 10);
      REQUIRE(con[3] == 11);
      REQUIRE(con[4] == 12);
      REQUIRE(con[5] == 13);
      REQUIRE(con[6] == 3);
      REQUIRE(con[7] == 4);
   }

   SECTION("const_iterator pos, initializer_list") {
      be::util::ChunkedList<int> con { 1,2,3,4 };

      con.insert(con.begin() + 2, { 10, 11, 12, 13 });
      REQUIRE(con[0] == 1);
      REQUIRE(con[1] == 2);
      REQUIRE(con[2] == 10);
      REQUIRE(con[3] == 11);
      REQUIRE(con[4] == 12);
      REQUIRE(con[5] == 13);
      REQUIRE(con[6] == 3);
      REQUIRE(con[7] == 4);
   }
}

TEST_CASE("util::ChunkedList emplace()", BE_CATCH_TAGS) {
   be::util::ChunkedList<std::unique_ptr<int>> con;
   con.push_back(std::unique_ptr<int>(new int(1)));
   con.push_back(std::unique_ptr<int>(new int(2)));
   con.push_back(std::unique_ptr<int>(new int(3)));
   con.push_back(std::unique_ptr<int>(new int(4)));

   con.emplace(con.begin() + 2, new int(5));
   REQUIRE(*con[0] == 1);
   REQUIRE(*con[1] == 2);
   REQUIRE(*con[2] == 5);
   REQUIRE(*con[3] == 3);
   REQUIRE(*con[4] == 4);
}

TEST_CASE("util::ChunkedList emplace_back()", BE_CATCH_TAGS) {
   be::util::ChunkedList<std::unique_ptr<int>> con;
   con.push_back(std::unique_ptr<int>(new int(1)));
   con.push_back(std::unique_ptr<int>(new int(2)));
   con.push_back(std::unique_ptr<int>(new int(3)));
   con.push_back(std::unique_ptr<int>(new int(4)));

   con.emplace_back(new int(5));
   REQUIRE(*con[0] == 1);
   REQUIRE(*con[1] == 2);
   REQUIRE(*con[2] == 3);
   REQUIRE(*con[3] == 4);
   REQUIRE(*con[4] == 5);
}

#endif

#if false // WIP


namespace {

#pragma region perf util

template <class T, size_t N>
class perf_test
{
public:
   using t_type = T;
   using X = typename T::value_type;

private:
   be::U32 hexdig(char ch)
   {
      ch = (char)tolower(ch);
      if (ch >= '0' && ch <= '9')
         return ch - '0';

      return ch - 'a';
   }

protected:
   perf_test()
   {
      std::string seq = "65670EA23AA24A0A3EB7178BBB188474CB89137472F8D5ECF709324778732F32AF0EDF940785F509859263CC909CB76ED7B596A5B1D1B7FAEED0D3456816F105";

      be::U32 ssdata[16] = { };
      for (int i = 0; i < 16; ++i)
      {
         for (int j = 0; j < 8; ++j)
         {
            ssdata[i] <<= 4;
            ssdata[i] |= hexdig(seq[i * 8 + j]);
         }
      }

      std::seed_seq ss(std::begin(ssdata), std::end(ssdata));
      prng_.seed(ss);
   }

   void init_(int index, size_t size)
   {
      tcon_[index] = T();

      for (size_t i = 0; i < size; ++i)
         tcon_[index].push_back(prng_());
   }

   T tcon_[N];

   std::mt19937 prng_;

#ifdef _MSC_VER
   void start_()
   {
      QueryPerformanceFrequency(&freq_);
      QueryPerformanceCounter(&st_);
   }

   void stop_()
   {
      QueryPerformanceCounter(&et_);
   }

   double micro_() const
   {
      double micro = 1000000.0 * (et_.QuadPart - st_.QuadPart);
      micro /= freq_.QuadPart;

      return micro;
   }

   LARGE_INTEGER st_, et_, freq_;
#endif
};

template <class I>
double mean(I begin, I end)
{
   // sample size
   int n = std::distance(begin, end);

   // mean
   double u = std::accumulate(begin, end, 0.0);
   u /= n;

   return u;
}

template <class I>
double sstddev(I begin, I end)
{
   // sample size
   int n = std::distance(begin, end);

   // mean
   double u = std::accumulate(begin, end, 0.0);
   u /= n;

   // sample variance
   double v = std::accumulate(begin, end, 0.0, [u](double v, double x) { double delta = x - u; return v + delta * delta; });
   v /= n - 1;

   // sample deviation
   return sqrt(v);
}

#pragma warning(push)
#pragma warning(disable: 4127) // constant conditional

template <size_t N>
struct run_test_helper
{
   template <class... T>
   double operator()(size_t n, std::tuple<T...>& tests)
   {
      return n == N ? std::get<N>(tests).test() : run_test_helper<N - 1>()(n, tests);
   }
};
template <>
struct run_test_helper<0>
{
   template <class... T>
   double operator()(size_t n, std::tuple<T...>& tests)
   {
      return n == 0 ? std::get<0>(tests).test() : 0.0;
   }
};

template <size_t N>
struct test_name_helper
{
   template <class... T>
   std::string operator()(size_t n, std::tuple<T...>& tests)
   {
      return n == N ? typeid(std::get<N>(tests)).name() : test_name_helper<N - 1>()(n, tests);
   }
};
template <>
struct test_name_helper<0>
{
   template <class... T>
   std::string operator()(size_t n, std::tuple<T...>&)
   {
      return n == 0 ? typeid(std::get<0>(std::tuple<T...>())).name() : "[TEST NOT FOUND]";
   }
};

#pragma warning(pop)

struct ptduration
{
   ptduration(double t) : us(t) { }
   double us;
};

std::ostream& operator<<(std::ostream& os, const ptduration& duration)
{
   double t = duration.us;

   std::ostringstream oss;
   oss << std::setprecision(4);
   //if (t > 1000.0)
   //{
   //   oss << (t / 1000.0) << " ms";
   //}
   //else if (t < 1.0)
   //{
   //   oss << (t * 1000.0) << " ns";
   //}
   //else
   //{
   oss << t << " us";
   //}
   return (os << std::setw(10) << oss.str());
}

template <class... T>
std::string run_tests(const std::string& info, int n, std::tuple<T...>&& tests)
{
   std::vector<std::vector<double>> data;

   size_t ntypes = sizeof...(T);

   for (size_t t = 0; t < ntypes; ++t)
   {
      data.emplace_back();
   }

   for (int i = 0; i < n; ++i)
   {
      for (size_t t = 0; t < ntypes; ++t)
      {
         data[t].push_back(run_test_helper<sizeof...(T) - 1>()(t, tests));
      }
   }

   std::ostringstream oss;
   oss << info << " (N = " << n << " tests)\n";

   for (size_t t = 0; t < ntypes; ++t)
   {
      double u = mean(data[t].begin(), data[t].end());
      double sd = sstddev(data[t].begin(), data[t].end());

      oss << std::left
         << "  u = " << ptduration(u)
         << " s = " << ptduration(sd)
         << "  " << test_name_helper<sizeof...(T)-1>()(t, tests)
         << std::endl;
   }
   return oss.str();
}

template <typename T>
std::unique_ptr<T> make_unique(T* ptr)
{
   return std::unique_ptr<T>(ptr);
}

#pragma endregion

template <size_t N, size_t S, class T>
class iter_test : public perf_test<T, N>
{
public:
   iter_test()
   {
      for (int i = 0; i < N; ++i)
      {
         init_(i, S);
      }
   }

   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      X xsum = 0;
      double xtime;

      start_();
      for (int i = 0; i < N; ++i)
      {
         for (auto iter(tcon_[i].begin()), end(tcon_[i].end()); iter != end; ++iter)
            xsum += *iter;
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = xsum;

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class rnd_access_test : public perf_test<T, 1>
{
public:
   rnd_access_test()
   {
      init_(0, S);
   }

   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      X xsum = 0;
      double xtime;

      size_t indices[N];
      for (int i = 0; i < N; ++i)
      {
         indices[i] = prng_() % S;
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         xsum += tcon_[0][indices[i]];
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = xsum;

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class push_test : public perf_test<T, N>
{
public:
   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      double xtime;

      for (int i = 0; i < N; ++i)
      {
         init_(i, 0);
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         for (int j = 0; j < S; ++j)
         {
            tcon_[i].emplace_back();
         }
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = X();

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class push_n_test : public perf_test<T, N>
{
public:
   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      double xtime;

      for (int i = 0; i < N; ++i)
      {
         init_(i, 0);
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         auto& con = tcon_[i];
         con.insert(con.begin(), S, int());
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = X();

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class insert_test : public perf_test<T, N>
{
public:
   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      double xtime;

      for (int i = 0; i < N; ++i)
      {
         init_(i, S);
      }

      size_t indices[S];
      for (int i = 0; i < S; ++i)
      {
         indices[i] = prng_();
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         auto& con = tcon_[i];

         for (int j = 0; j < S; ++j)
         {
            auto iter = con.begin();
            std::advance(iter, (indices[j] % con.size()));
            con.insert(iter, int());
         }
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = X();

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class pop_back_test : public perf_test<T, N>
{
public:
   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      double xtime;

      for (int i = 0; i < N; ++i)
      {
         init_(i, S);
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         auto& con = tcon_[i];

         while (!con.empty())
            con.pop_back();
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = X();

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class sort_test : public perf_test<T, N>
{
public:
   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      double xtime;

      for (int i = 0; i < N; ++i)
      {
         init_(i, S);
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         auto& con = tcon_[i];
         std::sort(con.begin(), con.end());
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = X();

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class list_sort_test : public perf_test<T, N>
{
public:
   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      double xtime;

      for (int i = 0; i < N; ++i)
      {
         init_(i, S);
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         auto& con = tcon_[i];
         con.sort();
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = X();

      return xtime;
   }

};

template <size_t N, size_t S, class T>
class stable_sort_test : public perf_test<T, N>
{
public:
   double test()
   {
      X x;
      return test(&x);
   }

   double test(X* xout)
   {
      double xtime;

      for (int i = 0; i < N; ++i)
      {
         init_(i, S);
      }

      start_();
      for (int i = 0; i < N; ++i)
      {
         auto& con = tcon_[i];
         std::stable_sort(con.begin(), con.end());
      }
      stop_();
      xtime = micro_();

      if (xout)
         *xout = X();

      return xtime;
   }

};

} // ()

TEST_CASE("be::util::ChunkedList iteration performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10")
   {
      std::string msg;
      const size_t n_cons = 1000;
      const size_t con_size = 10;

      auto tuple = make_unique(new std::tuple<
                               iter_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               iter_test<n_cons, con_size, std::vector<int>>,
                               iter_test<n_cons, con_size, std::deque<int>>,
                               iter_test<n_cons, con_size, std::list<int>>,
                               iter_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               iter_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100")
   {
      std::string msg;
      const size_t n_cons = 100;
      const size_t con_size = 100;

      auto tuple = make_unique(new std::tuple<
                               iter_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               iter_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               iter_test<n_cons, con_size, std::vector<int>>,
                               iter_test<n_cons, con_size, std::deque<int>>,
                               iter_test<n_cons, con_size, std::list<int>>,
                               iter_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               iter_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

TEST_CASE("be::util::ChunkedList random access performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10, n_accesses == 100")
   {
      std::string msg;
      const size_t con_size = 10;
      const size_t n_accesses = 100;

      auto tuple = make_unique(new std::tuple<
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, std::vector<int>>,
                               rnd_access_test<n_accesses, con_size, std::deque<int>>,
                               rnd_access_test<n_accesses, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100, n_accesses == 100")
   {
      std::string msg;
      const size_t con_size = 100;
      const size_t n_accesses = 100;

      auto tuple = make_unique(new std::tuple<
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, std::vector<int>>,
                               rnd_access_test<n_accesses, con_size, std::deque<int>>,
                               rnd_access_test<n_accesses, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }


   SECTION("con.size() == 10, n_accesses == 5000")
   {
      std::string msg;
      const size_t con_size = 10;
      const size_t n_accesses = 5000;

      auto tuple = make_unique(new std::tuple<
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, std::vector<int>>,
                               rnd_access_test<n_accesses, con_size, std::deque<int>>,
                               rnd_access_test<n_accesses, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }


   SECTION("con.size() == 100, n_accesses == 5000")
   {
      std::string msg;
      const size_t con_size = 100;
      const size_t n_accesses = 5000;

      auto tuple = make_unique(new std::tuple<
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               rnd_access_test<n_accesses, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               rnd_access_test<n_accesses, con_size, std::vector<int>>,
                               rnd_access_test<n_accesses, con_size, std::deque<int>>,
                               rnd_access_test<n_accesses, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

TEST_CASE("be::util::ChunkedList emplace_back performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10")
   {
      std::string msg;
      const size_t n_cons = 1000;
      const size_t con_size = 10;

      auto tuple = make_unique(new std::tuple<
                               push_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               push_test<n_cons, con_size, std::vector<int>>,
                               push_test<n_cons, con_size, std::deque<int>>,
                               push_test<n_cons, con_size, std::list<int>>,
                               push_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               push_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100")
   {
      std::string msg;
      const size_t n_cons = 100;
      const size_t con_size = 100;

      auto tuple = make_unique(new std::tuple<
                               push_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               push_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               push_test<n_cons, con_size, std::vector<int>>,
                               push_test<n_cons, con_size, std::deque<int>>,
                               push_test<n_cons, con_size, std::list<int>>,
                               push_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               push_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

TEST_CASE("be::util::ChunkedList pushBackN performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10")
   {
      std::string msg;
      const size_t n_cons = 1000;
      const size_t con_size = 10;

      auto tuple = make_unique(new std::tuple<
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               push_n_test<n_cons, con_size, std::vector<int>>,
                               push_n_test<n_cons, con_size, std::deque<int>>,
                               push_n_test<n_cons, con_size, std::list<int>>,
                               push_n_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               push_n_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100")
   {
      std::string msg;
      const size_t n_cons = 100;
      const size_t con_size = 100;

      auto tuple = make_unique(new std::tuple<
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               push_n_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               push_n_test<n_cons, con_size, std::vector<int>>,
                               push_n_test<n_cons, con_size, std::deque<int>>,
                               push_n_test<n_cons, con_size, std::list<int>>,
                               push_n_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               push_n_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

TEST_CASE("be::util::ChunkedList random insert performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10")
   {
      std::string msg;
      const size_t n_cons = 1000;
      const size_t con_size = 10;

      auto tuple = make_unique(new std::tuple<
                               insert_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               insert_test<n_cons, con_size, std::vector<int>>,
                               insert_test<n_cons, con_size, std::deque<int>>,
                               insert_test<n_cons, con_size, std::list<int>>,
                               insert_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               insert_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100")
   {
      std::string msg;
      const size_t n_cons = 100;
      const size_t con_size = 100;

      auto tuple = make_unique(new std::tuple<
                               insert_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               insert_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               insert_test<n_cons, con_size, std::vector<int>>,
                               insert_test<n_cons, con_size, std::deque<int>>,
                               insert_test<n_cons, con_size, std::list<int>>,
                               insert_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               insert_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

TEST_CASE("be::util::ChunkedList pop_back performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10")
   {
      std::string msg;
      const size_t n_cons = 1000;
      const size_t con_size = 10;

      auto tuple = make_unique(new std::tuple<
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               pop_back_test<n_cons, con_size, std::vector<int>>,
                               pop_back_test<n_cons, con_size, std::deque<int>>,
                               pop_back_test<n_cons, con_size, std::list<int>>,
                               pop_back_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               pop_back_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100")
   {
      std::string msg;
      const size_t n_cons = 100;
      const size_t con_size = 100;

      auto tuple = make_unique(new std::tuple<
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               pop_back_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               pop_back_test<n_cons, con_size, std::vector<int>>,
                               pop_back_test<n_cons, con_size, std::deque<int>>,
                               pop_back_test<n_cons, con_size, std::list<int>>,
                               pop_back_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               pop_back_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

TEST_CASE("be::util::ChunkedList sort performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10")
   {
      std::string msg;
      const size_t n_cons = 1000;
      const size_t con_size = 10;

      auto tuple = make_unique(new std::tuple<
                               sort_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               sort_test<n_cons, con_size, std::vector<int>>,
                               sort_test<n_cons, con_size, std::deque<int>>,
                               list_sort_test<n_cons, con_size, std::list<int>>,
                               list_sort_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               sort_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100")
   {
      std::string msg;
      const size_t n_cons = 100;
      const size_t con_size = 100;

      auto tuple = make_unique(new std::tuple<
                               sort_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               sort_test<n_cons, con_size, std::vector<int>>,
                               sort_test<n_cons, con_size, std::deque<int>>,
                               list_sort_test<n_cons, con_size, std::list<int>>,
                               list_sort_test<n_cons, con_size, std::list<int, be::NodeAllocatorLarge<int>>>,
                               sort_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

TEST_CASE("be::util::ChunkedList stable_sort performance comparison", "[be][be::util::ChunkedList][perf]")
{
   SECTION("con.size() == 10")
   {
      std::string msg;
      const size_t n_cons = 1000;
      const size_t con_size = 10;

      auto tuple = make_unique(new std::tuple<
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               stable_sort_test<n_cons, con_size, std::vector<int>>,
                               stable_sort_test<n_cons, con_size, std::deque<int>>,
                               stable_sort_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }

   SECTION("con.size() == 100")
   {
      std::string msg;
      const size_t n_cons = 100;
      const size_t con_size = 100;

      auto tuple = make_unique(new std::tuple<
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 8, 3, 2>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, std::allocator<int>, 4, 4, 1>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::SharedNodeAllocator<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::ThreadNodeAllocator<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocatorLarge<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 8, 3, 2>>,
                               stable_sort_test<n_cons, con_size, be::util::ChunkedList<int, be::NodeAllocator<int>, 4, 4, 1>>,
                               stable_sort_test<n_cons, con_size, std::vector<int>>,
                               stable_sort_test<n_cons, con_size, std::deque<int>>,
                               stable_sort_test<n_cons, con_size, boost::container::stable_vector<int>>
      >());
      msg = run_tests("", 40, std::move(*tuple));
      SUCCEED(msg);
   }
}

#endif

module 'util' {
   lib {
      src {
         'src/*.cpp',
         'src/native/*.cpp',
         'src/native/$(toolchain)/*.cpp',
         pch_src 'src/pch.cpp'
      },
      define 'BE_UTIL_IMPL',
      link_project 'core',
   },
   app '-test' {
      icon 'icon/bengine-test-perf.ico',
      link_project {
         'testing',
         'util'
      }
   },
   app '-perf' {
      icon 'icon/bengine-test-perf.ico',
      link_project {
         'testing',
         'util'
      }
   }
}

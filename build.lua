module 'util' {
   lib {
      src {
         'src/*.cpp',
         pch_src 'src/pch.cpp'
      },
      src {
         'src/native/*.cpp',
         'src/native/$(toolchain)/*.cpp'
      },
      define 'BE_UTIL_IMPL',
      link_project 'core',
   },
   lib '-compression' {
      src {
         'src-compression/*.cpp',
         pch_src 'src-compression/pch.cpp'
      },
      define 'BE_UTIL_COMPRESSION_IMPL',
      link_project 'core'
   },
   lib '-prng' {
      src {
         'src-prng/*.cpp',
         pch_src 'src-prng/pch.cpp'
      },
      define 'BE_UTIL_PRNG_IMPL',
      link_project 'core'
   },
   lib '-string' {
      src {
         'src-string/*.cpp',
         pch_src 'src-string/pch.cpp'
      },
      src {
         'src-string/native/*.cpp',
         'src-string/native/$(toolchain)/*.cpp'
      },
      define 'BE_UTIL_STRING_IMPL',
      link_project 'core'
   },
   lib '-fs' {
      src {
         'src-fs/*.cpp',
         pch_src 'src-fs/pch.cpp'
      },
      src {
         'src-fs/native/*.cpp',
         'src-fs/native/$(toolchain)/*.cpp'
      },
      define 'BE_UTIL_FS_IMPL',
      link_project {
         'util-compression',
         'util-prng'
      }
   },
   lib '-lua' {
      src 'src-lua/*.cpp',
      define 'BE_UTIL_LUA_IMPL',
      link_project {
         'belua',
         'util'
      }
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

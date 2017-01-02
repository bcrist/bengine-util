module { name = 'util',
   projects = {
      lib {
         src = {
            'src/*.cpp',
            'src/native/*.cpp',
            'src/native/' .. toolchain .. '/*.cpp',
            pch = 'src/pch.cpp'
         },
         preprocessor = {
            'BE_UTIL_IMPL'
         }
      },
      app { suffix = 'test',
         icon = 'icon/bengine-test-perf.ico',
         libs = {
            'testing',
            'core',
            'core-id',
            'util'
         },
         src = {
            'test/*.cpp'
         }
      },
      app { suffix = 'perf',
         icon = 'icon/bengine-test-perf.ico',
         libs = {
            'testing',
            'core',
            'core-id',
            'util'
         },
         src = {
            'perf/*.cpp'
         }
      }
   }
}

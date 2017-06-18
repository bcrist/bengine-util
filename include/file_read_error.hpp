#pragma once
#ifndef BE_UTIL_FS_FILE_READ_ERROR_HPP_
#define BE_UTIL_FS_FILE_READ_ERROR_HPP_

#include "util_fs_autolink.hpp"
#include <be/core/enum_traits.hpp>
#include <be/core/exceptions.hpp>

/*!! include 'file_read_error' !! 54 */
/* ################# !! GENERATED CODE -- DO NOT MODIFY !! ################# */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
enum class FileReadError : U8 {
   none = 0,
   unknown_filesystem_error,
   file_not_found,
   file_not_readable,
   out_of_memory
};

bool is_valid(FileReadError constant) noexcept;
const char* file_read_error_name(FileReadError constant) noexcept;
std::array<const FileReadError, 5> file_read_error_values() noexcept;
std::ostream& operator<<(std::ostream& os, FileReadError constant);

} // be::util

namespace be {

///////////////////////////////////////////////////////////////////////////////
template <>
struct EnumTraits<::be::util::FileReadError> {
   using type = ::be::util::FileReadError;
   using underlying_type = typename std::underlying_type<type>::type;

   static constexpr std::size_t count = 5;

   static bool is_valid(type value) {
      return ::be::util::is_valid(value);
   }

   static const char* name(type value) {
      return ::be::util::file_read_error_name(value);
   }

   template <typename C = std::array<const type, count>>
   static C values() {
      return {
         ::be::util::FileReadError::none,
         ::be::util::FileReadError::unknown_filesystem_error,
         ::be::util::FileReadError::file_not_found,
         ::be::util::FileReadError::file_not_readable,
         ::be::util::FileReadError::out_of_memory,
      };
   }
};

} // be

/* ######################### END OF GENERATED CODE ######################### */

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T ignore_error(std::pair<T, FileReadError> result) {
   return std::move(result.first);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T default_on_error(std::pair<T, FileReadError> result, T default_value) {
   if (result.second == FileReadError::none) {
      return std::move(result.first);
   } else {
      return std::move(default_value);
   }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
T throw_on_error(std::pair<T, FileReadError> result) {
   switch (result.second) {
      case FileReadError::none:
         return std::move(result.first);
      default:
         throw Recoverable<>(file_read_error_name(result.second));
   }
}

} // be::util

#endif

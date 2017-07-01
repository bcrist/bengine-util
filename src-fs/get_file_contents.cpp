#include "pch.hpp"
#include "get_file_contents.hpp"
#include <be/core/exceptions.hpp>
#include <fstream>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
S get_file_contents_string(FILE* fd) {
   S data;
   constexpr std::size_t read_size = 4096;
   constexpr std::size_t max_overhead = 64;
   char read_buf[read_size];

   if (0 != std::ferror(fd)) {
      throw RecoverableTrace(std::make_error_code(std::errc::bad_file_descriptor), "Bad FILE* passed to get_file_contents");
   }

   try {
      for (;;) {
         if (0 != std::feof(fd)) {
            break;
         }

         if (0 != std::ferror(fd)) {
            throw RecoverableTrace(std::error_code(errno, std::generic_category()), "Problem checking for end of FILE*");
         }

         std::size_t amount_read = std::fread(read_buf, sizeof(char), read_size, fd);
         if (0 != std::ferror(fd)) {
            throw RecoverableTrace(std::error_code(errno, std::generic_category()), "Problem reading from FILE*");
         }

         if (amount_read == 0) {
            continue;
         }

         if (amount_read < read_size) {
            data.reserve(data.size() + amount_read);
         } else if (data.size() + amount_read > data.capacity()) {
            std::size_t new_size = data.size() + (data.size() >> 1) + (data.size() >> 3) + (read_size << 1);
            // 1.625 * used_size + 2 * read_size
            data.reserve(new_size);
         }
         data.append(read_buf, amount_read);
      }

      if (data.size() > 0 && data.size() + max_overhead < data.capacity()) {
         data.shrink_to_fit();
      }
   } catch (const std::length_error& e) {
      throw RecoverableTrace(std::make_error_code(std::errc::file_too_large), e.what());
   } catch (const std::bad_alloc& e) {
      throw RecoverableTrace(std::make_error_code(std::errc::not_enough_memory), e.what());
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////
S get_file_contents_string(FILE* fd, std::error_code& ec) noexcept {
   S data;
   constexpr std::size_t read_size = 4096;
   constexpr std::size_t max_overhead = 64;
   char read_buf[read_size];

   if (0 != std::ferror(fd)) {
      ec = std::make_error_code(std::errc::bad_file_descriptor);
      return data;
   }

   try {
      for (;;) {
         if (0 != std::feof(fd)) {
            break;
         }

         if (0 != std::ferror(fd)) {
            ec = std::error_code(errno, std::generic_category());
            return data;
         }

         std::size_t amount_read = std::fread(read_buf, sizeof(char), read_size, fd);
         if (0 != std::ferror(fd)) {
            ec = std::error_code(errno, std::generic_category());
            return data;
         }

         if (amount_read == 0) {
            continue;
         }

         if (amount_read < read_size) {
            data.reserve(data.size() + amount_read);
         } else if (data.size() + amount_read > data.capacity()) {
            std::size_t new_size = data.size() + (data.size() >> 1) + (data.size() >> 3) + (read_size << 1);
            // 1.625 * used_size + 2 * read_size
            data.reserve(new_size);
         }
         data.append(read_buf, amount_read);
      }

      if (data.size() > 0 && data.size() + max_overhead < data.capacity()) {
         data.shrink_to_fit();
      }
   } catch (const std::length_error&) {
      ec = std::make_error_code(std::errc::file_too_large);
   } catch (const std::bad_alloc&) {
      ec = std::make_error_code(std::errc::not_enough_memory);
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> get_file_contents_buf(FILE* fd) {
   Buf<UC> data;
   std::size_t used_size = 0;
   constexpr std::size_t read_size = 4096;
   char read_buf[read_size];

   if (0 != std::ferror(fd)) {
      throw RecoverableTrace(std::make_error_code(std::errc::bad_file_descriptor), "Bad FILE* passed to get_file_contents");
   }

   try {
      for (;;) {
         if (0 != std::feof(fd)) {
            break;
         }

         if (0 != std::ferror(fd)) {
            throw RecoverableTrace(std::error_code(errno, std::generic_category()), "Problem checking for end of FILE*");
         }

         std::size_t amount_read = std::fread(read_buf, sizeof(char), read_size, fd);
         if (0 != std::ferror(fd)) {
            throw RecoverableTrace(std::error_code(errno, std::generic_category()), "Problem reading from FILE*");
         }

         if (amount_read == 0) {
            continue;
         }

         if (amount_read < read_size) {
            Buf<UC> new_data = make_buf<UC>(used_size + amount_read);
            if (used_size > 0) {
               memcpy(new_data.get(), data.get(), used_size);
            }
            memcpy(new_data.get() + used_size, read_buf, amount_read);

            data = std::move(new_data);
         } else if (used_size + amount_read <= data.size()) {
            memcpy(data.get() + used_size, read_buf, amount_read);
         } else {
            std::size_t new_size = used_size + (used_size >> 1) + (used_size >> 3) + (read_size << 1);
            // 1.625 * used_size + 2 * read_size
            Buf<UC> new_data = make_buf<UC>(new_size);
            if (used_size > 0) {
               memcpy(new_data.get(), data.get(), used_size);
            }
            memcpy(new_data.get() + used_size, read_buf, amount_read);

            data = std::move(new_data);
         }
         used_size += amount_read;
      }

      if (used_size > 0 && used_size < data.size()) {
         data = copy_buf(sub_buf(data, 0, used_size));
      }
   } catch (const std::bad_alloc& e) {
      throw RecoverableTrace(std::make_error_code(std::errc::not_enough_memory), e.what());
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> get_file_contents_buf(FILE* fd, std::error_code& ec) noexcept {
   Buf<UC> data;
   std::size_t used_size = 0;
   constexpr std::size_t read_size = 4096;
   char read_buf[read_size];

   if (0 != std::ferror(fd)) {
      ec = std::make_error_code(std::errc::bad_file_descriptor);
      return data;
   }

   try {
      for (;;) {
         if (0 != std::feof(fd)) {
            break;
         }

         if (0 != std::ferror(fd)) {
            ec = std::error_code(errno, std::generic_category());

            if (used_size > 0 && used_size < data.size()) {
               data = copy_buf(sub_buf(data, 0, used_size));
            }

            return data;
         }

         std::size_t amount_read = std::fread(read_buf, sizeof(char), read_size, fd);
         if (0 != std::ferror(fd)) {
            ec = std::error_code(errno, std::generic_category());
         }

         if (amount_read == 0) {
            continue;
         }

         if (amount_read < read_size) {
            Buf<UC> new_data = make_buf<UC>(used_size + amount_read);
            if (used_size > 0) {
               memcpy(new_data.get(), data.get(), used_size);
            }
            memcpy(new_data.get() + used_size, read_buf, amount_read);

            data = std::move(new_data);
         } else if (used_size + amount_read <= data.size()) {
            memcpy(data.get() + used_size, read_buf, amount_read);
         } else {
            std::size_t new_size = used_size + (used_size >> 1) + (used_size >> 3) + (read_size << 1);
            // 1.625 * used_size + 2 * read_size
            Buf<UC> new_data = make_buf<UC>(new_size);
            if (used_size > 0) {
               memcpy(new_data.get(), data.get(), used_size);
            }
            memcpy(new_data.get() + used_size, read_buf, amount_read);

            data = std::move(new_data);
         }
         used_size += amount_read;
      }

      if (used_size > 0 && used_size < data.size()) {
         data = copy_buf(sub_buf(data, 0, used_size));
      }
   } catch (const std::bad_alloc&) {
      ec = std::make_error_code(std::errc::not_enough_memory);
      data = Buf<UC>();
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////
S get_file_contents_string(const Path& path) {
   S data;
   
   try {
      if (!fs::exists(path)) {
         throw fs::filesystem_error("File not found", path, std::make_error_code(std::errc::no_such_file_or_directory));
      } else {
         std::ifstream ifs;
         ifs.exceptions(std::ios::badbit | std::ios::failbit);
         ifs.open(path.native(), std::ios::binary);
         ifs.seekg(0, std::ios::end);
         size_t size = (size_t)ifs.tellg();
         data.resize(size);
         ifs.seekg(0, std::ios::beg);
         ifs.read(((char*)(&data[0])), size);
      }
   } catch (const std::ios_base::failure& e) {
      throw fs::filesystem_error(e.what(), path, e.code());
   } catch (const std::length_error& e) {
      throw fs::filesystem_error(e.what(), path, std::make_error_code(std::errc::file_too_large));
   } catch (const std::bad_alloc& e) {
      throw fs::filesystem_error(e.what(), path, std::make_error_code(std::errc::not_enough_memory));
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////
S get_file_contents_string(const Path& path, std::error_code& ec) noexcept {
   S data;

   try {
      if (!fs::exists(path)) {
         ec = std::make_error_code(std::errc::no_such_file_or_directory);
      } else {
         std::ifstream ifs;
         ifs.exceptions(std::ios::badbit | std::ios::failbit);
         ifs.open(path.native(), std::ios::binary);
         ifs.seekg(0, std::ios::end);
         size_t size = (size_t)ifs.tellg();
         data.resize(size);
         ifs.seekg(0, std::ios::beg);
         ifs.read(((char*)(&data[0])), size);
      }
   } catch (const std::ios_base::failure& e) {
      ec = e.code();
      data = S();
   } catch (const std::length_error&) {
      ec = std::make_error_code(std::errc::file_too_large);
      data = S();
   } catch (const std::bad_alloc&) {
      ec = std::make_error_code(std::errc::not_enough_memory);
      data = S();
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> get_file_contents_buf(const Path& path) {
   Buf<UC> data;

   try {
      if (!fs::exists(path)) {
         throw fs::filesystem_error("File not found", path, std::make_error_code(std::errc::no_such_file_or_directory));
      } else {
         std::ifstream ifs;
         ifs.exceptions(std::ios::badbit | std::ios::failbit);
         ifs.open(path.native(), std::ios::binary);
         ifs.seekg(0, std::ios::end);
         size_t size = (size_t)ifs.tellg();
         data = make_buf<UC>(size);
         ifs.seekg(0, std::ios::beg);
         ifs.read(Buf<char>(tmp_buf(data)).get(), size);
      }
   } catch (const std::ios_base::failure& e) {
      throw fs::filesystem_error(e.what(), path, e.code());
   } catch (const std::bad_alloc& e) {
      throw fs::filesystem_error(e.what(), path, std::make_error_code(std::errc::not_enough_memory));
   }

   return data;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> get_file_contents_buf(const Path& path, std::error_code& ec) noexcept {
   Buf<UC> data;

   try {
      if (!fs::exists(path)) {
         ec = std::make_error_code(std::errc::no_such_file_or_directory);
      } else {
         std::ifstream ifs;
         ifs.exceptions(std::ios::badbit | std::ios::failbit);
         ifs.open(path.native(), std::ios::binary);
         ifs.seekg(0, std::ios::end);
         size_t size = (size_t)ifs.tellg();
         data = make_buf<UC>(size);
         ifs.seekg(0, std::ios::beg);
         ifs.read(Buf<char>(tmp_buf(data)).get(), size);
      }
   } catch (const std::ios_base::failure& e) {
      ec = e.code();
      data = Buf<UC>();
   } catch (const std::bad_alloc&) {
      ec = std::make_error_code(std::errc::not_enough_memory);
      data = Buf<UC>();
   }

   return data;
}

} // be::util

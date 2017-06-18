#include "pch.hpp"
#include "files.hpp"
#include <fstream>
#include <sstream>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
std::pair<S, FileReadError> get_file_contents_string(FILE* fd) {
   std::pair<S, FileReadError> result;
   result.second = FileReadError::none;

   try {
      constexpr std::size_t size = 4096;
      char buf[size];
      for (;;) {
         if (0 != std::feof(fd)) {
            break;
         }

         if (0 != std::ferror(fd)) {
            result.second = FileReadError::unknown_filesystem_error;
            break;
         }

         std::size_t read = std::fread(buf, sizeof(char), size, fd);
         result.first.append(buf, read);
      }
   } catch (const std::bad_alloc&) {
      result.second = FileReadError::out_of_memory;
   } catch (const std::length_error&) {
      result.second = FileReadError::out_of_memory;
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////
std::pair<Buf<UC>, FileReadError> get_file_contents_buf(FILE* fd) {
   std::pair<S, FileReadError> result = get_file_contents_string(fd);
   return std::make_pair(copy_buf<UC>(tmp_buf(result.first)), result.second);
}

///////////////////////////////////////////////////////////////////////////////
std::pair<S, FileReadError> get_file_contents_string(const Path& path) {
   std::pair<S, FileReadError> result;
   result.second = FileReadError::none;

   try {
      std::error_code ec;
      if (!fs::exists(path, ec)) {
         if (ec) {
            result.second = FileReadError::unknown_filesystem_error;
         } else {
            result.second = FileReadError::file_not_found;
         }
      } else {
         std::ifstream ifs;
         ifs.exceptions(0);

         ifs.open(path.native(), std::ios::binary);
         if (!ifs) {
            result.second = FileReadError::file_not_readable;
         } else {
            ifs.seekg(0, std::ios::end);
            size_t size = (size_t)ifs.tellg();
            result.first.resize(size);
            ifs.seekg(0, std::ios::beg);
            ifs.read(((char*)(&result.first[0])), size);

            if (!ifs) {
               result.second = FileReadError::unknown_filesystem_error;
            }
         }
      }
   } catch (const std::bad_alloc&) {
      result.second = FileReadError::out_of_memory;
   } catch (const std::length_error&) {
      result.second = FileReadError::out_of_memory;
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////
std::pair<Buf<UC>, FileReadError> get_file_contents_buf(const Path& path) {
   std::pair<Buf<UC>, FileReadError> result;
   result.second = FileReadError::none;

   try {
      std::error_code ec;
      if (!fs::exists(path, ec)) {
         if (ec) {
            result.second = FileReadError::unknown_filesystem_error;
         } else {
            result.second = FileReadError::file_not_found;
         }
      } else {
         std::ifstream ifs;
         ifs.exceptions(0);

         ifs.open(path.native(), std::ios::binary);
         if (!ifs) {
            result.second = FileReadError::file_not_readable;
         } else {
            ifs.seekg(0, std::ios::end);
            size_t size = (size_t)ifs.tellg();
            result.first = make_buf<UC>(size);
            ifs.seekg(0, std::ios::beg);
            ifs.read(Buf<char>(tmp_buf(result.first)).get(), size);

            if (!ifs) {
               result.second = FileReadError::unknown_filesystem_error;
            }
         }
      }
   } catch (const std::bad_alloc&) {
      result.second = FileReadError::out_of_memory;
   } catch (const std::length_error&) {
      result.second = FileReadError::out_of_memory;
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////
FileWriteError put_file_contents(const Path& path, const S& contents) {
   std::error_code ec;
   bool exists = fs::exists(path, ec);
   if (ec) {
      return FileWriteError::unknown_filesystem_error;
   }
   
   if (exists) {
      std::size_t size = fs::file_size(path, ec);
      if (ec) {
         return FileWriteError::unknown_filesystem_error;
      }

      auto space_info = fs::space(path, ec);
      if (ec) {
         return FileWriteError::unknown_filesystem_error;
      }

      if (contents.size() > size) {
         if (contents.size() - size > space_info.free) {
            return FileWriteError::disk_full;
         }
      }
   }

   std::ofstream ofs;
   ofs.exceptions(0);
   ofs.open(path.native(), std::ios::binary | std::ios::trunc);
   if (!ofs) {
      return exists ? FileWriteError::file_not_writable : FileWriteError::failed_to_create_file;
   }

   ofs.write(contents.c_str(), contents.size());

   if (!ofs) {
      return FileWriteError::unknown_filesystem_error;
   }

   return FileWriteError::none;
}

///////////////////////////////////////////////////////////////////////////////
FileWriteError put_file_contents(const Path& path, const Buf<const UC>& contents) {
   std::error_code ec;
   bool exists = fs::exists(path, ec);
   if (ec) {
      return FileWriteError::unknown_filesystem_error;
   }

   if (exists) {
      std::size_t size = fs::file_size(path, ec);
      if (ec) {
         return FileWriteError::unknown_filesystem_error;
      }

      auto space_info = fs::space(path, ec);
      if (ec) {
         return FileWriteError::unknown_filesystem_error;
      }

      if (contents.size() > size) {
         if (contents.size() - size > space_info.free) {
            return FileWriteError::disk_full;
         }
      }
   }

   std::ofstream ofs;
   ofs.exceptions(0);
   ofs.open(path.native(), std::ios::binary | std::ios::trunc);
   if (!ofs) {
      return exists ? FileWriteError::file_not_writable : FileWriteError::failed_to_create_file;
   }

   ofs.write(Buf<const char>(tmp_buf(contents)).get(), contents.size());

   if (!ofs) {
      return FileWriteError::unknown_filesystem_error;
   }

   return FileWriteError::none;
}

} // be::util

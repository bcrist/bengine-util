#include "pch.hpp"
#include "put_file_contents.hpp"
#include <fstream>
#include <sstream>

namespace be::util {

///////////////////////////////////////////////////////////////////////////////
void put_file_contents(const Path& path, const S& contents) {
   std::size_t existing_size = 0;
   if (fs::exists(path)) {
      existing_size = fs::file_size(path);
   }

   auto space_info = fs::space(path);
   if (contents.size() > existing_size) {
      if (contents.size() - existing_size > space_info.free) {
         throw fs::filesystem_error("Not enough free disk space to save file", path, std::make_error_code(std::errc::no_space_on_device));
      }
   }

   try {
      std::ofstream ofs;
      ofs.exceptions(std::ios::failbit | std::ios::badbit);
      ofs.open(path.native(), std::ios::binary | std::ios::trunc);
      ofs.write(contents.c_str(), contents.size());
   } catch (const std::ios_base::failure& e) {
      throw fs::filesystem_error(e.what(), path, e.code());
   }
}

///////////////////////////////////////////////////////////////////////////////
void put_file_contents(const Path& path, const S& contents, std::error_code& ec) noexcept {
   std::size_t existing_size = 0;
   auto space_info = fs::space(path, ec);
   if (fs::exists(path, ec)) {
      existing_size = fs::file_size(path, ec);
   }

   if (!ec) {
      if (contents.size() > existing_size) {
         if (contents.size() - existing_size > space_info.free) {
            ec = std::make_error_code(std::errc::no_space_on_device);
            return;
         }
      }

      try {
         std::ofstream ofs;
         ofs.exceptions(std::ios::failbit | std::ios::badbit);
         ofs.open(path.native(), std::ios::binary | std::ios::trunc);
         ofs.write(contents.c_str(), contents.size());
      } catch (const std::ios_base::failure& e) {
         ec = e.code();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void put_file_contents(const Path& path, const Buf<const UC>& contents) {
   std::size_t existing_size = 0;
   if (fs::exists(path)) {
      existing_size = fs::file_size(path);
   }

   auto space_info = fs::space(path);
   if (contents.size() > existing_size) {
      if (contents.size() - existing_size > space_info.free) {
         throw fs::filesystem_error("Not enough free disk space to save file", path, std::make_error_code(std::errc::no_space_on_device));
      }
   }

   try {
      std::ofstream ofs;
      ofs.exceptions(std::ios::failbit | std::ios::badbit);
      ofs.open(path.native(), std::ios::binary | std::ios::trunc);
      ofs.write(Buf<const char>(tmp_buf(contents)).get(), contents.size());
   } catch (const std::ios_base::failure& e) {
      throw fs::filesystem_error(e.what(), path, e.code());
   }
}

///////////////////////////////////////////////////////////////////////////////
void put_file_contents(const Path& path, const Buf<const UC>& contents, std::error_code& ec) noexcept {
   std::size_t existing_size = 0;
   auto space_info = fs::space(path, ec);
   if (fs::exists(path, ec)) {
      existing_size = fs::file_size(path, ec);
   }

   if (!ec) {
      if (contents.size() > existing_size) {
         if (contents.size() - existing_size > space_info.free) {
            ec = std::make_error_code(std::errc::no_space_on_device);
            return;
         }
      }

      try {
         std::ofstream ofs;
         ofs.exceptions(std::ios::failbit | std::ios::badbit);
         ofs.open(path.native(), std::ios::binary | std::ios::trunc);
         ofs.write(Buf<const char>(tmp_buf(contents)).get(), contents.size());
      } catch (const std::ios_base::failure& e) {
         ec = e.code();
      }
   }
}

} // be::util

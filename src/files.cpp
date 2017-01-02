#include "pch.hpp"
#include "files.hpp"
#include <fstream>
#include <sstream>

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
S get_file_contents_string(FILE* fd) {
   std::ifstream ifs(fd);
   if (ifs.fail() || ifs.bad()) {
      throw std::ios::failure("Could not open file descriptor!");
   }

   std::ostringstream oss;
   oss << ifs.rdbuf();

   if (ifs.fail() || ifs.bad()) {
      throw std::ios::failure("Error while reading from file descriptor!");
   }

   return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> get_file_contents_buf(FILE* fd) {
   return copy_buf<UC>(get_file_contents_string(fd));
}

///////////////////////////////////////////////////////////////////////////////
S get_file_contents_string(const Path& path) {
   S retval;
   std::ifstream ifs;
   ifs.open(path.native(), std::ios::binary);
   if (!ifs) {
      throw std::ios::failure("Could not open file!");
   }

   //std::ostringstream oss;
   //oss << ifs.rdbuf();
   //return oss.str();

   ifs.seekg(0, std::ios::end);
   size_t size = (size_t)ifs.tellg();
   retval.resize(size);
   ifs.seekg(0, std::ios::beg);
   ifs.read(((char*)(&retval[0])), size);

   if (ifs.fail() || ifs.bad()) {
      throw std::ios::failure("Error while reading from file!");
   }

   return retval;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> get_file_contents_buf(const Path& path) {
   Buf<UC> retval;
   std::ifstream ifs;
   ifs.open(path.native(), std::ios::binary);
   if (!ifs) {
      throw std::ios::failure("Could not open file!");
   }

   //std::ostringstream oss;
   //oss << ifs.rdbuf();
   //return oss.str();

   ifs.seekg(0, std::ios::end);
   size_t size = (size_t)ifs.tellg();
   retval = make_buf<UC>(size);
   ifs.seekg(0, std::ios::beg);
   ifs.read(tmp_buf<char>(retval).get(), size);

   if (ifs.fail() || ifs.bad()) {
      throw std::ios::failure("Error while reading from file!");
   }

   return retval;
}

///////////////////////////////////////////////////////////////////////////////
void put_file_contents(const Path& path, const S& contents) {
   std::ofstream ofs;
   ofs.open(path.native(), std::ios::binary | std::ios::trunc);
   if (ofs.fail() || ofs.bad()) {
      throw std::ios::failure("Could not open file for writing!");
   }

   ofs.write(contents.c_str(), contents.size());

   if (ofs.fail() || ofs.bad()) {
      throw std::ios::failure("Error while writing to file!");
   }
}

///////////////////////////////////////////////////////////////////////////////
void put_file_contents(const Path& path, const Buf<const UC>& contents) {
   std::ofstream ofs;
   ofs.open(path.native(), std::ios::binary | std::ios::trunc);
   if (ofs.fail() || ofs.bad()) {
      throw std::ios::failure("Could not open file for writing!");
   }

   ofs.write(tmp_buf<const char>(contents).get(), contents.size());

   if (ofs.fail() || ofs.bad()) {
      throw std::ios::failure("Error while writing to file!");
   }
}

} // be::util
} // be

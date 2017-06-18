#include "pch.hpp"
#include "zlib.hpp"
#include <be/core/byte_order.hpp>
#include <zlib/zlib.h>

namespace be::util {
namespace {

///////////////////////////////////////////////////////////////////////////////
/// type used to prefix uncompressed length to compressed data.
using L = U32;

///////////////////////////////////////////////////////////////////////////////
Buf<UC> deflate(const UC* uncompressed, std::size_t uncompressed_size, bool encode_length, I8 level) {
   assert(static_cast<std::size_t>(static_cast<L>(uncompressed_size)) == uncompressed_size);
   assert(static_cast<std::size_t>(static_cast<unsigned long>(uncompressed_size)) == uncompressed_size);

   unsigned long uc_size = static_cast<unsigned long>(uncompressed_size);
   unsigned long buffer_size = compressBound(uc_size);
   unsigned long compressed_size = buffer_size;
   unsigned long buffer_offset = encode_length ? static_cast<unsigned long>(sizeof(L)) : 0;
   buffer_size += buffer_offset;

   Buf<UC> buffer = make_buf<UC>(buffer_size);

   UC* buf = buffer.get() + buffer_offset;
   int result = compress2(buf, &compressed_size, uncompressed, uc_size, level);

   if (result != Z_OK) {
      if (result == Z_MEM_ERROR)
         throw std::bad_alloc();
      else if (result == Z_BUF_ERROR)
         throw std::length_error("Provided buffer is too small to contain all zlib data!");
      else if (result == Z_STREAM_ERROR)
         throw std::length_error("Invalid compression level specified!");

      throw std::runtime_error("compress2() returned an unrecognized status code!");
   }

   buffer_size = compressed_size;
   if (encode_length) {
      buffer_size += sizeof(L);
      L size = bo::to_net(static_cast<L>(uncompressed_size));
      memcpy(buffer.get(), &size, sizeof(L));
   }
   
   std::size_t final_size = static_cast<std::size_t>(buffer_size);
   if (buffer.size() > final_size + 100 && buffer.size() > final_size * 9 / 8) {
      buffer = copy_buf(sub_buf(buffer, 0, final_size));
   } else if (buffer.size() != final_size) {
      buffer.release();
      buffer = Buf<UC>(buffer.get(), final_size, detail::delete_array);
   }

   return buffer;
}

///////////////////////////////////////////////////////////////////////////////
std::size_t get_uncompressed_length(const UC* compressed, std::size_t compressed_size) {
   if (compressed_size < sizeof(L))
      throw std::length_error("Not enough data!");

   // first 4 bytes are big-endian uint32 defining uncompressed size.
   // save it to a variable, then advance compressed_source so it points
   // to the first byte after it, and update compressed_size.
   L uncompressed_size;
   memcpy(&uncompressed_size, compressed, sizeof(L));
   return bo::to_host(uncompressed_size);
}

///////////////////////////////////////////////////////////////////////////////
std::size_t inflate(const UC* compressed, std::size_t compressed_size, UC* uncompressed, std::size_t uncompressed_size) {
   auto uc_size = static_cast<unsigned long>(uncompressed_size);
   int result = uncompress(uncompressed, &uc_size, compressed, static_cast<unsigned long>(compressed_size));

   if (result == Z_OK)
      return static_cast<std::size_t>(uc_size);
   else if (result == Z_DATA_ERROR)
      throw std::runtime_error("Invalid compressed zlib data!");
   else if (result == Z_BUF_ERROR)
      throw std::length_error("Provided buffer is too small to contain all zlib data!");
   else if (result == Z_MEM_ERROR)
      throw std::bad_alloc();

   throw std::runtime_error("uncompress() returned an unrecognized status code!");
}

} // be::()

///////////////////////////////////////////////////////////////////////////////
/// \note   The returned buffer will be allocated for a larger size than its
///         size() accessor reports.
Buf<UC> deflate_text(const S& text, bool encode_length, I8 level) {
   return deflate(static_cast<const UC*>(static_cast<const void*>(text.c_str())),
                  text.size(), encode_length, level);
}

///////////////////////////////////////////////////////////////////////////////
/// \note   The returned buffer will be allocated for a larger size than its
///         size() accessor reports.
Buf<UC> deflate_blob(const Buf<const UC>& data, bool encode_length, I8 level) {
   return deflate(data.get(), data.size(), encode_length, level);
}

///////////////////////////////////////////////////////////////////////////////
S inflate_text(const Buf<const UC>& compressed) {
   S uncompressed;
   std::size_t uncompressed_length = get_uncompressed_length(compressed.get(), compressed.size());
   uncompressed.resize(uncompressed_length);

   uncompressed_length = inflate(compressed.get() + sizeof(L), compressed.size() - sizeof(L),
                                 static_cast<unsigned char*>(static_cast<void*>(&uncompressed[0])), uncompressed_length);

   uncompressed.resize(uncompressed_length);
   return uncompressed;
}

///////////////////////////////////////////////////////////////////////////////
S inflate_text(const Buf<const UC>& compressed, std::size_t uncompressed_length) {
   S uncompressed;
   uncompressed.resize(uncompressed_length);
   uncompressed_length = inflate(compressed.get(), compressed.size(),
                                 static_cast<unsigned char*>(static_cast<void*>(&uncompressed[0])), uncompressed_length);

   uncompressed.resize(uncompressed_length);
   return uncompressed;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> inflate_blob(const Buf<const UC>& compressed) {
   Buf<UC> uncompressed;
   std::size_t uncompressed_length = get_uncompressed_length(compressed.get(), compressed.size());
   uncompressed = make_buf<UC>(uncompressed_length);

   uncompressed_length = inflate(compressed.get() + sizeof(L), compressed.size() - sizeof(L),
                                 uncompressed.get(), uncompressed.size());

   
   if (uncompressed.size() != uncompressed_length) {
      if (uncompressed.size() > uncompressed_length + 100 && uncompressed.size() > uncompressed_length * 9 / 8) {
         uncompressed = copy_buf(sub_buf(uncompressed, 0, uncompressed_length));
      } else {
         uncompressed.release();
         uncompressed = Buf<UC>(uncompressed.get(), uncompressed_length, detail::delete_array);
      }
   }

   return uncompressed;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> inflate_blob(const Buf<const UC>& compressed, std::size_t uncompressed_length) {
   Buf<UC> uncompressed = make_buf<UC>(uncompressed_length);

   uncompressed_length = inflate(compressed.get(), compressed.size(),
                                 uncompressed.get(), uncompressed.size());

   if (uncompressed.size() != uncompressed_length) {
      if (uncompressed.size() > uncompressed_length + 100 && uncompressed.size() > uncompressed_length * 9 / 8) {
         uncompressed = copy_buf(sub_buf(uncompressed, 0, uncompressed_length));
      } else {
         uncompressed.release();
         uncompressed = Buf<UC>(uncompressed.get(), uncompressed_length, detail::delete_array);
      }
   }

   return uncompressed;
}

} // be::util

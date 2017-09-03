#include "pch.hpp"
#include "zlib.hpp"
#include "zlib_result_code.hpp"
#include <be/core/byte_order.hpp>
#include <be/core/exceptions.hpp>
#include <zlib/zlib.h>

namespace be::util {
namespace {

///////////////////////////////////////////////////////////////////////////////
/// type used to prefix uncompressed length to compressed data.
using L = U64;

///////////////////////////////////////////////////////////////////////////////
std::size_t deflate_bound(std::size_t uncompressed_size, bool encode_length) {
   return uncompressed_size + (uncompressed_size >> 12) + (uncompressed_size >> 14) + (uncompressed_size >> 25) + 13 +
      (encode_length ? sizeof(L) : 0);
}

///////////////////////////////////////////////////////////////////////////////
void* zlib_alloc(void*, uInt items, uInt size) {
   return std::malloc(std::size_t(items) * std::size_t(size));
}

///////////////////////////////////////////////////////////////////////////////
void zlib_free(void*, void* ptr) {
   std::free(ptr);
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> deflate(const UC* uncompressed, std::size_t uncompressed_size, bool encode_length, I8 level, std::error_code& ec) noexcept {
   Buf<UC> buffer;
   try {
      buffer = make_buf<UC>(deflate_bound(uncompressed_size, encode_length));
   } catch (const std::bad_alloc&) {
      ec = ZlibResultCode::not_enough_memory;
      return buffer;
   }

   const UC* in = uncompressed;
   std::size_t in_remaining = uncompressed_size;

   UC* out = buffer.get();
   std::size_t out_remaining = buffer.size();

   if (encode_length) {
      out += sizeof(L);
      out_remaining -= sizeof(L);
   }

   ::z_stream stream;
   stream.zalloc = zlib_alloc;
   stream.zfree = zlib_free;
   stream.opaque = (::voidpf)0;

   int result = deflateInit(&stream, level);
   if (result != Z_OK) {
      ec = zlib_result_code(result);
      buffer = Buf<UC>();
      return buffer;
   }

   stream.next_out = (::Bytef*)out;
   stream.avail_out = 0;
   stream.next_in = (const ::Bytef*)in;
   stream.avail_in = 0;

   constexpr ::uInt max_bytes = static_cast<::uInt>(-1);
   std::size_t compressed_size = 0;

   do {
      if (stream.avail_out == 0) {
         stream.avail_out = out_remaining > max_bytes ? max_bytes : static_cast<::uInt>(out_remaining);
         out_remaining -= stream.avail_out;
      }
      if (stream.avail_in == 0) {
         stream.avail_in = in_remaining > max_bytes ? max_bytes : static_cast<::uInt>(in_remaining);
         in_remaining -= stream.avail_in;
      }
      stream.total_out = 0;
      result = ::deflate(&stream, in_remaining > 0 ? Z_NO_FLUSH : Z_FINISH);
      compressed_size += stream.total_out;

   } while (result == Z_OK);

   ::deflateEnd(&stream);
   if (result != Z_STREAM_END) {
      ec = zlib_result_code(result);
   }

   if (encode_length) {
      compressed_size += sizeof(L);
      L size = bo::to_net(static_cast<L>(uncompressed_size));
      memcpy(buffer.get(), &size, sizeof(L));
   }

   if (buffer.size() > compressed_size + 100 && buffer.size() > (compressed_size / 8) * 9) {
      try {
         buffer = copy_buf(sub_buf(buffer, 0, compressed_size));
      } catch (const std::bad_alloc&) {
         if (buffer.size() != compressed_size) {
            buffer.release();
            buffer = Buf<UC>(buffer.get(), compressed_size, detail::delete_array);
         }
      }
   } else if (buffer.size() != compressed_size) {
      buffer.release();
      buffer = Buf<UC>(buffer.get(), compressed_size, detail::delete_array);
   }

   return buffer;
}

///////////////////////////////////////////////////////////////////////////////
std::size_t get_uncompressed_length(const UC* compressed, std::size_t compressed_size) noexcept {
   if (compressed_size < sizeof(L)) {
      return 0;
   }

   // first 4 bytes are big-endian uint32 defining uncompressed size.
   // save it to a variable, then advance compressed_source so it points
   // to the first byte after it, and update compressed_size.
   L uncompressed_size;
   memcpy(&uncompressed_size, compressed, sizeof(L));
   return bo::to_host(uncompressed_size);
}

///////////////////////////////////////////////////////////////////////////////
std::size_t inflate(const UC* compressed, std::size_t compressed_size, UC* uncompressed, std::size_t uncompressed_size, std::error_code& ec) noexcept {
   UC tmp[1];    /* for detection of incomplete stream when uncompressed_size == 0 */

   const UC* in = compressed;
   std::size_t in_remaining = compressed_size;

   UC* out = uncompressed;
   std::size_t out_remaining = uncompressed_size;

   if (uncompressed_size == 0) {
      out = tmp;
      out_remaining = sizeof(tmp);
   }

   z_stream stream;
   stream.zalloc = zlib_alloc;
   stream.zfree = zlib_free;
   stream.opaque = (voidpf)0;
   stream.next_in = (const Bytef*)in;
   stream.avail_in = 0;

   int result = inflateInit(&stream);
   if (result != Z_OK) {
      ec = zlib_result_code(result);
      return 0;
   }

   stream.next_out = (Bytef*)out;
   stream.avail_out = 0;

   constexpr ::uInt max_bytes = static_cast<::uInt>(-1);
   std::size_t actual_uncompressed_size = 0;

   do {
      if (stream.avail_out == 0) {
         stream.avail_out = out_remaining > max_bytes ? max_bytes : static_cast<::uInt>(out_remaining);
         out_remaining -= stream.avail_out;
      }
      if (stream.avail_in == 0) {
         stream.avail_in = in_remaining > max_bytes ? max_bytes : static_cast<::uInt>(in_remaining);
         in_remaining -= stream.avail_in;
      }
      stream.total_out = 0;
      result = ::inflate(&stream, Z_NO_FLUSH);
      actual_uncompressed_size += stream.total_out;
   } while (result == Z_OK);

   if (uncompressed_size == 0) {
      if (actual_uncompressed_size > 0 && result == Z_BUF_ERROR) {
         ec = ZlibResultCode::data_error;
      }
      actual_uncompressed_size = 0;
   }

   ::inflateEnd(&stream);
   if (result == Z_NEED_DICT || result == Z_BUF_ERROR && (out_remaining + stream.avail_out > 0)) {
      ec = ZlibResultCode::data_error;
   } else if (result != Z_STREAM_END) {
      ec = zlib_result_code(result);
   }

   return actual_uncompressed_size;
}

} // be::()

///////////////////////////////////////////////////////////////////////////////
/// \note   The returned buffer may be allocated for a larger size than its
///         size() accessor reports.
Buf<UC> deflate_string(const S& text, bool encode_length, I8 level) {
   Buf<UC> buf;
   std::error_code ec;
   buf = deflate(static_cast<const UC*>(static_cast<const void*>(text.c_str())),
                  text.size(), encode_length, level, ec);
   if (ec) {
      throw RecoverableError(ec);
   }
   return buf;
}

///////////////////////////////////////////////////////////////////////////////
/// \note   The returned buffer may be allocated for a larger size than its
///         size() accessor reports.
Buf<UC> deflate_string(const S& text, std::error_code& ec, bool encode_length, I8 level) noexcept {
   return deflate(static_cast<const UC*>(static_cast<const void*>(text.c_str())),
                  text.size(), encode_length, level, ec);
}

///////////////////////////////////////////////////////////////////////////////
/// \note   The returned buffer may be allocated for a larger size than its
///         size() accessor reports.
Buf<UC> deflate_buf(const Buf<const UC>& data, bool encode_length, I8 level) {
   Buf<UC> buf;
   std::error_code ec;
   buf = deflate(data.get(), data.size(), encode_length, level, ec);
   if (ec) {
      throw RecoverableError(ec);
   }
   return buf;
}

///////////////////////////////////////////////////////////////////////////////
/// \note   The returned buffer may be allocated for a larger size than its
///         size() accessor reports.
Buf<UC> deflate_buf(const Buf<const UC>& data, std::error_code& ec, bool encode_length, I8 level) noexcept {
   return deflate(data.get(), data.size(), encode_length, level, ec);
}

///////////////////////////////////////////////////////////////////////////////
S inflate_string(const Buf<const UC>& compressed) {
   S str;
   std::error_code ec;
   str = inflate_string(sub_buf(compressed, sizeof(L)), get_uncompressed_length(compressed.get(), compressed.size()), ec);
   if (ec) {
      throw RecoverableError(ec);
   }
   return str;
}

///////////////////////////////////////////////////////////////////////////////
S inflate_string(const Buf<const UC>& compressed, std::error_code& ec) noexcept {
   return inflate_string(sub_buf(compressed, sizeof(L)), get_uncompressed_length(compressed.get(), compressed.size()), ec);
}

///////////////////////////////////////////////////////////////////////////////
S inflate_string(const Buf<const UC>& compressed, std::size_t uncompressed_length) {
   S str;
   std::error_code ec;
   str = inflate_string(compressed, uncompressed_length, ec);
   if (ec) {
      throw RecoverableError(ec);
   }
   return str;
}

///////////////////////////////////////////////////////////////////////////////
S inflate_string(const Buf<const UC>& compressed, std::size_t uncompressed_length, std::error_code& ec) noexcept {
   S uncompressed;
   try {
      uncompressed.resize(uncompressed_length);
      UC* data = static_cast<UC*>(static_cast<void*>(&uncompressed[0]));
      uncompressed_length = inflate(compressed.get(), compressed.size(), data, uncompressed_length, ec);
      if (!ec) {
         uncompressed.resize(uncompressed_length);
      }
   } catch (const std::bad_alloc&) {
      ec = ZlibResultCode::not_enough_memory;
   } catch (const std::length_error&) {
      ec = ZlibResultCode::not_enough_memory;
   }
   return uncompressed;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> inflate_buf(const Buf<const UC>& compressed) {
   Buf<UC> buf;
   std::error_code ec;
   buf = inflate_buf(sub_buf(compressed, sizeof(L)), get_uncompressed_length(compressed.get(), compressed.size()), ec);
   if (ec) {
      throw RecoverableError(ec);
   }
   return buf;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> inflate_buf(const Buf<const UC>& compressed, std::error_code& ec) noexcept {
   return inflate_buf(sub_buf(compressed, sizeof(L)), get_uncompressed_length(compressed.get(), compressed.size()), ec);
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> inflate_buf(const Buf<const UC>& compressed, std::size_t uncompressed_length) {
   Buf<UC> buf;
   std::error_code ec;
   buf = inflate_buf(compressed, uncompressed_length, ec);
   if (ec) {
      throw RecoverableError(ec);
   }
   return buf;
}

///////////////////////////////////////////////////////////////////////////////
Buf<UC> inflate_buf(const Buf<const UC>& compressed, std::size_t uncompressed_length, std::error_code& ec) noexcept {
   Buf<UC> uncompressed;
   try {
      uncompressed = make_buf<UC>(uncompressed_length);
   } catch (const std::bad_alloc&) {
      ec = ZlibResultCode::not_enough_memory;
   }

   uncompressed_length = inflate(compressed.get(), compressed.size(), uncompressed.get(), uncompressed.size(), ec);

   if (uncompressed.size() != uncompressed_length) {
      if (uncompressed.size() > uncompressed_length + 100 && uncompressed.size() > (uncompressed_length / 8) * 9) {
         try {
            uncompressed = copy_buf(sub_buf(uncompressed, 0, uncompressed_length));
         } catch (const std::bad_alloc&) {
            uncompressed.release();
            uncompressed = Buf<UC>(uncompressed.get(), uncompressed_length, detail::delete_array);
         }
      } else {
         uncompressed.release();
         uncompressed = Buf<UC>(uncompressed.get(), uncompressed_length, detail::delete_array);
      }
   }

   return uncompressed;
}

} // be::util

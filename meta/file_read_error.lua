include 'common/enum'

local file_read_error = make_enum_class('be::util::FileReadError', 'U8', {
   'none',
   'unknown_filesystem_error',
   'file_not_found',
   'file_not_readable',
   'out_of_memory'
})

include('common/enum_std_begin', file_read_error)
include('common/enum_std_end', file_read_error)

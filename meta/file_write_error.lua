include 'common/enum'

local file_write_error = make_enum_class('be::util::FileWriteError', 'U8', {
   'none',
   'unknown_filesystem_error',
   'failed_to_create_file',
   'file_not_writable',
   'disk_full',
   'out_of_memory'
})

include('common/enum_std_begin', file_write_error)
include('common/enum_std_end', file_write_error)

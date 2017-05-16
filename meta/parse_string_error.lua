include 'common/enum'

local parse_string_error = make_enum_class('be::util::ParseStringError', 'U8', {
   'none',
   'empty_input',
   'syntax_error',
   'out_of_range'
})

include('common/enum_std_begin', parse_string_error)
include('common/enum_std_end', parse_string_error)

include 'common/enum'
include 'common/load_tsv'

local data = load_tsv(resolve_include_path('parse_string_error_condition.tsv'))
psec = make_enum_class('be::util::ParseStringErrorCondition', 'U8', data)

local msg_map = {
   name = 'parse_string_error_condition_msg',
   enum = psec,
   type = 'const char*',
   no_cast = true,
   mapper = function (constant)
      if constant.msg and #constant.msg > 0 then
         return '"' .. constant.msg .. '"'
      else
         return '"An unknown error occurred while parsing the input"'
      end
   end,
   default = '"An unknown error occurred while parsing the input"'
}

include('common/enum_std_begin', psec)
if file_ext == '.hpp' then
   write_template('common/templates/enum_scalar_mapping_decl', msg_map)
else
   write_template('common/templates/enum_scalar_mapping', msg_map)
end
include('common/enum_std_end', psec)

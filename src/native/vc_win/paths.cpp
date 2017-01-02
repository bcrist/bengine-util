#include "pch.hpp"
#include <be/core/native.hpp>
#ifdef BE_NATIVE_VC_WIN

#include "paths.hpp"
#include BE_NATIVE_CORE(vc_win_win32.hpp)
#include <Shlobj.h>
#include <Objbase.h>

namespace be {
namespace util {

///////////////////////////////////////////////////////////////////////////////
Path default_special_path(SpecialPath type) {
   
   if (type == SpecialPath::temp) {
      return fs::temp_directory_path();
   } else if (type == SpecialPath::install || type == SpecialPath::process_image) {
      ::HMODULE hModule = ::GetModuleHandleW(NULL);
      ::WCHAR tmp_path[MAX_PATH];
      ::GetModuleFileNameW(hModule, tmp_path, MAX_PATH);
      if (type == SpecialPath::install) {
         return Path(tmp_path).parent_path();
      } else {
         return Path(tmp_path);
      }
   } else {
      ::PWSTR path_str;
      const ::GUID* folder_id;

      switch (type) {
         case SpecialPath::default_install:        folder_id = &::FOLDERID_ProgramFiles; break;
         case SpecialPath::local_profile_data:     folder_id = &::FOLDERID_LocalAppData; break;
         case SpecialPath::roaming_profile_data:   folder_id = &::FOLDERID_RoamingAppData; break;
         case SpecialPath::device_local_data:      folder_id = &::FOLDERID_ProgramData; break;
         case SpecialPath::saved_games:            folder_id = &::FOLDERID_SavedGames; break;
         case SpecialPath::documents:              folder_id = &::FOLDERID_Documents; break;
         case SpecialPath::downloads:              folder_id = &::FOLDERID_Downloads; break;
         case SpecialPath::pictures:               folder_id = &::FOLDERID_Pictures; break;
         case SpecialPath::videos:                 folder_id = &::FOLDERID_Videos; break;
         case SpecialPath::music:                  folder_id = &::FOLDERID_Music; break;
         case SpecialPath::desktop:                folder_id = &::FOLDERID_Desktop; break;
         default:
            throw std::invalid_argument("Unknown or unsupported special path type!");
      }

      ::SHGetKnownFolderPath(*folder_id, 0, NULL, &path_str);
      Path path(path_str);
      ::CoTaskMemFree(path_str);
      return path;
   }
}

///////////////////////////////////////////////////////////////////////////////
S get_env(const S& name) {
   S retval;
   char* buf = nullptr;
   size_t size = 0;
   errno_t error = _dupenv_s(&buf, &size, name.c_str());
   if (error == 0 && buf != nullptr) {
      retval.assign(buf, size);
   }
   free(buf);
   return retval;
}

} // be::util
} // be

#endif

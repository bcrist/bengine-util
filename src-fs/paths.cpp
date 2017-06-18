#include "pch.hpp"
#include "paths.hpp"
#include "service_xoroshiro_128_plus.hpp"
#include "interpolate_string.hpp"
#include <be/core/service_helpers.hpp>
#include <regex>
#include <random>

namespace be::util {
namespace {

Nil init_special_path_config() {
   Path temp = default_special_path(SpecialPath::temp);
   Path inst = default_special_path(SpecialPath::install);
   Path process_image = default_special_path(SpecialPath::process_image);
   Path default_install = default_special_path(SpecialPath::default_install) / "$(AppLongRel)";
   Path local = default_special_path(SpecialPath::local_profile_data) / "$(AppLongRel)";
   Path roaming = default_special_path(SpecialPath::roaming_profile_data) / "$(AppLongRel)";
   Path device_local = default_special_path(SpecialPath::device_local_data) / "$(AppLongRel)";
   Path saves = default_special_path(SpecialPath::saved_games) / "$(AppRel)";
   Path docs = default_special_path(SpecialPath::documents) / "$(AppRel)";
   Path downloads = default_special_path(SpecialPath::downloads) / "$(AppRel)";
   Path pictures = default_special_path(SpecialPath::pictures) / "$(AppRel)";
   Path videos = default_special_path(SpecialPath::videos) / "$(AppRel)";
   Path music = default_special_path(SpecialPath::music) / "$(AppRel)";
   Path desktop = default_special_path(SpecialPath::desktop);

   // TODO Configuration service
   //Configuration& config = service<Configuration>();

   //config.set<S>(Id("Path.AppRel"), "$(AppName)");

   //S app_long_rel;
   //app_long_rel.reserve(26);
   //app_long_rel.append("$(CompanyName)");
   //app_long_rel.append(1, (char)fs::path::preferred_separator);
   //app_long_rel.append("$(AppName)");
   //config.set<S>(Id("Path.AppLongRel"), app_long_rel);

   //config.set<S>(Id("Path.Temp"), temp.string());
   //config.set<S>(Id("Path.Inst"), inst.string());
   //config.set<S>(Id("Path.DefaultInstall"), default_install.string());
   //config.set<S>(Id("Path.Local"), local.string());
   //config.set<S>(Id("Path.Roaming"), roaming.string());
   //config.set<S>(Id("Path.DeviceLocal"), device_local.string());
   //config.set<S>(Id("Path.Export"), "$(Desktop)");
   //config.set<S>(Id("Path.Screenshots"), "$(Pictures)");
   //config.set<S>(Id("Path.Recordings"), "$(Videos)");
   //config.set<S>(Id("Path.Saves"), saves.string());
   //config.set<S>(Id("Path.Docs"), docs.string());
   //config.set<S>(Id("Path.Downloads"), downloads.string());
   //config.set<S>(Id("Path.Pictures"), pictures.string());
   //config.set<S>(Id("Path.Videos"), videos.string());
   //config.set<S>(Id("Path.Music"), music.string());
   //config.set<S>(Id("Path.Desktop"), desktop.string());
   return Nil();
}

void ensure_special_path_config() {
   static Nil nil = init_special_path_config();
}

std::regex env_regex("%([A-Za-z0-9_]+)%");
std::regex quote_regex("\"((\"\"|\\\\\"?|[^\\\\\"]+)*)\"");

} // be::util::()

///////////////////////////////////////////////////////////////////////////////
Path cwd() {
   return fs::current_path();
}

///////////////////////////////////////////////////////////////////////////////
void cwd(const Path& path, bool create_path) {
   if (create_path) {
      fs::create_directories(path);
   }
   
   Path p = fs::canonical(path);
   fs::current_path(p);
}

///////////////////////////////////////////////////////////////////////////////
Path special_path(SpecialPath type) {
   const char* path_expr = "";
   switch (type) {
      case SpecialPath::temp:                   path_expr = "$(Temp)"; break;
      case SpecialPath::install:                path_expr = "$(Inst)"; break;
      case SpecialPath::process_image:          path_expr = "$(ProcessImage)"; break;
      case SpecialPath::default_install:        path_expr = "$(DefaultInstall)"; break;
      case SpecialPath::local_profile_data:     path_expr = "$(Local)"; break;
      case SpecialPath::roaming_profile_data:   path_expr = "$(Roaming)"; break;
      case SpecialPath::device_local_data:      path_expr = "$(DeviceLocal)"; break;
      case SpecialPath::file_export:            path_expr = "$(Export)"; break;
      case SpecialPath::screenshots:            path_expr = "$(Screenshots)"; break;
      case SpecialPath::recordings:             path_expr = "$(Recordings)"; break;
      case SpecialPath::saved_games:            path_expr = "$(Saves)"; break;
      case SpecialPath::documents:              path_expr = "$(Docs)"; break;
      case SpecialPath::downloads:              path_expr = "$(Downloads)"; break;
      case SpecialPath::pictures:               path_expr = "$(Pictures)"; break;
      case SpecialPath::videos:                 path_expr = "$(Videos)"; break;
      case SpecialPath::music:                  path_expr = "$(Music)"; break;
      case SpecialPath::desktop:                path_expr = "$(Desktop)"; break;
      default:
         throw std::invalid_argument("Unknown special path type!");
   }
   return parse_path(path_expr);
}

///////////////////////////////////////////////////////////////////////////////
Path random_path(const Path& model, const Path& symbols) {
   auto& prng = service<util::xo128p>();
   
   auto native = model.native();
   auto natsym = symbols.native();

   std::uniform_int_distribution<> dist(0, (int)(natsym.size() - 1));

   for (auto& c : native) {
      if (c == static_cast<decltype(natsym)::value_type>('%')) {
         int which = dist(prng);
         c = natsym[which];
      }
   }

   return Path(native);
}

///////////////////////////////////////////////////////////////////////////////
S expand_path(const S& path, bool use_config) {
   S new_path;
   if (use_config) {
      // replace configuration variables
      new_path = interpolate_string(path, [](S var) {
         // TODO Configuration service
         //Id var_id("Path." + var);
         //const Configuration& cfg = service<Configuration>();
         //if (cfg.exists(var_id)) {
         //   return expand_path(cfg.get<S>(var_id), use_config);
         //} else {
         //   var_id = Id(var);
         //   if (cfg.exists(var_id)) {
         //      return expand_path(cfg.get<S>(var_id), use_config);
         //   }
         //}
         return S();
      });
   } else {
      new_path = path;
   }

   // replace environment variables
   S final_path;
   auto it = new_path.cbegin();
   auto end = new_path.cend();
   std::smatch match;
   while (std::regex_search(it, end, match, env_regex)) {
      final_path.append(match.prefix());
      final_path.append(expand_path(get_env(match[1]), false));
      it = match.suffix().first;
   }
   final_path.append(it, end);
   return final_path;
}

///////////////////////////////////////////////////////////////////////////////
Path parse_path(const S& path) {
   S expanded = expand_path(path);

   std::smatch match;
   if (std::regex_match(expanded, match, quote_regex)) {
      expanded = match[1].str();
   }

   return Path(expanded).make_preferred();
}

///////////////////////////////////////////////////////////////////////////////
std::vector<Path> parse_multi_path(const S& multi_path) {
   std::vector<Path> paths;
   parse_multi_path(multi_path, paths);
   return paths;
}

///////////////////////////////////////////////////////////////////////////////
void parse_multi_path(const S& multi_path, std::vector<Path>& paths) {
   S expanded_multi = expand_path(multi_path);
   S path_str;
   path_str.reserve(expanded_multi.size());

   bool quoted = false;
   bool escaped = false;
   bool quote_esc = false;
   
   for (char c : expanded_multi) {
      switch (c) {
         case ';':
         case ':': {
            if (escaped) {
               escaped = false;
            } else if (quoted) {
               path_str.append(1, c);
               break;
            }
            Path p = parse_path(path_str);
            paths.push_back(std::move(p));
            path_str.clear();
            quote_esc = false;
            break;
         }

         case '\\':
            escaped = !escaped;
            quote_esc = false;
            path_str.append(1, c);
            break;

         case '"':
            if (escaped) {
               escaped = false;
            } else if (quote_esc) {
               quote_esc = false;
               quoted = true;
            }
            
            quoted = !quoted;

            if (!quoted) {
               quote_esc = true;
            }

            path_str.append(1, c);
            break;

         default:
            path_str.append(1, c);
            escaped = false;
            quote_esc = false;
            break;
      }
   }

   Path p = parse_path(path_str);
   if (!p.empty()) {
      paths.push_back(std::move(p));
   }
}

} // be::util

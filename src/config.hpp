#pragma once

#define TOML_ENABLE_FORMATTERS 0
#define TOML_ENABLE_WINDOWS_COMPAT 0
#include <toml.hpp>

struct config
{
   static inline uint32_t rotate_mouse_button { 257 }; // Left Mouse Button
   static inline uint32_t pan_mouse_button = 2; // Middle mouse button
   static inline uint32_t pan_gamepad_button = 10; // L3 button
   static inline float min_rotate_speed = 3.f;
   static inline float max_rotate_speed = 12.f;
   static inline float pan_speed = 5.f;

   static void load()
   {
       constexpr std::string_view config_path { "Data\\SKSE\\Plugins\\PlayerRotationGS.toml"sv };

       if (!std::filesystem::exists(config_path))
       {
           logger::warn("Could not find config at {}"sv, config_path);
           return;
       }

       try
       {
           auto tbl = toml::parse(config_path.data());
           rotate_mouse_button = toml::find_or(tbl, "RotateMouseButton", rotate_mouse_button);
           pan_mouse_button = toml::find_or(tbl, "PanMouseButton", pan_mouse_button);
           pan_gamepad_button = toml::find_or(tbl, "PanGamepadButton", pan_gamepad_button);
           min_rotate_speed = toml::find_or(tbl, "MinimumRotationSpeed", min_rotate_speed);
           max_rotate_speed = toml::find_or(tbl, "MaximumRotationSpeed", max_rotate_speed);
           pan_speed = toml::find_or(tbl, "PanSpeed", pan_speed);
       } 
       catch (const toml::syntax_error& err)
       {
           logger::error("{}"sv, err.what());
           return;
       }
   }
};
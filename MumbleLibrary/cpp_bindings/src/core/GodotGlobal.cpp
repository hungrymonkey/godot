#include "GodotGlobal.hpp"

#include "String.hpp"

#include <godot/gdnative.h>

namespace godot {

void *_RegisterState::nativescript_handle;

void Godot::print(const String& message)
{
	godot_print((godot_string *) &message);
}

void Godot::print_warning(const String& description, const String& function, const String& file, int line)
{
	godot_print_warning(description.c_string(), function.c_string(), file.c_string(), line);
}

void Godot::print_error(const String& description, const String& function, const String& file, int line)
{
	godot_print_error(description.c_string(), function.c_string(), file.c_string(), line);
}

};

void gdnative_init(godot_gdnative_init_options *options);
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *options)
{
	gdnative_init(options);
}

void gdnative_terminate(godot_gdnative_terminate_options *options);
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *options)
{
	gdnative_terminate(options);
}

void nativescript_init();
extern "C" void GDN_EXPORT godot_nativescript_init(void *handle)
{
	godot::_RegisterState::nativescript_handle = handle;
	nativescript_init();
}

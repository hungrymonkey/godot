#ifndef GODOT_GLOBAL_HPP
#define GODOT_GLOBAL_HPP

#include "String.hpp"


namespace godot {

class Godot {

public:
	static void print(const String& message);
	static void print_warning(const String& description, const String& function, const String& file, int line);
	static void print_error(const String& description, const String& function, const String& file, int line);

};



struct _RegisterState {
	static void *nativescript_handle;
};

}

#endif

#include <core/Godot.hpp>
#include <Reference.hpp>

using namespace godot;


class MumbleClass : public GodotScript<Reference> {
        GODOT_CLASS(MumbleClass);
public:
        MumbleClass() { }
	String _name;

};

/** GDNative Initialize **/
GDNATIVE_INIT(godot_gdnative_init_options *options) {

}

/** GDNative Terminate **/
GDNATIVE_TERMINATE(godot_gdnative_terminate_options *options) {

}

/** NativeScript Initialize **/
NATIVESCRIPT_INIT() {
        register_class<MumbleClass>();
}

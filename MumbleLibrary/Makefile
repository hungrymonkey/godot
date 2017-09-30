CC=gcc
CCFLAGS= -g -O3 -std=c++14 -fPIC

DEPS = cpp_bindings/include godot_headers cpp_bindings/include/



clean:
	pushd cpp_bindings
	scons -c
	popd
	rm lib/*

#!/usr/bin/env bash


pushd cpp_bindings
scons p=linux
popd
cp cpp_bindings/bin/libgodot_cpp_bindings.a lib/

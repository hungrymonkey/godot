#!/usr/bin/env python
# -*- coding: utf-8 -*-
# config.py

def can_build(env, platform):
        return True

#TODO: needs to be tested on windows
def configure(env):
        env.Append(LIBS=["opencv_video","opencv_imgproc","opencv_objdetect","opencv_core", "opencv_highgui"])
        env.Append(CXXFLAGS=['-mavx'])

        #env.Append(CCFLAGS=["-DLIB_NO_GUI_SUPPORT=1", "-DUSE_AVX_INSTRUCTIONS=1" ])
        #
        if env['platform'] == 'osx':
                env.ParseConfig('pkg-config opencv --cflags --libs')
                env.Append(CPPPATH=["/opt/X11/include/"])
                env.Append(LIBS=["X11"])
                env.Append(LIBPATH=["/opt/X11/lib"])

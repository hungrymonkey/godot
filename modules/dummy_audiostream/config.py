# config.py

def can_build(platform):
    return True

def configure(env):
    env.ParseConfig('pkg-config sdl2 --cflags --libs')
    pass

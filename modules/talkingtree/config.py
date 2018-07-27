# config.py

def can_build(env, platform):
	return True

def configure(env):
	env.ParseConfig('pkg-config sdl2 --cflags --libs')
	env.Append(LIBS=['protobuf'])
	pass

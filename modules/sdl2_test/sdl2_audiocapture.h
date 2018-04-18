#ifndef SDL2_AUDIOCAPTURE_H
#define SDL2_AUDIOCAPTURE_H

#include "object.h"
#include "rid.h"
#include "variant.h"
#include "set.h"
#include "os/thread.h"
#include "os/mutex.h"
#include <SDL.h>

class SDLDevice : public RID_Data {
	RID self;

private:
	SDL_AudioDeviceID devid_in;
	SDL_AudioSpec wanted;
	int format;
public:
	int get_available_frames() const;
	PoolByteArray get_frame();
	void talk();
	void mute();
	bool recording();
	SDLDevice(int sample_rate, int format, int frame_size);
	~SDLDevice();
};

class SDL2AudioCapture : public Object {
	GDCLASS(SDL2AudioCapture, Object);
    
	static SDL2AudioCapture *singleton;
	static void thread_func(void *p_udata);
private:
	bool thread_exited;
	mutable bool exit_thread;
	Thread *thread;
	Mutex *mutex;

public:
	static SDL2AudioCapture *get_singleton();
	Error init();
	void lock();
	void unlock();
	void finish();
protected:
	static void _bind_methods();

private:
	mutable RID_Owner<SDLDevice> mic_owner;
	Set<RID> running_devices;

public:
	enum Format {
		FORMAT_8_BIT = 1,
		FORMAT_16_BIT = 2,
		FORMAT_FLOAT = 4,
	};
	RID create(int sample_rate, int format, int frame_size);
	void flush();
	void destroy(RID dev);
	void talk(RID devid);
	void mute(RID devid);
	bool recording() const;
	SDL2AudioCapture();
};
class _SDL2AudioCapture : public Object {
	GDCLASS(_SDL2AudioCapture, Object);

	static _SDL2AudioCapture *singleton;

protected:
	static void _bind_methods();
public:
	static _SDL2AudioCapture *get_singleton();
	void _emit_pcm(PoolByteArray pcm);
	void destroy(RID devid);
	void talk(RID devid);
	void mute(RID devid);
	RID create(int sample_rate, int format, int frame_size);
	_SDL2AudioCapture();
	

};

VARIANT_ENUM_CAST(SDL2AudioCapture::Format);

#endif SDL2_AUDIOCAPTURE_H

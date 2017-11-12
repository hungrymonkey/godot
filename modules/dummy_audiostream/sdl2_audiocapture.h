#ifndef SDL2_AUDIOCAPTURE_H
#define SDL2_AUDIOCAPTURE_H

#include "object.h"
#include "variant.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"
#include <SDL.h>
class SDL2AudioCapture : public Object {
	GDCLASS(SDL2AudioCapture, Object);
    
	static SDL2AudioCapture *singleton;
	static void thread_func(void *p_udata);
public:
	static SDL2AudioCapture *get_singleton();

private:
	enum{
		SAMPLE_RATE = 48000,
		FRAME_SIZE = 960,
	};
	bool thread_exited;
	mutable bool exit_thread;
    Thread *thread;
	Mutex *mutex;
	SDL_AudioDeviceID devid_in;
	SDL_AudioSpec wanted;
	int get_available_frames() const;
protected:
	static void _bind_methods();
public:
	enum Format {
		FORMAT_8_BIT,
		FORMAT_16_BIT,
	};
	void set_singleton();
	void lock();
	void unlock();
	void talk();
	void mute();
	bool recording();
	void finish();
	Error init();
	SDL2AudioCapture();
};
VARIANT_ENUM_CAST(SDL2AudioCapture::Format);
#endif SDL2_AUDIOCAPTURE_H
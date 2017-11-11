/* summator.h */
#ifndef DUMMY_AUDIOSTREAM_H
#define DUMMY_AUDIOSTREAM_H

#include "reference.h"
#include "resource.h"

#include "servers/audio/audio_stream.h"
#include "ustring.h"
#include "list.h"
#include <SDL.h>

class AudioStreamDummy;

class AudioStreamPlaybackDummy : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamPlaybackDummy, AudioStreamPlaybackResampled)
	friend class AudioStreamDummy;
private:
	enum{
		PCM_BUFFER_SIZE = 4096
	};
	enum {
		MIX_FRAC_BITS = 13,
		MIX_FRAC_LEN = (1 << MIX_FRAC_BITS),
		MIX_FRAC_MASK = MIX_FRAC_LEN - 1,
	};
	void * pcm_buffer;
	Ref<AudioStreamDummy> base;
	bool active;
protected:
	virtual float get_stream_sampling_rate();
	virtual void _mix_internal(AudioFrame *p_buffer, int p_frames);
public:
	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;
	virtual int get_loop_count() const; //times it looped
	virtual float get_playback_position() const;
	virtual void seek(float p_time);
	virtual float get_length() const; //if supported, otherwise return 0
	
	AudioStreamPlaybackDummy();

};

class AudioStreamDummy : public AudioStream {
	GDCLASS(AudioStreamDummy, AudioStream)
private:
	SDL_AudioDeviceID devid_in;
	friend class AudioStreamPlaybackDummy;
	uint64_t pos;
	int mix_rate;
	bool stereo;
	int hz;
	List<uint8_t> data;
	Error put_data(const uint8_t * pcm_data, int size);
	static void _sdl_callback(void * usr_data, unsigned char * pcm, int len);
public:
	int get_available_bytes() const;
	void reset();
	void set_position(uint64_t pos);
	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;
	void gen_tone(int16_t *, int frames);
	int get_16();
	AudioStreamDummy();
	void talk();
	void mute();
	
protected:
	static void _bind_methods();
};
#endif

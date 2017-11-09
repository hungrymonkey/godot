/* summator.h */
#ifndef DUMMY_AUDIOSTREAM_H
#define DUMMY_AUDIOSTREAM_H

#include "reference.h"
#include "resource.h"

#include "servers/audio/audio_stream.h"
#include "ustring.h"

class AudioStreamDummy;

class AudioStreamPlaybackDummy : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackDummy, AudioStreamPlayback)
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
	bool playing;
public:
	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;
	virtual int get_loop_count() const; //times it looped
	virtual float get_playback_position() const;
	virtual void seek(float p_time);
	virtual void mix(AudioFrame *p_buffer, float p_rate_scale, int p_frames);
	virtual float get_length() const; //if supported, otherwise return 0
	
	AudioStreamPlaybackDummy();

private:
	template <class Depth, bool is_stereo>
	void do_resample(const Depth *p_src, AudioFrame *p_dst, int64_t &offset, int32_t &increment, uint32_t amount);
};

class AudioStreamDummy : public AudioStream {
	GDCLASS(AudioStreamDummy, AudioStream)
private:
	friend class AudioStreamPlaybackDummy;
	enum {
		DATA_PAD = 16 //padding for interpolation
	};
	uint64_t pos;
	int mix_rate;
	bool stereo;
	int hz;
public:
	void reset();
	void set_position(uint64_t pos);
	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;
	void gen_tone(uint16_t *, int frames);
	AudioStreamDummy();
	
protected:
	static void _bind_methods();
};
#endif

#ifndef MYTONE_AUDIOSTREAM_RESAMPLED_H
#define MYTONE_AUDIOSTREAM_RESAMPLED_H

#include "reference.h"
#include "resource.h"

#include "servers/audio/audio_stream.h"

class AudioStreamMyToneResampled;

class AudioStreamPlaybackResampledMyTone : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamPlaybackResampledMyTone, AudioStreamPlaybackResampled)
	friend class AudioStreamMyToneResampled;
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
	Ref<AudioStreamMyToneResampled> base;
	bool active;
protected:
	virtual void _mix_internal(AudioFrame *p_buffer, int p_frames);
public:
	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;
	virtual int get_loop_count() const; //times it looped
	virtual float get_playback_position() const;
	virtual void seek(float p_time);
	virtual float get_length() const; //if supported, otherwise return 0
	virtual float get_stream_sampling_rate();
	AudioStreamPlaybackResampledMyTone();

};

class AudioStreamMyToneResampled : public AudioStream {
	GDCLASS(AudioStreamMyToneResampled, AudioStream)
private:
	friend class AudioStreamPlaybackResampledMyTone;
	uint64_t pos;
	int mix_rate;
	bool stereo;
	int hz;
public:
	void reset();
	void set_position(uint64_t pos);
	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;
	void gen_tone(int16_t *, int frames);
	AudioStreamMyToneResampled();
	
protected:
	static void _bind_methods();
};

#endif



#ifndef AUDIOSTREAMMUMBLE_H
#define AUDIOSTREAMMUMBLE_H

#include "servers/audio/audio_stream.h"
#include "ustring.h"
#include "reference.h"

class AudioStreamMumble;

class AudioStreamPlaybackMumble : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackMumble, AudioStreamPlayback)
	friend class AudioStreamMumble;
	

private:
	enum {
		MIX_FRAC_BITS = 13,
		MIX_FRAC_LEN = (1 << MIX_FRAC_BITS),
		MIX_FRAC_MASK = MIX_FRAC_LEN - 1,
	};
	bool active;
	Ref<AudioStreamMumble> base;
	int64_t offset;
	template <class Depth, bool is_stereo>
	void do_resample(const Depth *p_src, AudioFrame *p_dst, int64_t &offset, int32_t &increment, uint32_t amount);
public:
	AudioStreamPlaybackMumble();
	~AudioStreamPlaybackMumble();
	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;
	virtual int get_loop_count() const; //times it looped
	virtual float get_playback_position() const;
	virtual void seek(float p_time);
	virtual void mix(AudioFrame *p_bufer, float p_rate_scale, int p_frames);
	virtual float get_length() const; //if supported, otherwise return 0
};


class AudioStreamMumble : public AudioStream {
	GDCLASS(AudioStreamMumble, AudioStream)
public:
	enum Format {
		FORMAT_8_BITS,
		FORMAT_16_BITS,
	};
protected:
	static void _bind_methods();
private:
	friend class AudioStreamPlaybackMumble;
	enum {
		DATA_PAD = 16, //padding for interpolation
		BUFFER_SIZE = 4096
	};
	Format format;
	uint32_t data_bytes;
	int mix_rate;
	int64_t offset;
	void *data;
	bool stereo;
	
public:
	AudioStreamMumble();
	~AudioStreamMumble();
	PoolVector<uint8_t> get_data() const;
	void append_data(const int16_t * pcm_data, uint32_t size);
	void set_format(Format p_format);
	Format get_format() const;
	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;

private:
	Ref<AudioStream> audio_stream;
};
VARIANT_ENUM_CAST(AudioStreamMumble::Format)
#endif AUDIOSTREAMMUMBLE_H

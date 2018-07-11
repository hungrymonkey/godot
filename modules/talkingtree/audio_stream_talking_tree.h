#ifndef AUDIOSTREAMTALKINGTREE_H
#define AUDIOSTREAMTALKINGTREE_H

#include "servers/audio/audio_stream.h"
#include "ustring.h"
#include "reference.h"
#include "io/stream_peer.h"
#include "audio_stream_lockless_buffer.h"

class AudioStreamTalkingTree;

class AudioStreamPlaybackTalkingTree : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamPlaybackTalkingTree, AudioStreamPlaybackResampled)
	friend class AudioStreamTalkingTree;
	

private:
	enum {
		PCM_BUFFER_SIZE = 4096
	};
	enum {
		MIX_FRAC_BITS = 13,
		MIX_FRAC_LEN = (1 << MIX_FRAC_BITS),
		MIX_FRAC_MASK = MIX_FRAC_LEN - 1,
	};
	bool active;
	Ref<AudioStreamTalkingTree> base;
	int64_t offset;
protected:
	virtual void _mix_internal(AudioFrame *p_buffer, int p_frames);
	virtual float get_stream_sampling_rate();	
	
public:
	AudioStreamPlaybackTalkingTree();
	~AudioStreamPlaybackTalkingTree();
	virtual void start(float p_from_pos = 0.0);
	virtual void stop();
	virtual bool is_playing() const;
	virtual int get_loop_count() const; //times it looped
	virtual float get_playback_position() const;
	virtual void seek(float p_time);
	virtual float get_length() const; //if supported, otherwise return 0
};


class AudioStreamTalkingTree : public AudioStream {
	GDCLASS(AudioStreamTalkingTree, AudioStream)
public:
	enum Format {
		FORMAT_8_BITS,
		FORMAT_16_BITS,
	};
protected:
	static void _bind_methods();
private:
	friend class AudioStreamPlaybackTalkingTree;
	Format format;
	float mix_rate;
	bool stereo;
	bool opened;
	int id;
	TreecursionAudioStreamBuffer<16384> data_buffer;
	Mutex *writeLock;
public:
	AudioStreamTalkingTree();
	~AudioStreamTalkingTree();
	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;
	virtual float get_length() const { return 0; }
	int get_available_bytes() const;
	int get_byte_array( uint8_t *buf, int size);
	Error append_data(const uint8_t * pcm_data, int p_bytes);
	void set_format(Format p_format);
	void set_mix_rate(float rate);
	void clear();
	Format get_format() const;
	void set_pid(int id);
};
VARIANT_ENUM_CAST(AudioStreamTalkingTree::Format)
#endif AUDIOSTREAMTALKINGTREE_H
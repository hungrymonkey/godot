
#include "servers/audio/audio_stream.h"
#include "ustring.h"



class AudioStreamPlaybackMumble : public AudioStreamPlayback {
	GDCLASS(AudioStreamPlaybackMumble, AudioStreamPlayback)
public:
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
	virtual Ref<AudioStreamPlayback> instance_playback();
	virtual String get_stream_name() const;
	AudioStreamMumble();
private:
	Ref<AudioStream> audio_stream;
};

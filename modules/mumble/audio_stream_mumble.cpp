#include "audio_stream_mumble.h"



void AudioStreamPlaybackMumble::start(float p_from_pos) {
}
void AudioStreamPlaybackMumble::stop() {
}
bool AudioStreamPlaybackMumble::is_playing() const {
}
int AudioStreamPlaybackMumble::get_loop_count() const {
	//times it looped
	return 0;
}
float AudioStreamPlaybackMumble::get_playback_position() const{
	return 0.0f;
}
void AudioStreamPlaybackMumble::seek(float p_time){
}
void AudioStreamPlaybackMumble::mix(AudioFrame *p_bufer, float p_rate_scale, int p_frames){
}
float AudioStreamPlaybackMumble::get_length() const {
	//if supported, otherwise return 0
	return 0;
}

String AudioStreamMumble::get_stream_name() const {
	/*
	if (audio_stream.is_valid()) {
		return "Mumble: " + audio_stream->get_name();
	}*/
	return "Mumble";
}
AudioStreamMumble::AudioStreamMumble(){
}

#include "dummy_audiostream.h"
#include "math/math_funcs.h"
#include "print_string.h"
#include "sdl2_audiocapture.h"

AudioStreamPlaybackDummy::AudioStreamPlaybackDummy() 
    : active(false){
        AudioServer::get_singleton()->lock();
        pcm_buffer = AudioServer::get_singleton()->audio_data_alloc(PCM_BUFFER_SIZE);
        zeromem(pcm_buffer, PCM_BUFFER_SIZE);
        AudioServer::get_singleton()->unlock();
		
}

void AudioStreamPlaybackDummy::stop(){
    active = false;
    base->reset();
	base->talk();
}

void AudioStreamPlaybackDummy::start(float p_from_pos){
	base->mute();
	if(base->get_available_bytes()<10000){
		return;
	}
    seek(p_from_pos);
    active = true;
	_begin_resample();
}
void AudioStreamPlaybackDummy::seek(float p_time){
    float max = get_length();
	if (p_time < 0) {
		p_time = 0;
	}
    base->set_position(uint64_t(p_time * base->mix_rate) << MIX_FRAC_BITS);
}
void AudioStreamPlaybackDummy::_mix_internal(AudioFrame *p_buffer, int p_frames){
	ERR_FAIL_COND(!active);
	if (!active) {
		for(int i = 0; i < p_frames; i++){
			p_buffer[i] = AudioFrame(0,0);
		}
		return;
    }
	int smaller_buf = MIN(base->get_available_bytes()/2, p_frames );


	for(int i = 0;  i < smaller_buf; i++){
		float sample =  float(base->get_16())/32768.0f;
	//	print_line("0: " + rtos(sample));
		p_buffer[i] = AudioFrame(sample, sample);
	}
	int todo = p_frames - smaller_buf;
	while(todo){
		p_buffer[p_frames-todo-1] = AudioFrame(0,0);
		todo--;
		active = false;
	}
}
float AudioStreamPlaybackDummy::get_stream_sampling_rate(){
	return float(base->mix_rate);
}
int AudioStreamPlaybackDummy::get_loop_count() const {
    return 0;
}
float AudioStreamPlaybackDummy::get_playback_position() const {
    return float(base->pos)/float(base->mix_rate);
}
float AudioStreamPlaybackDummy::get_length() const {
	float a = base->get_available_bytes()/2;
    return a/float(base->mix_rate);
}
bool AudioStreamPlaybackDummy::is_playing() const {
    return active;
}

AudioStreamDummy::AudioStreamDummy()
    : mix_rate(48000), stereo(false), hz(639) {
	talk();
}

int AudioStreamDummy::get_available_bytes() const{
	return data.size();
}

void AudioStreamDummy::talk(){
	SDL2AudioCapture::get_singleton()->connect("get_pcm", this, "append_data");
	SDL2AudioCapture::get_singleton()->talk();	
}
void AudioStreamDummy::mute(){
	SDL2AudioCapture::get_singleton()->disconnect("get_pcm", this, "append_data");
	SDL2AudioCapture::get_singleton()->mute();
}

Error AudioStreamDummy::put_data(const float * pcm_data, int size){
	for(int i = 0; i < size; i++){
		data.push_back(pcm_data[i]*32767);
	}
	//emit_signal("audio_recieved");
	return OK;
}
void AudioStreamDummy::append_data(PoolByteArray pcm){
	for(int i = 0; i < pcm.size(); i++){

		data.push_back(pcm[i]);
	}
	//emit_signal("audio_recieved");
}
int AudioStreamDummy::get_16(){
	int16_t buf;
	uint8_t *ptr = (uint8_t *) &buf;
	ptr[0] = data[0];
	ptr[1] = data[1];
	data.pop_front();
	data.pop_front();
	return buf;
}
Ref<AudioStreamPlayback> AudioStreamDummy::instance_playback(){
	Ref<AudioStreamPlaybackDummy> talking_tree;
	talking_tree.instance();
	talking_tree->base = Ref<AudioStreamDummy>(this);

	return talking_tree;
}

String AudioStreamDummy::get_stream_name() const {
	return "Dummy";
}
void AudioStreamDummy::reset() {
    set_position(0);
	data.clear();
}
void AudioStreamDummy::set_position(uint64_t p) {
    pos = p;
}
void AudioStreamDummy::gen_tone(int16_t * pcm_buf, int size){
    for( int i = 0; i < size; i++){
        pcm_buf[i] = 32767.0 * sin(2.0*Math_PI*double(pos+i)/(double(mix_rate)/double(hz)));
    }
    pos += size;
}
void AudioStreamDummy::_bind_methods(){
	ClassDB::bind_method(D_METHOD("reset"), &AudioStreamDummy::reset);
	ClassDB::bind_method(D_METHOD("get_stream_name"), &AudioStreamDummy::get_stream_name);
	ClassDB::bind_method(D_METHOD("append_data", "pcm"), &AudioStreamDummy::append_data);
	
}

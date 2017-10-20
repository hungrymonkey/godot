/* mumble.cpp */

#include "mumble.h"
#include "callback.h"
#include "utils.h"
#include "os/thread.h"
#include "print_string.h"
#include <mumlib/Transport.hpp>
#include <string>
#include "scene/main/timer.h"

Mumble::Mumble() : _cb(*this) {
	_pMumble = Ref<_PrivateMumble>(memnew(_PrivateMumble(_cb)));
}

Mumble::_PrivateMumble::_PrivateMumble( mumlib::Callback & c) : _mum(c) {
}
void Mumble::_PrivateMumble::engage(String host, int port, String user, String password){
	std::string h = utils::gstr2cpp_str(host);
	std::string u = utils::gstr2cpp_str(user);
	std::string p = utils::gstr2cpp_str(password);
	this->_mum.connect(h, port,  u, p);
	this -> start();
}
void Mumble::_PrivateMumble::start(){
	while(true){
		try{
			this->_mum.run();
		}catch (mumlib::TransportException &exp) {
			print_line( "Mumble: error " + utils::cpp_str2gstr(exp.what()));
			print_line( "Mumble: attempting to reconnect in 5s. ");
			Timer * sleep = memnew(Timer);
			sleep -> set_wait_time(5.0);
			sleep -> start();
			memdelete(sleep);
		}
	}
}



void Mumble::_PrivateMumble::sendText(const String text){
	_mum.sendTextMessage( utils::gstr2cpp_str(text) );
}

void Mumble::_PrivateMumble::send16bAudio(const PoolByteArray & sample){
	uint32_t pcm_len = sample.size()/2;
	int16_t pcm[50000];
	for(int i = 0; i < pcm_len; i++){
		uint16_t low = (uint16_t) sample[2*i];
		uint16_t hi = (uint16_t) sample[2*i+1];
		pcm[i] = ( low | ( hi << 8));
	}
	_mum.sendAudioData(pcm, pcm_len);
}

void Mumble::_bind_methods() {
	ClassDB::bind_method(D_METHOD("engage", "host", "port", "user", "password"), &Mumble::engage);
	ClassDB::bind_method(D_METHOD("disengage"), &Mumble::disengage);
//	ClassDB::bind_method(D_METHOD("start"), &Mumble::start);

	ClassDB::bind_method(D_METHOD("sendText", "text"), &Mumble::sendText);
	ClassDB::bind_method(D_METHOD("sendAudio", "sample"), &Mumble::sendAudio);
	ADD_SIGNAL(MethodInfo("audio_message", PropertyInfo(Variant::OBJECT, "audio_sample"), PropertyInfo(Variant::VECTOR3, "position"), PropertyInfo(Variant::INT, "target"), PropertyInfo(Variant::INT, "session_id")));
	ADD_SIGNAL(MethodInfo("text_message", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::INT, "actor")));

}
void Mumble::engage(String host, int port, String user, String password) {
	_pMumble -> engage( host,  port,  user,  password);
}

void Mumble::disengage(){
	_pMumble->disengage();
	_userState.clear();
}
void Mumble::start(){
	_pMumble->start();
}
void Mumble::sendText(const String text){
	_pMumble -> sendText( text );
}
void Mumble::removeUser(uint32_t session_id){
	_userState.erase(Variant(session_id));
}
void Mumble::addUser(String s, uint32_t session_id){
	_userState[Variant(session_id)]=s;
}
void Mumble::sendAudio(Ref<AudioStreamSample> sample){

	const PoolByteArray data = sample->get_data();


	if(data.size() > 0){
		switch(sample->get_format()){
			case AudioStreamSample::FORMAT_16_BITS:         
				_pMumble->send16bAudio(data);
			default:
				return;
		}
	}
    
}


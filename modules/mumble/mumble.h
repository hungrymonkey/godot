/* mumble.h */
#ifndef MUMBLE_H
#define MUMBLE_H

#include "reference.h"
#include <mumlib.hpp>
#include "scene/resources/audio_stream_sample.h"
#include "variant.h"
#include "callback.h"
#include "dictionary.h"


class Mumble :  public Reference {
	GDCLASS(Mumble,Reference);
private:
	class _PrivateMumble: public Reference {
		GDCLASS(_PrivateMumble,Reference);
	private:
		mumlib::Mumlib _mum;
		
	public:
		_PrivateMumble(mumlib::Callback & c);
		void engage(String host, int port, String user, String password);
		void sendText(const String text);
		void send16bAudio(const PoolByteArray & arr);
		void disengage() { _mum.disconnect(); }
		void start();
	};
	Ref<_PrivateMumble> _pMumble;

protected:
	static void _bind_methods();

public:

	void engage(String host, int port, String user, String password);
	void disengage();
	void start();
	void sendText(const String message);
	void sendAudio(Ref<AudioStreamSample> sample);
	void removeUser(uint32_t user);
	void addUser(String u, uint32_t user);
	Mumble();
//    ~Mumble();
private:
	MyCallBack _cb;
	Dictionary _userState;
};

#endif

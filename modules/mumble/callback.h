/* callback.h */
#ifndef SimpleCALLBACK_H
#define SimpleCALLBACK_H

#include "reference.h"
#include <string.h>
#include <mumlib/Callback.hpp>
#include <vector>

class MyCallBack : public mumlib::Callback{

public:
	MyCallBack( Object &o ): _mumble_ref(o){
	};
	virtual void audio(int target,
			int sessionId,
			int sequenceNumber,
			int16_t *pcm_data,
			uint32_t pcm_data_size,
			float position [3]);
	virtual void textMessage(
		uint32_t actor,
		std::vector<uint32_t> session,
		std::vector<uint32_t> channel_id,
		std::vector<uint32_t> tree_id,
		std::string message);
	virtual void userState(int32_t session, int32_t actor, std::string name, 
		int32_t user_id, int32_t channel_id, int32_t mute, int32_t deaf, 
		int32_t suppress, int32_t self_mute, int32_t self_deaf, std::string comment, 
		std::string plugin_context, int32_t priority_speaker, int32_t recording);
	virtual void userRemove(uint32_t session, int32_t actor,
		std::string reason, bool ban);
	virtual void version(
		uint16_t major,
		uint8_t minor,
		uint8_t patch,
		std::string release,
		std::string os,
		std::string os_version);
private:
	Object &_mumble_ref;
};


#endif

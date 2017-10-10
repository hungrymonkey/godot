#include "callback.h"
#include "utils.h"
#include "variant.h"
#include "print_string.h"
#include "scene/resources/audio_stream_sample.h"

SimpleCallback::SimpleCallback() : _callback(*this) { }
SimpleCallback::~SimpleCallback() { }
mumlib::Callback *SimpleCallback::get_callback(){
    return &_callback;
}
void SimpleCallback::MyCallBack::audio( int target,
                       int sessionId,
                       int sequenceNumber,
                       int16_t *pcm_data,
                       uint32_t pcm_data_size){
   if(!_cb._audio_handler.is_null()){
        Variant sid(sessionId);
        Variant snum(sequenceNumber);
        Ref<AudioStreamSample> sam = Ref<AudioStreamSample>(utils::pcm2Sample(pcm_data, pcm_data_size));    
        Variant pcm(sam);

        Variant::CallError err;
        const Variant *args[3] = { &sid, &snum, &pcm};
        Variant result =  _cb._audio_handler->call_func( args, 3, err);
   }
}


void SimpleCallback::MyCallBack::textMessage(
            uint32_t actor,
            std::vector<uint32_t> session,
            std::vector<uint32_t> channel_id,
            std::vector<uint32_t> tree_id,
	    std::string message) {
   if(!_cb._text_handler.is_null()){
        Variant s = utils::cpp_uint32vec2Variant(session);
        Variant c = utils::cpp_uint32vec2Variant(channel_id);
        Variant t = utils::cpp_uint32vec2Variant(tree_id);
        Variant a(actor);
        Variant m(String(message.c_str()));
        Variant::CallError err;
        const Variant *args[5] = {&a, &s, &c, &t, &m};
        Variant result =  _cb._text_handler->call_func( args, 5, err );
   }

}

void SimpleCallback::MyCallBack::version(
                uint16_t major,
                uint8_t minor,
                uint8_t patch,
                std::string release,
                std::string os,
                std::string os_version){ }
                
void SimpleCallback::MyCallBack::suggestConfig(
                uint32_t version,
                uint32_t positional,
                uint32_t push_to_talk){
    print_line( String("Postional audio is supported: ") + ((positional) ? "True" : "False"));
}

void SimpleCallback::_bind_methods(){
   ClassDB::bind_method(D_METHOD("setAudioHandler", "handler"), &SimpleCallback::setAudioHandler);
   ClassDB::bind_method(D_METHOD("setTextHandler", "handler"), &SimpleCallback::setTextHandler);

}

void SimpleCallback::setAudioHandler(  Ref<FuncRef> handler){
   this->_audio_handler = handler;
}
void SimpleCallback::setTextHandler( Ref<FuncRef> handler){
   this->_text_handler = handler;
}

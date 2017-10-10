/* callback.h */
#ifndef SimpleCALLBACK_H
#define SimpleCALLBACK_H

#include "reference.h"
#include "ustring.h"
#include "func_ref.h"
#include <string.h>
#include <mumlib/Callback.hpp>
#include <vector>
#include "scene/resources/audio_stream_sample.h"

class SimpleCallback : public Reference {
    GDCLASS(SimpleCallback,Reference);


protected:
//    bool _set(const StringName &p_name, const Variant &p_value);
//    bool _get(const StringName &p_name, Variant &r_ret) const;
//    void _get_property_list(List<PropertyInfo> *p_list) const;
    static void _bind_methods();
   
private:
    Ref<FuncRef>  _audio_handler;
    Ref<FuncRef>  _text_handler;
    
    class MyCallBack : public mumlib::Callback{
    protected:
        static void _bind_methods();

    public:
        MyCallBack( SimpleCallback &o ): _cb(o){
        };
        virtual void audio(int target,
                        int sessionId,
                        int sequenceNumber,
                        int16_t *pcm_data,
                uint32_t pcm_data_size)  ;

        virtual void textMessage(
                uint32_t actor,
                std::vector<uint32_t> session,
                std::vector<uint32_t> channel_id,
                std::vector<uint32_t> tree_id,
                std::string message);

        virtual void version(
                    uint16_t major,
                    uint8_t minor,
                    uint8_t patch,
                    std::string release,
                    std::string os,
            std::string os_version);
        
        virtual void suggestConfig(
                uint32_t version,
                uint32_t positional,
                uint32_t push_to_talk);
        
    private:
        SimpleCallback &_cb;
    };
    MyCallBack _callback;
public:
     SimpleCallback();
    ~SimpleCallback();
     mumlib::Callback * get_callback();
   
    void setAudioHandler(Ref<FuncRef> handler);
    void setTextHandler(Ref<FuncRef> handler);


};

#endif

/* talking_tree.h */
#ifndef TALKING_TREE_H
#define TALKING_TREE_H

#include "object.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "io/multiplayer_api.h"
#include "ustring.h"
#include "talking_tree_enum.h"
#include "scene/main/timer.h"
#include "audio_stream_talking_tree.h"
#include "utils.h"
#include <opus.h>
#include <google/protobuf/message.h>

class TalkingTree : public Object {
	GDCLASS(TalkingTree, Object);

	static TalkingTree *singleton;
	static void thread_func(void *p_udata);

protected:
	static void _bind_methods();

public:
	static TalkingTree *get_singleton();
	void lock();
	void unlock();
	void finish();
	Error init();

private:
	bool thread_exited;
	mutable bool exit_thread;
	Thread *thread;
	Mutex *mutex;

public:
	TalkingTree();
	~TalkingTree();
	void set_network_peer(const Ref<MultiplayerAPI> &p_network_peer);
	Vector<int> get_network_connected_peers() const;
	bool is_network_server() const;
	bool has_network_peer() const;
	int get_network_unique_id() const;
	void send_user_info();
	void poll();
	//VOIP
	void talk();
	void mute();
	//Text
	void send_text(String msg);
	
private:
	int last_send_cache_id;
	Ref<MultiplayerAPI> multiplayer;
	void _send_user_info(int p_to);
	void _send_packet(int p_to, PacketType type, google::protobuf::Message &message, NetworkedMultiplayerPeer::TransferMode transfer);
	void _network_process_packet(int p_from, const uint8_t *p_packet, int p_packet_len);
	void _network_poll();
	void _network_peer_connected(int p_id);
	void _network_peer_disconnected(int p_id);
	void _connected_to_server();
	void _connection_failed();
	void _server_disconnected();
	void _create_audio_peer_stream(int p_id);
	//voip
	int outgoing_sequence_number;
	uint64_t last_sent_audio_timestamp;
	//encoders
    enum{
		SAMPLE_RATE = 48000, //44100 crashes in the constructor
		BIT_RATE = 16000, //https://wiki.xiph.org/Opus_Recommended_Settings#Bandwidth_Transition_Thresholds
		FRAME_SIZE = 960 //https://www.opus-codec.org/docs/html_api/group__opusencoder.html#ga88621a963b809ebfc27887f13518c966
	};
	OpusDecoder *opusDecoder;
	OpusEncoder *opusEncoder;
	void reset_encoder();
	//audiostream
    HashMap<int, Ref<AudioStreamTalkingTree>> connected_audio_stream_peers;
	void _create_audio_frame(PoolVector<uint8_t> pcm);
	int _encode_audio_frame(int target, PoolVector<uint8_t> &pcm);
	void _process_audio_packet(int p_from, const uint8_t *p_packet, int p_packet_len);
	Pair<int, bool> _decode_opus_frame(const uint8_t *in_buf, int in_len, int16_t *pcm_buf, int buf_len);
};

class _TalkingTree : public Object {
	GDCLASS(_TalkingTree, Object);

	friend class TalkingTree;

	static _TalkingTree *singleton;
protected:
	
	static void _bind_methods();
	
public:
	static _TalkingTree *get_singleton() { return singleton; }
	_TalkingTree();
	~_TalkingTree();
};

#endif

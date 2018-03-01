/* talking_tree.h */
#ifndef TALKING_TREE_H
#define TALKING_TREE_H

#include "reference.h"
#include "io/networked_multiplayer_peer.h"
#include "ustring.h"
#include "talking_tree_enum.h"
#include "scene/main/timer.h"
#include "audio_stream_talking_tree.h"
#include "utils.h"

#include "modules/benet/enet_packet_peer.h"
#include <opus.h>
#include <google/protobuf/message.h>

class TalkingTree : public Reference {
	GDCLASS(TalkingTree, Reference);

protected:
	static void _bind_methods();

public:
	TalkingTree();
	~TalkingTree();
	void set_network_peer(const Ref<ENetPacketPeer> &p_network_peer);
	void set_game_network_peer(const Ref<ENetPacketPeer> &p_network_peer);
	Vector<int> get_network_connected_peers() const;
	bool is_network_server() const;
	bool has_network_peer() const;
	int get_network_unique_id() const;
	void send_user_info();
	void poll();
	//VOIP
	void talk();
	void mute();
	Ref<AudioStreamTalkingTree> get_audio_stream_peer(int pid);
	//Text
	void send_text(String msg);
	


private:
	int last_send_cache_id;
	Ref<ENetPacketPeer> network_peer;
	Ref<ENetPacketPeer> game_peer;
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
	BiMap<int, int> connected_peers; /* game_id : talkingtree_id */
	void _create_audio_frame(PoolVector<uint8_t> pcm);
	int _encode_audio_frame(int target, PoolVector<uint8_t> &pcm);
	void _process_audio_packet(int p_from, const uint8_t *p_packet, int p_packet_len);
	Pair<int, bool> _decode_opus_frame(const uint8_t *in_buf, int in_len, int16_t *pcm_buf, int buf_len);
};

#endif

/* talking_tree.cpp */

#include "talking_tree_enum.h"
#include "varint.h"
#include "talking_tree.h"

#include "TalkingTree.pb.h"
#include "io/marshalls.h"

#include "sdl2_audiocapture.h"

#include "os/os.h"
#include "pair.h"

//#include "modules/talkingtree_storage/talking_tree_storage.h"

TalkingTree *TalkingTree::singleton = nullptr;

void TalkingTree::lock() {
	if (!thread || !mutex)
		return;
	mutex->lock();
}
void TalkingTree::unlock() {
	if (!thread || !mutex)
		return;
	mutex->unlock();
}

Error TalkingTree::init(){
	thread_exited = false;
	mutex = Mutex::create();
	thread = Thread::create(TalkingTree::thread_func, this);
	return OK;
}
void TalkingTree::finish() {
	if (!thread)
		return;
	exit_thread = true;
	Thread::wait_to_finish(thread);

	memdelete(thread);
	if (mutex)
		memdelete(mutex);
	thread = NULL;
}

void TalkingTree::thread_func(void *p_udata){
	TalkingTree *ac = (TalkingTree *) p_udata;

	uint64_t usdelay = 20000;
	while(!ac -> exit_thread){
		ac->poll();
		OS::get_singleton()->delay_usec(usdelay);
	}
}

void TalkingTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_network_server"), &TalkingTree::is_network_server);
	ClassDB::bind_method(D_METHOD("has_network_peer"), &TalkingTree::has_network_peer);
	ClassDB::bind_method(D_METHOD("get_network_connected_peers"), &TalkingTree::get_network_connected_peers);
	ClassDB::bind_method(D_METHOD("get_network_unique_id"), &TalkingTree::get_network_unique_id);
	ClassDB::bind_method(D_METHOD("send_user_info"), &TalkingTree::send_user_info);
	
	ClassDB::bind_method(D_METHOD("_network_peer_connected"), &TalkingTree::_network_peer_connected);
	ClassDB::bind_method(D_METHOD("_network_peer_disconnected"), &TalkingTree::_network_peer_disconnected);
	ClassDB::bind_method(D_METHOD("_connected_to_server"), &TalkingTree::_connected_to_server);
	ClassDB::bind_method(D_METHOD("_connection_failed"), &TalkingTree::_connection_failed);
	ClassDB::bind_method(D_METHOD("_server_disconnected"), &TalkingTree::_server_disconnected);
	ClassDB::bind_method(D_METHOD("_queue_network_packet"), &TalkingTree::_queue_network_packet);

	ADD_SIGNAL(MethodInfo("connected_to_server"));
	ADD_SIGNAL(MethodInfo("connection_failed"));
	ADD_SIGNAL(MethodInfo("server_disconncted"));
	ADD_SIGNAL(MethodInfo("network_peer_connected", PropertyInfo(Variant::INT, "id")));
	ADD_SIGNAL(MethodInfo("network_peer_disconnected", PropertyInfo(Variant::INT, "id")));
	//Text
	ClassDB::bind_method(D_METHOD("send_text", "message"), &TalkingTree::send_text);
	ADD_SIGNAL(MethodInfo("text_message", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::INT, "sender_id")));
	//VOIP
	ClassDB::bind_method(D_METHOD("_create_audio_frame", "audio_frame"), &TalkingTree::_create_audio_frame);

}

TalkingTree::TalkingTree() : last_sent_audio_timestamp(0){
	int error;
	opusDecoder = opus_decoder_create(TalkingTree::SAMPLE_RATE, 1, &error);
	if(error != OPUS_OK){
		ERR_PRINTS( "failed to initialize OPUS decoder: " + String( opus_strerror(error)));
	}
	opusEncoder = opus_encoder_create(TalkingTree::SAMPLE_RATE, 1, OPUS_APPLICATION_VOIP, &error);
	if(error != OPUS_OK){
		ERR_PRINTS( "failed to initialize OPUS encoder: " + String( opus_strerror(error)));
	}
	opus_encoder_ctl(opusEncoder, OPUS_SET_VBR(0));
	error = opus_encoder_ctl(opusEncoder, OPUS_SET_BITRATE(TalkingTree::BIT_RATE));
	if (error != OPUS_OK) {
        ERR_PRINTS("failed to initialize bitrate to " + itos(TalkingTree::BIT_RATE) + "B/s: " + String(opus_strerror(error)));
	}
	reset_encoder();
	singleton = this;
}
TalkingTree::~TalkingTree(){
    if (opusDecoder) {
        opus_decoder_destroy(opusDecoder);
    }

    if (opusEncoder) {
        opus_encoder_destroy(opusEncoder);
	}
	singleton = nullptr;
}
void TalkingTree::reset_encoder(){
	int status = opus_encoder_ctl(opusEncoder, OPUS_RESET_STATE, nullptr);
	
	if (status != OPUS_OK) {
		ERR_PRINTS("failed to reset encoder: " + String( opus_strerror(status)));
	}
	outgoing_sequence_number = 0;
}
void TalkingTree::send_text(const String &msg) {
	TalkingTreeProto::TextMessage txtMsg;
	CharString m = msg.utf8();
	txtMsg.set_message(m.get_data(), m.length());
	_send_packet(0, PacketType::TEXTMESSAGE, txtMsg, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
}
void TalkingTree::send_user_info(){
	_send_user_info(0);
}
void TalkingTree::_send_user_info(int p_to){
	if(multiplayer.is_valid()){
		TalkingTreeProto::UserInfo usrInfo;
		usrInfo.set_user_id(multiplayer->get_network_peer()->get_unique_id());
		_send_packet(p_to, PacketType::USERINFO, usrInfo, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
	}
}


void TalkingTree::_create_audio_peer_stream(int p_id){
	connected_audio_stream_peers[p_id].instance();
	connected_audio_stream_peers[p_id]->set_pid(p_id);
	connected_audio_stream_peers[p_id]->set_mix_rate(TalkingTree::SAMPLE_RATE);
	connected_audio_stream_peers[p_id]->set_format(AudioStreamTalkingTree::FORMAT_16_BITS);
	
}

void TalkingTree::_send_packet(int p_to, PacketType type, google::protobuf::Message &message, NetworkedMultiplayerPeer::TransferMode transferMode){
	PoolVector<uint8_t> packet;
	//incorrect
	packet.resize(1 + message.ByteSize());
	packet.set(0,(uint8_t)type);
	PoolVector<uint8_t>::Write w = packet.write();
	encode_uint32(message.ByteSize(), w.ptr()+1);
	message.SerializeToArray( w.ptr()+1, message.ByteSize());
	multiplayer->send_bytes(packet, p_to, transferMode);
}

void TalkingTree::_network_process_packet(int p_from, const uint8_t *p_packet, int p_packet_len) {
	PacketType packet_type = (PacketType) p_packet[0];
	const uint8_t * proto_packet = &p_packet[1];
	int proto_packet_len = p_packet_len - 1;
	switch(packet_type){
		case PacketType::VERSION: {
		} break;
		case PacketType::UDPTUNNEL: {
			_process_audio_packet(p_from, proto_packet, proto_packet_len);
		} break;
		case PacketType::TEXTMESSAGE: {
			TalkingTreeProto::TextMessage txtMsg;
			txtMsg.ParseFromArray( proto_packet, proto_packet_len );
			String msg;
			msg.parse_utf8(txtMsg.message().c_str(), txtMsg.message().length());
			this->emit_signal("text_message", msg, p_from);
		} break;
		case PacketType::USERINFO: {
			TalkingTreeProto::UserInfo usrInfo;
			usrInfo.ParseFromArray( proto_packet, proto_packet_len );
			int game_id = usrInfo.user_id();
			int tree_id = usrInfo.tree_id();
			//create audio stream peer
			_create_audio_peer_stream(tree_id);
			if(multiplayer->is_network_server()){
				//send everybody else
				const int *k=NULL;
				Vector<int> peers = get_network_connected_peers();
				for(int i = 0; peers.size(); i++) {
					TalkingTreeProto::UserInfo otherUsr;
					otherUsr.set_user_id(peers[i]);
					_send_packet(p_from, PacketType::USERINFO, otherUsr, NetworkedMultiplayerPeer::TRANSFER_MODE_RELIABLE);
					
				}
				//send yourself
				_send_user_info(p_from);
			}
			
		}
		default:
			break;
	}
}

bool TalkingTree::is_network_server() const {

	ERR_FAIL_COND_V(!multiplayer->is_network_server(), false);
	return multiplayer->is_network_server();
}

bool TalkingTree::has_network_peer() const {
	return multiplayer->has_network_peer();
}

void TalkingTree::set_multiplayer(const Ref<MultiplayerAPI> &p_multiplayer) {
	if (multiplayer.is_valid() && multiplayer->has_network_peer()) {
		multiplayer->disconnect("network_peer_connected", this, "_network_peer_connected");
		multiplayer->disconnect("network_peer_packet", this, "_queue_network_packet");
		multiplayer->disconnect("server_disconnected", this, "_server_disconnected");
		connected_audio_stream_peers.clear();
		last_send_cache_id = 1;
		SDL2AudioCapture::get_singleton()->disconnect("get_pcm", this, "_create_audio_frame");
	}

	ERR_EXPLAIN("Supplied MultiplayerAPI must be connecting or connected.");
	ERR_FAIL_COND(p_multiplayer.is_valid() && p_multiplayer->get_network_peer()->get_connection_status() == NetworkedMultiplayerPeer::CONNECTION_DISCONNECTED);
	multiplayer = p_multiplayer;
	if (multiplayer.is_valid()) {
		multiplayer->connect("network_peer_connected", this, "_network_peer_connected");
		multiplayer->connect("network_peer_packet", this, "_queue_network_packet");
		multiplayer->connect("connection_succeeded", this, "_connected_to_server");
		multiplayer->connect("server_disconnected", this, "_server_disconnected");
		SDL2AudioCapture::get_singleton()->connect("get_pcm", this, "_create_audio_frame");
	}
}
void TalkingTree::_connection_failed() {
	emit_signal("connection_failed");
}
void TalkingTree::_connected_to_server() {
	emit_signal("connected_to_server");
}
void TalkingTree::_network_peer_connected(int p_id) {
	emit_signal("network_peer_connected", p_id);
}
void TalkingTree::_network_peer_disconnected(int p_id) {
	connected_audio_stream_peers.erase(p_id);
	emit_signal("network_peer_disconnected", p_id);
}
void TalkingTree::_server_disconnected(){
	connected_audio_stream_peers.clear();
	emit_signal("server_disconnected");
}
int TalkingTree::get_network_unique_id() const {
	ERR_FAIL_COND_V(!multiplayer.is_valid(), 0);
	return multiplayer->get_network_unique_id();
}
Vector<int> TalkingTree::get_network_connected_peers() const {
	ERR_FAIL_COND_V(!multiplayer.is_valid(), Vector<int>());	
	return multiplayer->get_network_connected_peers();
}

void TalkingTree::_poll_queue() {
	if (!multiplayer.is_valid() || multiplayer->get_network_peer()-> get_connection_status() == NetworkedMultiplayerPeer::CONNECTION_DISCONNECTED)
		return;

	while(queue.size() > 0){
		Packet tmp = queue.front()->get();
		auto d = &tmp.data;
		_network_process_packet( tmp.p_from, d->read().ptr(), d->size() );
		queue.pop_front();
	}
}

void TalkingTree::_queue_network_packet( int from, const PoolVector<uint8_t> &data) {
	Packet p;
	p.p_from = from;
	p.data = data;
	queue.push_back(p);
}
void TalkingTree::poll(){
	_poll_queue();
}
void TalkingTree::mute(){
	SDL2AudioCapture::get_singleton()->mute();
}

void TalkingTree::talk(){
	SDL2AudioCapture::get_singleton()->talk();
}


Pair<int, bool> TalkingTree::_decode_opus_frame(const uint8_t *in_buf, int in_len, int16_t *pcm_buf, int buf_len){
	VarInt varint(in_buf);
	int16_t opus_length = varint.getValue();
	int pointer = varint.getEncoded().size();
	bool endFrame = opus_length != 0x2000;
	opus_length = opus_length & 0x1fff;
	int decoded_samples = opus_decode(opusDecoder, (const unsigned char *) &in_buf[pointer], opus_length, pcm_buf, buf_len, 0);
	return Pair<int,bool>(decoded_samples, endFrame);
}

void TalkingTree::_process_audio_packet(int p_from, const uint8_t *p_packet, int p_packet_len){
	//print_line("from id: "+ itos(p_from));
	int pointer = 1;
	VarInt seqNum(&p_packet[pointer]);
	pointer += seqNum.getEncoded().size();
	uint64_t sequenceNumber = seqNum.getValue();

	if(pointer >= p_packet_len){
		ERR_PRINTS("invalid incoming audio packet " + itos(p_packet_len) + "B : header : " + itos(pointer));
	}
	AudioCodingType codingType = static_cast<AudioCodingType>((p_packet[0] & 0xC0) >> 6);
	bool facial_flag = (p_packet[0] & 0x20); //anything greater than 0 is true;
	uint8_t target = p_packet[0] & 0x1F;

	if(facial_flag){
		ERR_PRINTS("Facial data not supported");
	}
	int payloadLength = p_packet_len - pointer;
	const uint8_t *payload = &p_packet[pointer];
	int16_t pcm_buf[50000];
	//print_line("talking: " + itos(p_from) + " size: " + itos(payloadLength));
	Pair<int, bool> out_len;
	//TreecursionVoiceTask *vt;
	switch( codingType ){
		case AudioCodingType::OPUS: {
				out_len = _decode_opus_frame(payload, payloadLength, pcm_buf, 50000);
				//vt = memnew(
				//	TreecursionVoiceTask(OS::get_singleton()->get_ticks_usec(), connected_peers.getBackward(p_from), payload, payloadLength));
				break;
			}
		default:
			ERR_PRINTS("Unsupported Audio format: " + itos((uint32_t) codingType));
			return;
	}
	//TalkingTreeStorage::get_singleton()->enqueue(vt);
	//connected_audio_stream_peers[p_from]->append_data((uint8_t *) pcm_buf, sizeof(int16_t) * out_len.first);
	//this->emit_signal("audio_message", p_from);

	PoolByteArray ret;
	ret.resize(sizeof(int16_t) * out_len.first);
	copymem( ret.write().ptr(), pcm_buf, sizeof(int16_t) * out_len.first);
	_TalkingTree::get_singleton()->audio_message_signal(ret, p_from);
}

void TalkingTree::_create_audio_frame(PoolVector<uint8_t> pcm){
	_encode_audio_frame(0, pcm);
}
int TalkingTree::_encode_audio_frame(int target, PoolVector<uint8_t> &pcm){
	int now = OS::get_singleton()->get_ticks_msec();
	if( (now-last_sent_audio_timestamp) > 5000 ){
		reset_encoder();
	}
	int16_t pcm_buf[TalkingTree::FRAME_SIZE];
	PoolVector<uint8_t>::Read  tmp = pcm.read();
	copymem( pcm_buf, tmp.ptr(), pcm.size());
	//https://www.opus-codec.org/docs/html_api/group__opusencoder.html#ga88621a963b809ebfc27887f13518c966
	//in_len most be multiples of 120
	uint8_t opus_buf[1024];
	const int output_size = opus_encode(opusEncoder, pcm_buf, TalkingTree::FRAME_SIZE, opus_buf, 1024);
	//get self network id
	//TreecursionVoiceTask *vt = memnew(
	//	TreecursionVoiceTask(OS::get_singleton()->get_ticks_usec(), game_peer->get_unique_id(), opus_buf, output_size));
	//TalkingTreeStorage::get_singleton()->enqueue(vt);
	Vector<uint8_t> encoded_size = VarInt(output_size).getEncoded();
	Vector<uint8_t> encoded_seq = VarInt( outgoing_sequence_number ).getEncoded();

	int seqNum = 100 * pcm.size()/2 / TalkingTree::SAMPLE_RATE;	
	outgoing_sequence_number += seqNum;

	PoolVector<uint8_t> audiobuf;
	audiobuf.resize(1 + 1 + encoded_seq.size() + encoded_size.size() + output_size);
	audiobuf.set(1, 0x00 | target);
	PoolVector<uint8_t>::Write w = audiobuf.write();
	
	copymem( w.ptr() + 2,  encoded_seq.ptr(), encoded_seq.size() );
	copymem( w.ptr() + 2 + encoded_seq.size(),  encoded_size.ptr(), encoded_size.size() );
	copymem( w.ptr() + 2 + encoded_seq.size() + encoded_size.size(),  opus_buf, output_size );

	
	last_sent_audio_timestamp = OS::get_singleton()->get_ticks_msec();
	audiobuf.set(0, (uint8_t) PacketType::UDPTUNNEL);

	multiplayer->send_bytes(audiobuf, 0, NetworkedMultiplayerPeer::TRANSFER_MODE_UNRELIABLE_ORDERED);
	return audiobuf.size();
}

_TalkingTree *_TalkingTree::singleton = nullptr;

void _TalkingTree::_bind_methods() {
	ClassDB::bind_method(D_METHOD("send_text", "message"), &_TalkingTree::send_text);

	ClassDB::bind_method(D_METHOD("talk"), &_TalkingTree::talk);
	ClassDB::bind_method(D_METHOD("mute"), &_TalkingTree::mute);
	ClassDB::bind_method(D_METHOD("set_multiplayer"), &_TalkingTree::set_multiplayer);

	ADD_SIGNAL(MethodInfo("text_message", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::INT, "sender_id")));
	ADD_SIGNAL(MethodInfo("audio_message", PropertyInfo(Variant::POOL_BYTE_ARRAY, "message"), PropertyInfo(Variant::INT, "sender_id")));
}
void _TalkingTree::set_multiplayer(const Ref<MultiplayerAPI> &p_multiplayer){
	TalkingTree::get_singleton()->set_multiplayer(p_multiplayer);
}
void _TalkingTree::send_text(const String &msg){
	TalkingTree::get_singleton()->send_text(msg);
}
void _TalkingTree::mute(){
	SDL2AudioCapture::get_singleton()->mute();
}

void _TalkingTree::talk(){
	SDL2AudioCapture::get_singleton()->talk();
}
void _TalkingTree::audio_message_signal( const PoolByteArray &data, int p_from){
	emit_signal("audio_message", data, p_from);
}

_TalkingTree::_TalkingTree() {
	singleton = this;	
}
_TalkingTree::~_TalkingTree() {
	singleton = nullptr;
}
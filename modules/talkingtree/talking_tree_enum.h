#pragma once

enum class PacketType{
	VERSION = 0,
	TEXTMESSAGE = 1,
	UDPTUNNEL = 2,
	USERINFO = 3
};

enum class AudioCodingType{
	OPUS = 0
};

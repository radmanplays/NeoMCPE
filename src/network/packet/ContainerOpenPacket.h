#ifndef NET_MINECRAFT_NETWORK_PACKET__ContainerOpenPacket_H__
#define NET_MINECRAFT_NETWORK_PACKET__ContainerOpenPacket_H__

//package net.minecraft.network.packet;

#include "../Packet.h"

class ContainerOpenPacket: public Packet
{
public:
    ContainerOpenPacket() {
    }

    ContainerOpenPacket(int x, int y, int z, int containerId, int type, const std::string& title, int size)
    :   x(x),
        y(y),
        z(z),
        containerId(containerId),
        type(type),
        title(title.c_str()),
        size(size)
    {
    }

    void write(RakNet::BitStream* bitStream) {
		bitStream->Write((RakNet::MessageID)(ID_USER_PACKET_ENUM + PACKET_CONTAINEROPEN));
        bitStream->Write(x);
        bitStream->Write(y);
        bitStream->Write(z);
        bitStream->Write(containerId);
        bitStream->Write(type);
        bitStream->Write(size);
        bitStream->Write(title);
    }

	void read(RakNet::BitStream* bitStream) {
        bitStream->Read(x);
        bitStream->Read(y);
        bitStream->Read(z);
        bitStream->Read(containerId);
        bitStream->Read(type);
        bitStream->Read(size);
        bitStream->Read(title);
    }

	void handle(const RakNet::RakNetGUID& source, NetEventCallback* callback) {
		callback->handle(source, (ContainerOpenPacket*)this);
	}

    int x, y, z;
    RakNet::RakString title;
    unsigned char containerId;
    unsigned char type;
    unsigned char size;
};

#endif /*NET_MINECRAFT_NETWORK_PACKET__ContainerOpenPacket_H__*/

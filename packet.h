#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; // Packet Length
	double send_time; // Time at which the packet is generated from the source
	bool wayback;
	double q_time;
	double q_elapsed; 
};


#endif 


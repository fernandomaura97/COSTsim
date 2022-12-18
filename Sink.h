/*
	Sinks. Collect de traffic generate by the mobile nodes.
*/

#ifndef _SINK_H
#define _SINK_H

#include "packet.h"

component Sink : public TypeII
{
	public:
		// Connections
		inport void in(MPDU_packet &packet);

	public:
		void Setup();
		void Start();
		void Stop();
		

	public:
		double system_time;
		double received_packets;
		double av_L; // average packet length
		int id;

};

void Sink :: Setup()
{

};

void Sink :: Start()
{
	system_time = 0;
	received_packets = 0;
	av_L = 0; 
};

void Sink :: Stop()
{	
	switch(id){

		case 1: 
			printf("[STATS 1st SINK :]\n");
			printf("\ttest Average System Time (Queueing + Transmission) = %f\n",system_time/received_packets);
			printf("\tReceived traffic = %f\n",av_L/SimTime());
			break;
		case 2: 
			printf("STATS last SINK:\n");
			printf("\ttest Average System Time (Queueing + Transmission) = %f\n",system_time/received_packets);
			printf("\tReceived traffic = %f\n",av_L/SimTime());
			break;
		default: printf("ID SINK ERR!\n");

	}
};

void Sink :: in(MPDU_packet &packet)
{
	system_time += SimTime() - packet.send_time;
	av_L += packet.L;
	received_packets++;
	#ifdef DBG_SINK
	printf("rx packets: %f, paket l: %d, av_l = %f\n", received_packets, packet.L, av_L);
	#endif
};

#endif

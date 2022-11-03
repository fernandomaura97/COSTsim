/*
	Sinks. Collect de traffic generate by the mobile nodes.
*/

#ifndef _SINK_
#define _SINK_
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

};

void Sink :: Setup()
{

};

void Sink :: Start()
{
	system_time = 0;
	received_packets = 0;
};

void Sink :: Stop()
{
	printf("test Average System Time (Queueing + Transmission) = %f\n",system_time/received_packets);
	printf("Received traffic = %f\n",av_L/SimTime());
};

void Sink :: in(MPDU_packet &packet)
{
	system_time += SimTime() - packet.send_time;
	av_L += packet.L;
	received_packets++;
};

#endif

/*
	Queue Module
*/


#include "packet.h"
#include "FIFO2.h"
 

//#define Q X #define C X

component QueueModule2 : public TypeII
{
	public:
		// Connections
		inport void in(MPDU_packet &packet);
		outport void out(MPDU_packet &packet);

		// Timer
		Timer <trigger_t> service_time;
		inport inline void endService(trigger_t& t);

		QueueModule2 () { connect service_time.to_component,endService; }


	private:
		FIFO queue;

	public:
		double blocked_packets;
		double arrived_packets;
		double queue_length;

	public:
		void Setup();
		void Start();
		void Stop();


};

void QueueModule2 :: Setup()
{

};

void QueueModule2 :: Start()
{
	// Statistics
	blocked_packets = 0;
	arrived_packets = 0;
	queue_length = 0;
};

void QueueModule2 :: Stop()
{
	#ifdef DBG_QUEUE2
		printf("WAY OUT:\n");
		printf("\ttest Blocking Probability = %f\n",blocked_packets/arrived_packets);
		printf("\ttest E[Queue Length] = %f\n",queue_length/arrived_packets);
	#endif
};

void QueueModule2 :: in(MPDU_packet &packet)
{
	arrived_packets++;
	queue_length += queue.QueueSize();
	if(queue.QueueSize() < Q)
	{
		queue.PutPacket(packet);

		if(queue.QueueSize() == 1)
		{
			service_time.Set(SimTime()+(packet.L/C));
		}
	}
	else blocked_packets++;


};

void QueueModule2 :: endService(trigger_t &)
{
	MPDU_packet packet = queue.GetFirstPacket();
	queue.DelFirstPacket();
	out(packet);

	if(queue.QueueSize() > 0)
	{
		MPDU_packet packet = queue.GetFirstPacket();
		service_time.Set(SimTime()+(packet.L/C));
	}

};




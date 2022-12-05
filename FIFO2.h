
#ifndef _FIFO_QUEUE_
#define _FIFO_QUEUE_

//#include "/home/boris/Programes/Simulation/sense-2003-1229/common/cost.h"

#include "packet.h"
#include <deque>


/*
	FIFO Class
*/

component FIFO2 : public TypeII
{	
	private:
		std::deque <MPDU_packet> m_queue;
		
	public:
		MPDU_packet GetFirstPacket();
		MPDU_packet GetPacketAt(int n);
		void DelFirstPacket();		
		void DeletePacketIn(int i);
		void PutPacket(MPDU_packet &packet);	
		void PutPacketFront(MPDU_packet &packet);	
		void PutPacketIn(MPDU_packet &packet, int);	
		int QueueSize();
};

MPDU_packet FIFO2 :: GetFirstPacket()
{
	return(m_queue.front());	
}; 

MPDU_packet FIFO2 :: GetPacketAt(int n)
{
	return(m_queue.at(n));	
}; 


void FIFO2 :: DelFirstPacket()
{
	m_queue.pop_front();
}; 

void FIFO2 :: PutPacket(MPDU_packet &packet)
{	
	m_queue.push_back(packet);
}; 

void FIFO2 :: PutPacketFront(MPDU_packet &packet)
{	
	m_queue.push_front(packet);
}; 

int FIFO2 :: QueueSize()
{
	return(m_queue.size());
}; 

void FIFO2 :: PutPacketIn(MPDU_packet & packet,int i)
{
	m_queue.insert(m_queue.begin()+i,packet);
}; 

void FIFO2 :: DeletePacketIn(int i)
{
	m_queue.erase(m_queue.begin()+i);
};


#endif

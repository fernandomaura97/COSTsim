/*
	Poisson source. 
*/


		
#include "packet.h"

component PoissonSource : public TypeII
{
	public:
		// Connections
		outport void out(MPDU_packet &packet);	

		// Timer
		Timer <trigger_t> inter_packet_timer;
		inport inline void new_packet(trigger_t& t); // action that takes place when timer expires

		PoissonSource () { connect inter_packet_timer.to_component,new_packet; }

	public:
		double bandwidth; // Source Bandwidth
		double arrival_rate;
		double L;
	public:
		void Setup();
		void Start();
		void Stop();
			
	private:
		//void new_packet(trigger_t &t);
};

void PoissonSource :: Setup()
{

};

void PoissonSource :: Start()
{
	arrival_rate=bandwidth/L;

	inter_packet_timer.Set(Exponential(1/arrival_rate));
};
	
void PoissonSource :: Stop()
{

};

void PoissonSource :: new_packet(trigger_t &)
{
	MPDU_packet packet;

	packet.L=(int) Exponential(L);
	
	packet.send_time = SimTime();

	out(packet);
	
	inter_packet_timer.Set(SimTime()+Exponential(1/arrival_rate));	

};




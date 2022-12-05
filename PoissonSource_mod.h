/*
	Poisson source.
*/



#include "packet.h"


double on_period = 0.5;
double off_period = 0.5;
int on_off_sources = 1;

component PoissonSource_m : public TypeII
{
	public:
		// Connections
		outport void out(MPDU_packet &packet);

		// Timer
		Timer <trigger_t> inter_packet_timer;
		Timer <trigger_t> on_off_period;

		inport inline void new_packet(trigger_t& t); // action that takes place when timer expires
		inport inline void on_off(trigger_t& t);

		PoissonSource_m () { connect inter_packet_timer.to_component,new_packet;
												connect on_off_period.to_component, on_off;};

	public:
		int id;
		double bandwidth; // Source Bandwidth
		double arrival_rate;
		double L;
		double sent_packets;
		int on;
		int packet_id;
		int counter;
		int fullBuffer;
	public:
		void Setup();
		void Start();
		void Stop();

	private:
		//void new_packet(trigger_t &t);
};

void PoissonSource_m :: Setup()
{
	printf("hello setup\n");
	
};

void PoissonSource_m :: Start()
{
	Setup();
	//printf("hola \n" );
	bandwidth = 2e3;
	L = 45e3;
	fullBuffer = 0;
	on = 1;
	arrival_rate=bandwidth/L;
	sent_packets = 0;
	packet_id = 0;
	counter = 0;

	//inter_packet_timer.Set(Exponential(1/arrival_rate));
	if(on_off_sources == 1){
		on_off_period.Set(Exponential(on_period));
	}
	if(fullBuffer == 1){
		inter_packet_timer.Set(SimTime());
	}else{
	inter_packet_timer.Set(Exponential(1/arrival_rate));

	}
};

void PoissonSource_m :: Stop()
{
	printf("Sent packets from generator = %f\n",sent_packets);

};

void PoissonSource_m :: new_packet(trigger_t &)
{
for(int i=0;i<=0;i++){
	MPDU_packet packet;

	packet.L=L;
	packet.send_time = SimTime();
	/*
	packet.tx_attempts = 0;
	packet.id = packet_id;
	packet.transmitting = 0;
	packet.tx_time = -1;
	packet.tx_time_final = -1;
	packet.assignedInterface = -1;
	packet.interruptions = 0;
	packet.assignedBO = -1;
	packet.time_of_backoff_assignment = -1;
	*/
	//printf("%f: SOURCE %d creates packet: %d \n",SimTime(),id, packet.id);
	out(packet);
	packet_id++;

	counter++;
}
	//inter_packet_timer.Set(SimTime()+Exponential(1/arrival_rate));
	if(fullBuffer == 1 && counter == 1){
		inter_packet_timer.Set((SimTime()));
	}else{
		inter_packet_timer.Set((SimTime()+Exponential(1/arrival_rate)));
	}
	sent_packets++;
	//printf("print timer: %f\n",inter_packet_timer.GetTime());

};

void PoissonSource_m :: on_off(trigger_t &){
	if(on == 1){
		on = 0;
		inter_packet_timer.Cancel();
		on_off_period.Set(SimTime()+Exponential(off_period));
		//printf(" %d,packets disabled at %f until %f\n", id,SimTime(), on_off_period.GetTime());
	}else{
		on = 1;
		inter_packet_timer.Set(SimTime()+Exponential(1/arrival_rate));
		on_off_period.Set(SimTime()+Exponential(on_period));
		//printf("%d,packets enabled at %f until %f\n", id,SimTime(),on_off_period.GetTime());
	}
};

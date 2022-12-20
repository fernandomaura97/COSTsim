

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "./COST/cost.h"

#include <deque>
#include "PoissonSource.h"
#include "PoissonSource_mod.h"
#include "QueueModule.h"
#include "QueueModule2.h"
#include "Sink.h"


//comment defined vars to disable Debug
#define DBG_QUEUE1 1
#define DBG_QUEUE2 1
#define DBG_SINK 1

//using namespace cost;

component MM1K : public CostSimEng
{
	public:
		void Setup();
		void Start();		
		void Stop();
		

	public:
		PoissonSource source;
		PoissonSource_m onof;
		QueueModule queue_module;
		QueueModule2 queue_back; 

		Sink sink2; 
		Sink sink;
};

void MM1K :: Setup()
{	

	
	connect source.out,queue_module.in;
	connect onof.out, queue_module.in; 

	connect queue_module.out,sink.in;
	connect queue_module.out, queue_back.in; //instead of passing from sink, we straight connect queue_ to queue_back in opposite direction  
	connect queue_back.out, sink2.in; 

	
	source.bandwidth = 40E3; // bps
	source.L = 5000; // bits

	//onoff settings
	onof.bandwidth = 40E3;
	onof.L = 5000;

	sink.id = 1; 
	sink2.id = 2; 

};

void MM1K:: Start()
{
	// Do nothing
}

void MM1K:: Stop()
{

}

// ---------------------------------------

int main(int argc, char *argv[])
{

	MM1K az;;

 	long int seed = 2114;

	az.Seed=seed;
	az.StopTime(1000);

	az.Setup();

	az.Run();

	return 0;
};

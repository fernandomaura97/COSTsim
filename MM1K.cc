

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "./COST/cost.h"

#include <deque>

#include "PoissonSource.h"
#include "PoissonSource2.h"
#include "QueueModule.h"
#include "Sink.h"


#define ALT
//using namespace cost;

component MM1K : public CostSimEng
{
	public:
		void Setup();
		void Start();		
		void Stop();
		

	public:
		PoissonSource source;
		QueueModule queue_module;
		Sink sink;
		#ifdef ALT
			PoissonSource2 altsource; 
		#endif
};

void MM1K :: Setup()
{
	
	connect source.out,queue_module.in;
	connect altsource.out, queue_module.in;

	connect queue_module.out,sink.in;
	
	source.L = 1000; // bits
	source.bandwidth = 40E3; // bps
	
	#ifdef ALT
		
		altsource.L = 1000; //bits
		altsource.bandwidth = 40E3; 
	#endif
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

	MM1K az;

 	long int seed = 2114;

	az.Seed=seed;
	az.StopTime(1000);

	az.Setup();

	az.Run();

	return 0;
};

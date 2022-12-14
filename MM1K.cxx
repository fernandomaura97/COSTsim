
#line 3 "MM1K.cc"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#line 1 "./COST/cost.h"

























#ifndef queue_t
#define queue_t SimpleQueue
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <deque>
#include <vector>
#include <assert.h>


#line 1 "./COST/priority_q.h"























#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include <stdio.h>
#include <string.h>














template < class ITEM >
class SimpleQueue 
{
 public:
  SimpleQueue() :m_head(NULL) {};
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  void Delete(ITEM*);
  ITEM* NextEvent() const { return m_head; };
  const char* GetName();
 protected:
  ITEM* m_head;
};

template <class ITEM>
const char* SimpleQueue<ITEM>::GetName()
{
  static const char* name = "SimpleQueue";
  return name;
}

template <class ITEM>
void SimpleQueue<ITEM>::EnQueue(ITEM* item)
{
  if( m_head==NULL || item->time < m_head->time )
  {
    if(m_head!=NULL)m_head->prev=item;
    item->next=m_head;
    m_head=item;
    item->prev=NULL;
    return;
  }
    
  ITEM* i=m_head;
  while( i->next!=NULL && item->time > i->next->time)
    i=i->next;
  item->next=i->next;
  if(i->next!=NULL)i->next->prev=item;
  i->next=item;
  item->prev=i;

}

template <class ITEM>
ITEM* SimpleQueue<ITEM> ::DeQueue()
{
  if(m_head==NULL)return NULL;
  ITEM* item = m_head;
  m_head=m_head->next;
  if(m_head!=NULL)m_head->prev=NULL;
  return item;
}

template <class ITEM>
void SimpleQueue<ITEM>::Delete(ITEM* item)
{
  if(item==NULL) return;

  if(item==m_head)
  {
    m_head=m_head->next;
    if(m_head!=NULL)m_head->prev=NULL;
  }
  else
  {
    item->prev->next=item->next;
    if(item->next!=NULL)
      item->next->prev=item->prev;
  }

}

template <class ITEM>
class GuardedQueue : public SimpleQueue<ITEM>
{
 public:
  void Delete(ITEM*);
  void EnQueue(ITEM*);
  bool Validate(const char*);
};
template <class ITEM>
void GuardedQueue<ITEM>::EnQueue(ITEM* item)
{

  ITEM* i=SimpleQueue<ITEM>::m_head;
  while(i!=NULL)
  {
    if(i==item)
    {
      pthread_printf("queue error: item %f(%p) is already in the queue\n",item->time,item);
    }
    i=i->next;
  }
  SimpleQueue<ITEM>::EnQueue(item);
}

template <class ITEM>
void GuardedQueue<ITEM>::Delete(ITEM* item)
{
  ITEM* i=SimpleQueue<ITEM>::m_head;
  while(i!=item&&i!=NULL)
    i=i->next;
  if(i==NULL)
    pthread_printf("error: cannot find the to-be-deleted event %f(%p)\n",item->time,item);
  else
    SimpleQueue<ITEM>::Delete(item);
}

template <class ITEM>
bool GuardedQueue<ITEM>::Validate(const char* s)
{
  char out[1000],buff[100];

  ITEM* i=SimpleQueue<ITEM>::m_head;
  bool qerror=false;

  sprintf(out,"queue error %s : ",s);
  while(i!=NULL)
  {
    sprintf(buff,"%f ",i->time);
    strcat(out,buff);
    if(i->next!=NULL)
      if(i->next->prev!=i)
      {
	qerror=true;
	sprintf(buff," {broken} ");
	strcat(out,buff);
      }
    if(i==i->next)
    {
      qerror=true;
      sprintf(buff,"{loop}");
      strcat(out,buff);
      break;
    }
    i=i->next;
  }
  if(qerror)
    printf("%s\n",out);
  return qerror;
}

template <class ITEM>
class ErrorQueue : public SimpleQueue<ITEM>
{
 public:
  ITEM* DeQueue(double);
  const char* GetName();
};

template <class ITEM>
const char* ErrorQueue<ITEM>::GetName()
{
  static const char* name = "ErrorQueue";
  return name;
}

template <class ITEM>
ITEM* ErrorQueue<ITEM> ::DeQueue(double stoptime)
{
  

  if(drand48()>0.5)
    return SimpleQueue<ITEM>::DeQueue();

  int s=0;
  ITEM* e;
  e=SimpleQueue<ITEM>::m_head;
  while(e!=NULL&&e->time<stoptime)
  {
    s++;
    e=e->next;
  }
  e=SimpleQueue<ITEM>::m_head;
  s=(int)(s*drand48());
  while(s!=0)
  {
    e=e->next;
    s--;
  }
  Delete(e);
  return e;
}

template < class ITEM >
class HeapQueue 
{
 public:
  HeapQueue();
  ~HeapQueue();
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  void Delete(ITEM*);
  const char* GetName();
  ITEM* NextEvent() const { return num_of_elems?elems[0]:NULL; };
 private:
  void SiftDown(int);
  void PercolateUp(int);
  void Validate(const char*);
        
  ITEM** elems;
  int num_of_elems;
  int curr_max;
};

template <class ITEM>
const char* HeapQueue<ITEM>::GetName()
{
  static const char* name = "HeapQueue";
  return name;
}

template <class ITEM>
void HeapQueue<ITEM>::Validate(const char* s)
{
  int i,j;
  char out[1000],buff[100];
  for(i=0;i<num_of_elems;i++)
    if(  ((2*i+1)<num_of_elems&&elems[i]->time>elems[2*i+1]->time) ||
	 ((2*i+2)<num_of_elems&&elems[i]->time>elems[2*i+2]->time) )
    {
      sprintf(out,"queue error %s : ",s);
      for(j=0;j<num_of_elems;j++)
      {
	if(i!=j)
	  sprintf(buff,"%f(%d) ",elems[j]->time,j);
	else
	  sprintf(buff,"{%f(%d)} ",elems[j]->time,j);
	strcat(out,buff);
      }
      printf("%s\n",out);
    }
}
template <class ITEM>
HeapQueue<ITEM>::HeapQueue()
{
  curr_max=16;
  elems=new ITEM*[curr_max];
  num_of_elems=0;
}
template <class ITEM>
HeapQueue<ITEM>::~HeapQueue()
{
  delete [] elems;
}
template <class ITEM>
void HeapQueue<ITEM>::SiftDown(int node)
{
  if(num_of_elems<=1) return;
  int i=node,k,c1,c2;
  ITEM* temp;
        
  do{
    k=i;
    c1=c2=2*i+1;
    c2++;
    if(c1<num_of_elems && elems[c1]->time < elems[i]->time)
      i=c1;
    if(c2<num_of_elems && elems[c2]->time < elems[i]->time)
      i=c2;
    if(k!=i)
    {
      temp=elems[i];
      elems[i]=elems[k];
      elems[k]=temp;
      elems[k]->pos=k;
      elems[i]->pos=i;
    }
  }while(k!=i);
}
template <class ITEM>
void HeapQueue<ITEM>::PercolateUp(int node)
{
  int i=node,k,p;
  ITEM* temp;
        
  do{
    k=i;
    if( (p=(i+1)/2) != 0)
    {
      --p;
      if(elems[i]->time < elems[p]->time)
      {
	i=p;
	temp=elems[i];
	elems[i]=elems[k];
	elems[k]=temp;
	elems[k]->pos=k;
	elems[i]->pos=i;
      }
    }
  }while(k!=i);
}

template <class ITEM>
void HeapQueue<ITEM>::EnQueue(ITEM* item)
{
  if(num_of_elems>=curr_max)
  {
    curr_max*=2;
    ITEM** buffer=new ITEM*[curr_max];
    for(int i=0;i<num_of_elems;i++)
      buffer[i]=elems[i];
    delete[] elems;
    elems=buffer;
  }
        
  elems[num_of_elems]=item;
  elems[num_of_elems]->pos=num_of_elems;
  num_of_elems++;
  PercolateUp(num_of_elems-1);
}

template <class ITEM>
ITEM* HeapQueue<ITEM>::DeQueue()
{
  if(num_of_elems<=0)return NULL;
        
  ITEM* item=elems[0];
  num_of_elems--;
  elems[0]=elems[num_of_elems];
  elems[0]->pos=0;
  SiftDown(0);
  return item;
}

template <class ITEM>
void HeapQueue<ITEM>::Delete(ITEM* item)
{
  int i=item->pos;

  num_of_elems--;
  elems[i]=elems[num_of_elems];
  elems[i]->pos=i;
  SiftDown(i);
  PercolateUp(i);
}



#define CQ_MAX_SAMPLES 25

template <class ITEM>
class CalendarQueue 
{
 public:
  CalendarQueue();
  const char* GetName();
  ~CalendarQueue();
  void enqueue(ITEM*);
  ITEM* dequeue();
  void EnQueue(ITEM*);
  ITEM* DeQueue();
  ITEM* NextEvent() const { return m_head;}
  void Delete(ITEM*);
 private:
  long last_bucket,number_of_buckets;
  double bucket_width;
        
  void ReSize(long);
  double NewWidth();

  ITEM ** buckets;
  long total_number;
  double bucket_top;
  long bottom_threshold;
  long top_threshold;
  double last_priority;
  bool resizable;

  ITEM* m_head;
  char m_name[100];
};


template <class ITEM>
const char* CalendarQueue<ITEM> :: GetName()
{
  sprintf(m_name,"Calendar Queue (bucket width: %.2e, size: %ld) ",
	  bucket_width,number_of_buckets);
  return m_name;
}
template <class ITEM>
CalendarQueue<ITEM>::CalendarQueue()
{
  long i;
        
  number_of_buckets=16;
  bucket_width=1.0;
  bucket_top=bucket_width;
  total_number=0;
  last_bucket=0;
  last_priority=0.0;
  top_threshold=number_of_buckets*2;
  bottom_threshold=number_of_buckets/2-2;
  resizable=true;
        
  buckets= new ITEM*[number_of_buckets];
  for(i=0;i<number_of_buckets;i++)
    buckets[i]=NULL;
  m_head=NULL;

}
template <class ITEM>
CalendarQueue<ITEM>::~CalendarQueue()
{
  delete [] buckets;
}
template <class ITEM>
void CalendarQueue<ITEM>::ReSize(long newsize)
{
  long i;
  ITEM** old_buckets=buckets;
  long old_number=number_of_buckets;
        
  resizable=false;
  bucket_width=NewWidth();
  buckets= new ITEM*[newsize];
  number_of_buckets=newsize;
  for(i=0;i<newsize;i++)
    buckets[i]=NULL;
  last_bucket=0;
  total_number=0;

  
        
  ITEM *item;
  for(i=0;i<old_number;i++)
  {
    while(old_buckets[i]!=NULL)
    {
      item=old_buckets[i];
      old_buckets[i]=item->next;
      enqueue(item);
    }
  }
  resizable=true;
  delete[] old_buckets;
  number_of_buckets=newsize;
  top_threshold=number_of_buckets*2;
  bottom_threshold=number_of_buckets/2-2;
  bucket_top=bucket_width*((long)(last_priority/bucket_width)+1)+bucket_width*0.5;
  last_bucket = long(last_priority/bucket_width) % number_of_buckets;

}
template <class ITEM>
ITEM* CalendarQueue<ITEM>::DeQueue()
{
  ITEM* head=m_head;
  m_head=dequeue();
  return head;
}
template <class ITEM>
ITEM* CalendarQueue<ITEM>::dequeue()
{
  long i;
  for(i=last_bucket;;)
  {
    if(buckets[i]!=NULL&&buckets[i]->time<bucket_top)
    {
      ITEM * item=buckets[i];
      buckets[i]=buckets[i]->next;
      total_number--;
      last_bucket=i;
      last_priority=item->time;
                        
      if(resizable&&total_number<bottom_threshold)
	ReSize(number_of_buckets/2);
      item->next=NULL;
      return item;
    }
    else
    {
      i++;
      if(i==number_of_buckets)i=0;
      bucket_top+=bucket_width;
      if(i==last_bucket)
	break;
    }
  }
        
  
  long smallest;
  for(smallest=0;smallest<number_of_buckets;smallest++)
    if(buckets[smallest]!=NULL)break;

  if(smallest >= number_of_buckets)
  {
    last_priority=bucket_top;
    return NULL;
  }

  for(i=smallest+1;i<number_of_buckets;i++)
  {
    if(buckets[i]==NULL)
      continue;
    else
      if(buckets[i]->time<buckets[smallest]->time)
	smallest=i;
  }
  ITEM * item=buckets[smallest];
  buckets[smallest]=buckets[smallest]->next;
  total_number--;
  last_bucket=smallest;
  last_priority=item->time;
  bucket_top=bucket_width*((long)(last_priority/bucket_width)+1)+bucket_width*0.5;
  item->next=NULL;
  return item;
}
template <class ITEM>
void CalendarQueue<ITEM>::EnQueue(ITEM* item)
{
  
  if(m_head==NULL)
  {
    m_head=item;
    return;
  }
  if(m_head->time>item->time)
  {
    enqueue(m_head);
    m_head=item;
  }
  else
    enqueue(item);
}
template <class ITEM>
void CalendarQueue<ITEM>::enqueue(ITEM* item)
{
  long i;
  if(item->time<last_priority)
  {
    i=(long)(item->time/bucket_width);
    last_priority=item->time;
    bucket_top=bucket_width*(i+1)+bucket_width*0.5;
    i=i%number_of_buckets;
    last_bucket=i;
  }
  else
  {
    i=(long)(item->time/bucket_width);
    i=i%number_of_buckets;
  }

        
  

  if(buckets[i]==NULL||item->time<buckets[i]->time)
  {
    item->next=buckets[i];
    buckets[i]=item;
  }
  else
  {

    ITEM* pos=buckets[i];
    while(pos->next!=NULL&&item->time>pos->next->time)
    {
      pos=pos->next;
    }
    item->next=pos->next;
    pos->next=item;
  }
  total_number++;
  if(resizable&&total_number>top_threshold)
    ReSize(number_of_buckets*2);
}
template <class ITEM>
void CalendarQueue<ITEM>::Delete(ITEM* item)
{
  if(item==m_head)
  {
    m_head=dequeue();
    return;
  }
  long j;
  j=(long)(item->time/bucket_width);
  j=j%number_of_buckets;
        
  

  
  

  ITEM** p = &buckets[j];
  
  ITEM* i=buckets[j];
    
  while(i!=NULL)
  {
    if(i==item)
    { 
      (*p)=item->next;
      total_number--;
      if(resizable&&total_number<bottom_threshold)
	ReSize(number_of_buckets/2);
      return;
    }
    p=&(i->next);
    i=i->next;
  }   
}
template <class ITEM>
double CalendarQueue<ITEM>::NewWidth()
{
  long i, nsamples;
        
  if(total_number<2) return 1.0;
  if(total_number<=5)
    nsamples=total_number;
  else
    nsamples=5+total_number/10;
  if(nsamples>CQ_MAX_SAMPLES) nsamples=CQ_MAX_SAMPLES;
        
  long _last_bucket=last_bucket;
  double _bucket_top=bucket_top;
  double _last_priority=last_priority;
        
  double AVG[CQ_MAX_SAMPLES],avg1=0,avg2=0;
  ITEM* list,*next,*item;
        
  list=dequeue(); 
  long real_samples=0;
  while(real_samples<nsamples)
  {
    item=dequeue();
    if(item==NULL)
    {
      item=list;
      while(item!=NULL)
      {
	next=item->next;
	enqueue(item);
	item=next;      
      }

      last_bucket=_last_bucket;
      bucket_top=_bucket_top;
      last_priority=_last_priority;

                        
      return 1.0;
    }
    AVG[real_samples]=item->time-list->time;
    avg1+=AVG[real_samples];
    if(AVG[real_samples]!=0.0)
      real_samples++;
    item->next=list;
    list=item;
  }
  item=list;
  while(item!=NULL)
  {
    next=item->next;
    enqueue(item);
    item=next;      
  }
        
  last_bucket=_last_bucket;
  bucket_top=_bucket_top;
  last_priority=_last_priority;
        
  avg1=avg1/(double)(real_samples-1);
  avg1=avg1*2.0;
        
  
  long count=0;
  for(i=0;i<real_samples-1;i++)
  {
    if(AVG[i]<avg1&&AVG[i]!=0)
    {
      avg2+=AVG[i];
      count++;
    }
  }
  if(count==0||avg2==0)   return 1.0;
        
  avg2 /= (double) count;
  avg2 *= 3.0;
        
  return avg2;
}

#endif /*PRIORITY_QUEUE_H*/

#line 38 "./COST/cost.h"


#line 1 "./COST/corsa_alloc.h"
































#ifndef corsa_allocator_h
#define corsa_allocator_h

#include <typeinfo>
#include <string>

class CorsaAllocator
{
private:
    struct DT{
#ifdef CORSA_DEBUG
	DT* self;
#endif
	DT* next;
    };
public:
    CorsaAllocator(unsigned int );         
    CorsaAllocator(unsigned int, int);     
    ~CorsaAllocator();		
    void *alloc();		
    void free(void*);
    unsigned int datasize() 
    {
#ifdef CORSA_DEBUG
	return m_datasize-sizeof(DT*);
#else
	return m_datasize; 
#endif
    }
    int size() { return m_size; }
    int capacity() { return m_capacity; }			
    
    const char* GetName() { return m_name.c_str(); }
    void SetName( const char* name) { m_name=name; } 

private:
    CorsaAllocator(const CorsaAllocator& ) {}  
    void Setup(unsigned int,int); 
    void InitSegment(int);
  
    unsigned int m_datasize;
    char** m_segments;	          
    int m_segment_number;         
    int m_segment_max;      
    int m_segment_size;	          
				  
    DT* m_free_list; 
    int m_size;
    int m_capacity;

    int m_free_times,m_alloc_times;
    int m_max_allocs;

    std::string m_name;
};
#ifndef CORSA_NODEF
CorsaAllocator::CorsaAllocator(unsigned int datasize)
{
    Setup(datasize,256);	  
}

CorsaAllocator::CorsaAllocator(unsigned int datasize, int segsize)
{
    Setup(datasize,segsize);
}

CorsaAllocator::~CorsaAllocator()
{
    #ifdef CORSA_DEBUG
    printf("%s -- alloc: %d, free: %d, max: %d\n",GetName(),
	   m_alloc_times,m_free_times,m_max_allocs);
    #endif

    for(int i=0;i<m_segment_number;i++)
	delete[] m_segments[i];	   
    delete[] m_segments;			
}

void CorsaAllocator::Setup(unsigned int datasize,int seg_size)
{

    char buffer[50];
    sprintf(buffer,"%s[%d]",typeid(*this).name(),datasize);
    m_name = buffer;

#ifdef CORSA_DEBUG
    datasize+=sizeof(DT*);  
#endif

    if(datasize<sizeof(DT))datasize=sizeof(DT);
    m_datasize=datasize;
    if(seg_size<16)seg_size=16;    
    m_segment_size=seg_size;			
    m_segment_number=1;		   
    m_segment_max=seg_size;	   
    m_segments= new char* [ m_segment_max ] ;   
    m_segments[0]= new char [m_segment_size*m_datasize];  

    m_size=0;
    m_capacity=0;
    InitSegment(0);

    m_free_times=m_alloc_times=m_max_allocs=00;
}

void CorsaAllocator::InitSegment(int s)
{
    char* p=m_segments[s];
    m_free_list=reinterpret_cast<DT*>(p);
    for(int i=0;i<m_segment_size-1;i++,p+=m_datasize)
    {
	reinterpret_cast<DT*>(p)->next=
	    reinterpret_cast<DT*>(p+m_datasize);
    }
    reinterpret_cast<DT*>(p)->next=NULL;
    m_capacity+=m_segment_size;
}

void* CorsaAllocator::alloc()
{
    #ifdef CORSA_DEBUG
    m_alloc_times++;
    if(m_alloc_times-m_free_times>m_max_allocs)
	m_max_allocs=m_alloc_times-m_free_times;
    #endif
    if(m_free_list==NULL)	
    
    {
	int i;
	if(m_segment_number==m_segment_max)	
	
	
	{
	    m_segment_max*=2;		
	    char** buff;
	    buff=new char* [m_segment_max];   
#ifdef CORSA_DEBUG
	    if(buff==NULL)
	    {
		printf("CorsaAllocator runs out of memeory.\n");
		exit(1);
	    }
#endif
	    for(i=0;i<m_segment_number;i++)
		buff[i]=m_segments[i];	
	    delete [] m_segments;		
	    m_segments=buff;
	}
	m_segment_size*=2;
	m_segments[m_segment_number]=new char[m_segment_size*m_datasize];
#ifdef CORSA_DEBUG
	    if(m_segments[m_segment_number]==NULL)
	    {
		printf("CorsaAllocator runs out of memeory.\n");
		exit(1);
	    }
#endif
	InitSegment(m_segment_number);
	m_segment_number++;
    }

    DT* item=m_free_list;		
    m_free_list=m_free_list->next;
    m_size++;

#ifdef CORSA_DEBUG
    item->self=item;
    char* p=reinterpret_cast<char*>(item);
    p+=sizeof(DT*);
    
    return static_cast<void*>(p);
#else
    return static_cast<void*>(item);
#endif
}

void CorsaAllocator::free(void* data)
{
#ifdef CORSA_DEBUG
    m_free_times++;
    char* p=static_cast<char*>(data);
    p-=sizeof(DT*);
    DT* item=reinterpret_cast<DT*>(p);
    
    if(item!=item->self)
    {
	if(item->self==(DT*)0xabcd1234)
	    printf("%s: packet at %p has already been released\n",GetName(),p+sizeof(DT*)); 
	else
	    printf("%s: %p is probably not a pointer to a packet\n",GetName(),p+sizeof(DT*));
    }
    assert(item==item->self);
    item->self=(DT*)0xabcd1234;
#else
    DT* item=static_cast<DT*>(data);
#endif

    item->next=m_free_list;
    m_free_list=item;
    m_size--;
}
#endif /* CORSA_NODEF */

#endif /* corsa_allocator_h */

#line 39 "./COST/cost.h"


class trigger_t {};
typedef double simtime_t;

#ifdef COST_DEBUG
#define Printf(x) Print x
#else
#define Printf(x)
#endif



class TimerBase;



struct CostEvent
{
  double time;
  CostEvent* next;
  union {
    CostEvent* prev;
    int pos;  
  };
  TimerBase* object;
  int index;
  unsigned char active;
};



class TimerBase
{
 public:
  virtual void activate(CostEvent*) = 0;
  inline virtual ~TimerBase() {}	
};

class TypeII;



class CostSimEng
{
 public:

  class seed_t
      {
       public:
	void operator = (long seed) { srand48(seed); };
      };
  seed_t		Seed;
  CostSimEng()
      : stopTime( 0), clearStatsTime( 0), m_clock( 0.0)
      {
        if( m_instance == NULL)
	  m_instance = this;
        else
	  printf("Error: only one simulation engine can be created\n");
      }
  virtual		~CostSimEng()	{ }
  static CostSimEng	*Instance()
      {
        if(m_instance==NULL)
        {
	  printf("Error: a simulation engine has not been initialized\n");
	  m_instance = new CostSimEng;
        }
        return m_instance;
      }
  CorsaAllocator	*GetAllocator(unsigned int datasize)
      {
    	for(unsigned int i=0;i<m_allocators.size();i++)
    	{
	  if(m_allocators[i]->datasize()==datasize)return m_allocators[i];
    	} 
    	CorsaAllocator* allocator=new CorsaAllocator(datasize);
    	char buffer[25];
    	sprintf(buffer,"EventAllocator[%d]",datasize);
    	allocator->SetName(buffer);
    	m_allocators.push_back(allocator);
    	return allocator;
      }
  void		AddComponent(TypeII*c)
      {
        m_components.push_back(c);
      }
  void		ScheduleEvent(CostEvent*e)
      {
	if( e->time < m_clock)
	  assert(e->time>=m_clock);
        
        m_queue.EnQueue(e);
      }
  void		CancelEvent(CostEvent*e)
      {
        
        m_queue.Delete(e);
      }
  double	Random( double v=1.0)	{ return v*drand48();}
  int		Random( int v)		{ return (int)(v*drand48()); }
  double	Exponential(double mean)	{ return -mean*log(Random());}
  virtual void	Start()		{}
  virtual void	Stop()		{}
  void		Run();
  double	SimTime()	{ return m_clock; } 
  void		StopTime( double t)	{ stopTime = t; }
  double	StopTime() const	{ return stopTime; }
  void		ClearStatsTime( double t)	{ clearStatsTime = t; }
  double	ClearStatsTime() const	{ return clearStatsTime; }
  virtual void	ClearStats()	{}
 private:
  double	stopTime;
  double	clearStatsTime;	
  double	eventRate;
  double	runningTime;
  long		eventsProcessed;
  double	m_clock;
  queue_t<CostEvent>	m_queue;
  std::vector<TypeII*>	m_components;
  static CostSimEng	*m_instance;
  std::vector<CorsaAllocator*>	m_allocators;
};




class TypeII
{
 public: 
  virtual void Start() {};
  virtual void Stop() {};
  inline virtual ~TypeII() {}		
  TypeII()
      {
        m_simeng=CostSimEng::Instance();
        m_simeng->AddComponent(this);
      }

#ifdef COST_DEBUG
  void Print(const bool, const char*, ...);
#endif
    
  double Random(double v=1.0) { return v*drand48();}
  int Random(int v) { return (int)(v*drand48());}
  double Exponential(double mean) { return -mean*log(Random());}
  inline double SimTime() const { return m_simeng->SimTime(); }
  inline double StopTime() const { return m_simeng->StopTime(); }
 private:
  CostSimEng* m_simeng;
}; 

#ifdef COST_DEBUG
void TypeII::Print(const bool flag, const char* format, ...)
{
  if(flag==false) return;
  va_list ap;
  va_start(ap, format);
  printf("[%.10f] ",SimTime());
  vprintf(format,ap);
  va_end(ap);
}
#endif

CostSimEng* CostSimEng::m_instance = NULL;

void CostSimEng::Run()
{
  double	nextTime = (clearStatsTime != 0.0 && clearStatsTime < stopTime) ? clearStatsTime : stopTime;

  m_clock = 0.0;
  eventsProcessed = 0l;
  std::vector<TypeII*>::iterator iter;
      
  struct timeval start_time;    
  gettimeofday( &start_time, NULL);

  Start();

  for( iter = m_components.begin(); iter != m_components.end(); iter++)
    (*iter)->Start();

  CostEvent* e=m_queue.DeQueue();
  while( e != NULL)
  {
    if( e->time >= nextTime)
    {
      if( nextTime == stopTime)
	break;
      
      printf( "Clearing statistics @ %f\n", nextTime);
      nextTime = stopTime;
      ClearStats();
    }
    
    assert( e->time >= m_clock);
    m_clock = e->time;
    e->object->activate( e);
    eventsProcessed++;
    e = m_queue.DeQueue();
  }
  m_clock = stopTime;
  for(iter = m_components.begin(); iter != m_components.end(); iter++)
    (*iter)->Stop();
	    
  Stop();

  struct timeval stop_time;    
  gettimeofday(&stop_time,NULL);

  runningTime = stop_time.tv_sec - start_time.tv_sec +
      (stop_time.tv_usec - start_time.tv_usec) / 1000000.0;
  eventRate = eventsProcessed/runningTime;
  
  
  printf("# -------------------------------------------------------------------------\n");	
  printf("# CostSimEng with %s, stopped at %f\n", m_queue.GetName(), stopTime);	
  printf("# %ld events processed in %.3f seconds, event processing rate: %.0f\n",	
  eventsProcessed, runningTime, eventRate);
  
}







#line 8 "MM1K.cc"


#include <deque>

#line 1 "PoissonSource.h"





		

#line 1 "packet.h"
#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; 
	double send_time; 
	bool wayback;
};


#endif 


#line 7 "PoissonSource.h"



#line 39 "PoissonSource.h"
;


#line 46 "PoissonSource.h"
;
	

#line 51 "PoissonSource.h"
;


#line 65 "PoissonSource.h"
;




#line 11 "MM1K.cc"


#line 1 "PoissonSource_mod.h"







#line 1 "packet.h"
#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; 
	double send_time; 
	bool wayback;
};


#endif 


#line 7 "PoissonSource_mod.h"



double on_period = 0.9;
double off_period = 0.1;
int on_off_sources = 1;


#line 53 "PoissonSource_mod.h"
;


#line 76 "PoissonSource_mod.h"
;


#line 82 "PoissonSource_mod.h"
;


#line 118 "PoissonSource_mod.h"
;


#line 132 "PoissonSource_mod.h"
;

#line 12 "MM1K.cc"


#line 1 "QueueModule.h"




#line 1 "packet.h"
#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; 
	double send_time; 
	bool wayback;
};


#endif 


#line 4 "QueueModule.h"


#line 1 "FIFO.h"

#ifndef _FIFO_QUEUE_
#define _FIFO_QUEUE_




#line 1 "packet.h"
#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; 
	double send_time; 
	bool wayback;
};


#endif 


#line 7 "FIFO.h"

#include <deque>







#line 34 "FIFO.h"
; 


#line 39 "FIFO.h"
; 



#line 45 "FIFO.h"
; 


#line 50 "FIFO.h"
; 


#line 55 "FIFO.h"
; 


#line 60 "FIFO.h"
; 


#line 65 "FIFO.h"
; 


#line 70 "FIFO.h"
;


#endif

#line 5 "QueueModule.h"


#define Q 30
#define C 100E3


#line 43 "QueueModule.h"
;


#line 51 "QueueModule.h"
;


#line 63 "QueueModule.h"
;


#line 81 "QueueModule.h"
;


#line 95 "QueueModule.h"
;




#line 13 "MM1K.cc"


#line 1 "QueueModule2.h"






#line 1 "packet.h"
#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; 
	double send_time; 
	bool wayback;
};


#endif 


#line 6 "QueueModule2.h"


#line 1 "FIFO2.h"

#ifndef _FIFO_QUEUE_
#define _FIFO_QUEUE_




#line 1 "packet.h"
#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; 
	double send_time; 
	bool wayback;
};


#endif 


#line 7 "FIFO2.h"

#include <deque>







#line 34 "FIFO2.h"
; 


#line 39 "FIFO2.h"
; 



#line 45 "FIFO2.h"
; 


#line 50 "FIFO2.h"
; 


#line 55 "FIFO2.h"
; 


#line 60 "FIFO2.h"
; 


#line 65 "FIFO2.h"
; 


#line 70 "FIFO2.h"
;


#endif

#line 7 "QueueModule2.h"

 




#line 45 "QueueModule2.h"
;


#line 53 "QueueModule2.h"
;


#line 62 "QueueModule2.h"
;


#line 80 "QueueModule2.h"
;


#line 94 "QueueModule2.h"
;




#line 14 "MM1K.cc"


#line 1 "Sink.h"




#ifndef _SINK_H
#define _SINK_H


#line 1 "packet.h"
#ifndef _MPDU_PACKET_
#define _MPDU_PACKET_

struct MPDU_packet
{
	int L; 
	double send_time; 
	bool wayback;
};


#endif 


#line 8 "Sink.h"



#line 33 "Sink.h"
;


#line 40 "Sink.h"
;


#line 59 "Sink.h"
;


#line 69 "Sink.h"
;

#endif

#line 15 "MM1K.cc"




#define DBG_QUEUE1 1
#define DBG_QUEUE2 1
#define DBG_SINK 1




#line 65 "MM1K.cc"
;

#include "compcxx_MM1K.h"
class compcxx_PoissonSource_m_10;/*template <class T> */
#line 267 "./COST/cost.h"
class compcxx_Timer_3 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_3() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_PoissonSource_m_10* p_compcxx_parent;};

class compcxx_PoissonSource_m_10;/*template <class T> */
#line 267 "./COST/cost.h"
class compcxx_Timer_4 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_4() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_PoissonSource_m_10* p_compcxx_parent;};

class compcxx_QueueModule_11;
#line 14 "PoissonSource_mod.h"
class compcxx_PoissonSource_m_10 : public compcxx_component, public TypeII
{
	public:
		
		/*outport void out(MPDU_packet &packet)*/;

		
		compcxx_Timer_3 /*<trigger_t> */inter_packet_timer;
		compcxx_Timer_4 /*<trigger_t> */on_off_period;

		/*inport */inline void new_packet(trigger_t& t); 
		/*inport */inline void on_off(trigger_t& t);

		compcxx_PoissonSource_m_10 () { inter_packet_timer.p_compcxx_parent=this /*connect inter_packet_timer.to_component,*/;
												on_off_period.p_compcxx_parent=this /*connect on_off_period.to_component, */;};

	public:
		int id;
		double bandwidth; 
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
		
public:compcxx_QueueModule_11* p_compcxx_QueueModule_11;};


#line 15 "FIFO.h"
class compcxx_FIFO_8 : public compcxx_component, public TypeII
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

class compcxx_QueueModule2_12;/*template <class T> */
#line 267 "./COST/cost.h"
class compcxx_Timer_7 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_7() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_QueueModule2_12* p_compcxx_parent;};

class compcxx_Sink_13;
#line 12 "QueueModule2.h"
class compcxx_QueueModule2_12 : public compcxx_component, public TypeII
{
	public:
		
		/*inport */void in(MPDU_packet &packet);
		/*outport void out(MPDU_packet &packet)*/;

		
		compcxx_Timer_7 /*<trigger_t> */service_time;
		/*inport */inline void endService(trigger_t& t);

		compcxx_QueueModule2_12 () { service_time.p_compcxx_parent=this /*connect service_time.to_component,*/; }


	private:
		compcxx_FIFO_8 queue;

	public:
		double blocked_packets;
		double arrived_packets;
		double queue_length;

	public:
		void Setup();
		void Start();
		void Stop();


public:compcxx_Sink_13* p_compcxx_Sink_13;};


#line 15 "FIFO.h"
class compcxx_FIFO_6 : public compcxx_component, public TypeII
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

class compcxx_QueueModule_11;/*template <class T> */
#line 267 "./COST/cost.h"
class compcxx_Timer_5 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_5() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_QueueModule_11* p_compcxx_parent;};

class compcxx_Sink_14;class compcxx_QueueModule2_12;
#line 10 "QueueModule.h"
class compcxx_QueueModule_11 : public compcxx_component, public TypeII
{
	public:
		
		/*inport */void in(MPDU_packet &packet);
		/*outport void out(MPDU_packet &packet)*/;

		
		compcxx_Timer_5 /*<trigger_t> */service_time;
		/*inport */inline void endService(trigger_t& t);

		compcxx_QueueModule_11 () { service_time.p_compcxx_parent=this /*connect service_time.to_component,*/; }


	private:
		compcxx_FIFO_6 queue;

	public:
		double blocked_packets;
		double arrived_packets;
		double queue_length;

	public:
		void Setup();
		void Start();
		void Stop();


public:compcxx_Sink_14* p_compcxx_Sink_14;public:compcxx_QueueModule2_12* p_compcxx_QueueModule2_12;};


#line 10 "Sink.h"
class compcxx_Sink_14 : public compcxx_component, public TypeII
{
	public:
		
		/*inport */void in(MPDU_packet &packet);

	public:
		void Setup();
		void Start();
		void Stop();
		

	public:
		double system_time;
		double received_packets;
		double av_L; 
		int id;

};


#line 10 "Sink.h"
class compcxx_Sink_13 : public compcxx_component, public TypeII
{
	public:
		
		/*inport */void in(MPDU_packet &packet);

	public:
		void Setup();
		void Start();
		void Stop();
		

	public:
		double system_time;
		double received_packets;
		double av_L; 
		int id;

};

class compcxx_PoissonSource_9;/*template <class T> */
#line 267 "./COST/cost.h"
class compcxx_Timer_2 : public compcxx_component, public TimerBase
{
 public:
  struct event_t : public CostEvent { trigger_t data; };
  

  compcxx_Timer_2() { m_simeng = CostSimEng::Instance(); m_event.active= false; }
  inline void Set(trigger_t const &, double );
  inline void Set(double );
  inline double GetTime() { return m_event.time; }
  inline bool Active() { return m_event.active; }
  inline trigger_t & GetData() { return m_event.data; }
  inline void SetData(trigger_t const &d) { m_event.data = d; }
  void Cancel();
  /*outport void to_component(trigger_t &)*/;
  void activate(CostEvent*);
 private:
  CostSimEng* m_simeng;
  event_t m_event;
public:compcxx_PoissonSource_9* p_compcxx_parent;};

class compcxx_QueueModule_11;
#line 9 "PoissonSource.h"
class compcxx_PoissonSource_9 : public compcxx_component, public TypeII
{
	public:
		
		/*outport void out(MPDU_packet &packet)*/;	

		
		compcxx_Timer_2 /*<trigger_t> */inter_packet_timer;
		/*inport */inline void new_packet(trigger_t& t); 

		compcxx_PoissonSource_9 () { inter_packet_timer.p_compcxx_parent=this /*connect inter_packet_timer.to_component,*/; }

	public:
		double bandwidth; 
		double arrival_rate;
		double L;

	
	public:
		void Setup();
		void Start();
		void Stop();
			
	private:
		
public:compcxx_QueueModule_11* p_compcxx_QueueModule_11;};


#line 25 "MM1K.cc"
class compcxx_MM1K_15 : public compcxx_component, public CostSimEng
{
	public:
		void Setup();
		void Start();		
		void Stop();
		

	public:
		compcxx_PoissonSource_9 source;
		compcxx_PoissonSource_m_10 onof;
		compcxx_QueueModule_11 queue_module;
		compcxx_QueueModule2_12 queue_back; 

		compcxx_Sink_13 sink2; 
		compcxx_Sink_14 sink;
};


#line 288 "./COST/cost.h"

#line 288 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "./COST/cost.h"

#line 300 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "./COST/cost.h"

#line 311 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "./COST/cost.h"

#line 319 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_3/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->new_packet(m_event.data));
}




#line 288 "./COST/cost.h"

#line 288 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "./COST/cost.h"

#line 300 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "./COST/cost.h"

#line 311 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "./COST/cost.h"

#line 319 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_4/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->on_off(m_event.data));
}




#line 24 "PoissonSource_mod.h"

#line 25 "PoissonSource_mod.h"

#line 49 "PoissonSource_mod.h"
void compcxx_PoissonSource_m_10 :: Setup()
{
	printf("hello setup\n");
	
}
#line 55 "PoissonSource_mod.h"
void compcxx_PoissonSource_m_10 :: Start()
{
	Setup();
	
	
	fullBuffer = 0;
	on = 1;
	arrival_rate=bandwidth/L;
	sent_packets = 0;
	packet_id = 0;
	counter = 0;

	
	if(on_off_sources == 1){
		on_off_period.Set(Exponential(on_period));
	}
	if(fullBuffer == 1){
		inter_packet_timer.Set(SimTime());
	}else{
		inter_packet_timer.Set(Exponential(1/arrival_rate));
	}
}
#line 78 "PoissonSource_mod.h"
void compcxx_PoissonSource_m_10 :: Stop()
{
	printf("Sent packets from generator = %f\n",sent_packets);

}
#line 84 "PoissonSource_mod.h"
void compcxx_PoissonSource_m_10 :: new_packet(trigger_t &)
{
for(int i=0;i<=0;i++){
	MPDU_packet packet;

	packet.L=L;
	packet.send_time = SimTime();
	










	
	(p_compcxx_QueueModule_11->in(packet));
	sent_packets++;
	packet_id++;

	counter++;
}
	
	if(fullBuffer == 1 && counter == 1){
		inter_packet_timer.Set((SimTime()));
	}else{
		inter_packet_timer.Set((SimTime()+Exponential(1/arrival_rate)));
	}
	
	

}
#line 120 "PoissonSource_mod.h"
void compcxx_PoissonSource_m_10 :: on_off(trigger_t &){
	if(on == 1){
		on = 0;
		inter_packet_timer.Cancel();
		on_off_period.Set(SimTime()+Exponential(off_period));
		
	}else{
		on = 1;
		inter_packet_timer.Set(SimTime()+Exponential(1/arrival_rate));
		on_off_period.Set(SimTime()+Exponential(on_period));
		
	}
}
#line 31 "FIFO.h"
MPDU_packet compcxx_FIFO_8 :: GetFirstPacket()
{
	return(m_queue.front());	
}
#line 36 "FIFO.h"
MPDU_packet compcxx_FIFO_8 :: GetPacketAt(int n)
{
	return(m_queue.at(n));	
}
#line 42 "FIFO.h"
void compcxx_FIFO_8 :: DelFirstPacket()
{
	m_queue.pop_front();
}
#line 47 "FIFO.h"
void compcxx_FIFO_8 :: PutPacket(MPDU_packet &packet)
{	
	m_queue.push_back(packet);
}
#line 52 "FIFO.h"
void compcxx_FIFO_8 :: PutPacketFront(MPDU_packet &packet)
{	
	m_queue.push_front(packet);
}
#line 57 "FIFO.h"
int compcxx_FIFO_8 :: QueueSize()
{
	return(m_queue.size());
}
#line 62 "FIFO.h"
void compcxx_FIFO_8 :: PutPacketIn(MPDU_packet & packet,int i)
{
	m_queue.insert(m_queue.begin()+i,packet);
}
#line 67 "FIFO.h"
void compcxx_FIFO_8 :: DeletePacketIn(int i)
{
	m_queue.erase(m_queue.begin()+i);
}
#line 288 "./COST/cost.h"

#line 288 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_7/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "./COST/cost.h"

#line 300 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_7/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "./COST/cost.h"

#line 311 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_7/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "./COST/cost.h"

#line 319 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_7/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->endService(m_event.data));
}




#line 21 "QueueModule2.h"

#line 42 "QueueModule2.h"
void compcxx_QueueModule2_12 :: Setup()
{

}
#line 47 "QueueModule2.h"
void compcxx_QueueModule2_12 :: Start()
{
	
	blocked_packets = 0;
	arrived_packets = 0;
	queue_length = 0;
}
#line 55 "QueueModule2.h"
void compcxx_QueueModule2_12 :: Stop()
{
	#ifdef DBG_QUEUE2
		printf("WAY OUT:\n");
		printf("\ttest Blocking Probability = %f\n",blocked_packets/arrived_packets);
		printf("\ttest E[Queue Length] = %f\n",queue_length/arrived_packets);
	#endif
}
#line 64 "QueueModule2.h"
void compcxx_QueueModule2_12 :: in(MPDU_packet &packet)
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


}
#line 82 "QueueModule2.h"
void compcxx_QueueModule2_12 :: endService(trigger_t &)
{
	MPDU_packet packet = queue.GetFirstPacket();
	queue.DelFirstPacket();
	(p_compcxx_Sink_13->in(packet));

	if(queue.QueueSize() > 0)
	{
		MPDU_packet packet = queue.GetFirstPacket();
		service_time.Set(SimTime()+(packet.L/C));
	}

}
#line 31 "FIFO.h"
MPDU_packet compcxx_FIFO_6 :: GetFirstPacket()
{
	return(m_queue.front());	
}
#line 36 "FIFO.h"
MPDU_packet compcxx_FIFO_6 :: GetPacketAt(int n)
{
	return(m_queue.at(n));	
}
#line 42 "FIFO.h"
void compcxx_FIFO_6 :: DelFirstPacket()
{
	m_queue.pop_front();
}
#line 47 "FIFO.h"
void compcxx_FIFO_6 :: PutPacket(MPDU_packet &packet)
{	
	m_queue.push_back(packet);
}
#line 52 "FIFO.h"
void compcxx_FIFO_6 :: PutPacketFront(MPDU_packet &packet)
{	
	m_queue.push_front(packet);
}
#line 57 "FIFO.h"
int compcxx_FIFO_6 :: QueueSize()
{
	return(m_queue.size());
}
#line 62 "FIFO.h"
void compcxx_FIFO_6 :: PutPacketIn(MPDU_packet & packet,int i)
{
	m_queue.insert(m_queue.begin()+i,packet);
}
#line 67 "FIFO.h"
void compcxx_FIFO_6 :: DeletePacketIn(int i)
{
	m_queue.erase(m_queue.begin()+i);
}
#line 288 "./COST/cost.h"

#line 288 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "./COST/cost.h"

#line 300 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "./COST/cost.h"

#line 311 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "./COST/cost.h"

#line 319 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_5/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->endService(m_event.data));
}




#line 19 "QueueModule.h"

#line 40 "QueueModule.h"
void compcxx_QueueModule_11 :: Setup()
{

}
#line 45 "QueueModule.h"
void compcxx_QueueModule_11 :: Start()
{
	
	blocked_packets = 0;
	arrived_packets = 0;
	queue_length = 0;
}
#line 53 "QueueModule.h"
void compcxx_QueueModule_11 :: Stop()
{
	

	#ifdef DBG_QUEUE1
	printf("WAY IN:\n");
	printf("\ttest Blocking Probability = %f\n",blocked_packets/arrived_packets);
	printf("\ttest E[Queue Length] = %f\n",queue_length/arrived_packets);
	#endif

}
#line 65 "QueueModule.h"
void compcxx_QueueModule_11 :: in(MPDU_packet &packet)
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


}
#line 83 "QueueModule.h"
void compcxx_QueueModule_11 :: endService(trigger_t &)
{
	MPDU_packet packet = queue.GetFirstPacket();
	queue.DelFirstPacket();
	(p_compcxx_Sink_14->in(packet),p_compcxx_QueueModule2_12->in(packet));

	if(queue.QueueSize() > 0)
	{
		MPDU_packet packet = queue.GetFirstPacket();
		service_time.Set(SimTime()+(packet.L/C));
	}

}
#line 30 "Sink.h"
void compcxx_Sink_14 :: Setup()
{

}
#line 35 "Sink.h"
void compcxx_Sink_14 :: Start()
{
	system_time = 0;
	received_packets = 0;
	av_L = 0; 
}
#line 42 "Sink.h"
void compcxx_Sink_14 :: Stop()
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
}
#line 61 "Sink.h"
void compcxx_Sink_14 :: in(MPDU_packet &packet)
{
	system_time += SimTime() - packet.send_time;
	av_L += packet.L;
	received_packets++;
	#ifdef DBG_SINK
	printf("rx packets: %f, packet l: %d, av_l = %f\n", received_packets, packet.L, av_L/received_packets);
	#endif
}
#line 30 "Sink.h"
void compcxx_Sink_13 :: Setup()
{

}
#line 35 "Sink.h"
void compcxx_Sink_13 :: Start()
{
	system_time = 0;
	received_packets = 0;
	av_L = 0; 
}
#line 42 "Sink.h"
void compcxx_Sink_13 :: Stop()
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
}
#line 61 "Sink.h"
void compcxx_Sink_13 :: in(MPDU_packet &packet)
{
	system_time += SimTime() - packet.send_time;
	av_L += packet.L;
	received_packets++;
	#ifdef DBG_SINK
	printf("rx packets: %f, packet l: %d, av_l = %f\n", received_packets, packet.L, av_L/received_packets);
	#endif
}
#line 288 "./COST/cost.h"

#line 288 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Set(trigger_t const & data, double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.data = data;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 300 "./COST/cost.h"

#line 300 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Set(double time)
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.time = time;
  m_event.object = this;
  m_event.active=true;
  m_simeng->ScheduleEvent(&m_event);
}


#line 311 "./COST/cost.h"

#line 311 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::Cancel()
{
  if(m_event.active)
    m_simeng->CancelEvent(&m_event);
  m_event.active = false;
}


#line 319 "./COST/cost.h"

#line 319 "./COST/cost.h"
/*template <class T>
*/void compcxx_Timer_2/*<trigger_t >*/::activate(CostEvent*e)
{
  assert(e==&m_event);
  m_event.active=false;
  (p_compcxx_parent->new_packet(m_event.data));
}




#line 17 "PoissonSource.h"

#line 36 "PoissonSource.h"
void compcxx_PoissonSource_9 :: Setup()
{

}
#line 41 "PoissonSource.h"
void compcxx_PoissonSource_9 :: Start()
{
	arrival_rate=bandwidth/L;

	inter_packet_timer.Set(Exponential(1/arrival_rate));
}
#line 48 "PoissonSource.h"
void compcxx_PoissonSource_9 :: Stop()
{

}
#line 53 "PoissonSource.h"
void compcxx_PoissonSource_9 :: new_packet(trigger_t &)
{
	MPDU_packet packet;

	packet.L=(int) Exponential(L);
	
	packet.send_time = SimTime();

	(p_compcxx_QueueModule_11->in(packet));
	
	inter_packet_timer.Set(SimTime()+Exponential(1/arrival_rate));	

}
#line 43 "MM1K.cc"
void compcxx_MM1K_15 :: Setup()
{	

	
	source.p_compcxx_QueueModule_11=&queue_module /*connect source.out,queue_module.in*/;
	onof.p_compcxx_QueueModule_11=&queue_module /*connect onof.out, queue_module.in*/; 

	queue_module.p_compcxx_Sink_14=&sink /*connect queue_module.out,sink.in*/;
	queue_module.p_compcxx_QueueModule2_12=&queue_back /*connect queue_module.out, queue_back.in*/; 
	queue_back.p_compcxx_Sink_13=&sink2 /*connect queue_back.out, sink2.in*/; 

	
	source.bandwidth = 411E3; 
	source.L = 5000; 

	
	onof.bandwidth = 80E3;
	onof.L = 5000;

	sink.id = 1; 
	sink2.id = 2; 

}
#line 67 "MM1K.cc"
void compcxx_MM1K_15:: Start()
{
	
}


#line 72 "MM1K.cc"
void compcxx_MM1K_15:: Stop()
{

}




#line 79 "MM1K.cc"
int main(int argc, char *argv[])
{

	compcxx_MM1K_15 az;;

 	long int seed = 2114;

	az.Seed=seed;
	az.StopTime(1000);

	az.Setup();

	az.Run();

	return 0;
};

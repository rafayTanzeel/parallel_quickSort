// Penn Parallel Primitives Library
// Author: Prof. Milo Martin
// University of Pennsylvania
// Spring 2010

#include "ppp.h"
#include "TaskGroup.h"

int taskSpawned=0;

namespace ppp {

  namespace internal {
    // Task-based scheduling variables
    TaskQueue* g_queues_ptr = NULL;
    atomic<int> g_stop_counter;  
  }

  using namespace internal;

  void TaskGroup::spawn(Task& t) {
    assert(g_queues_ptr != NULL);
    //std::cout<<taskSpawned%get_thread_count()<<std::endl;
    TaskQueue& queue = g_queues_ptr[taskSpawned%get_thread_count()];  // ASSIGNMENT: use per-thread task queue with "get_thread_id()"
    taskSpawned++;
    m_wait_counter.fetch_and_inc();
    t.setCounter(&m_wait_counter);
    queue.enqueue(&t);
  }
  
  void process_tasks(const atomic<int>* counter)
  {
    TaskQueue& queue = g_queues_ptr[get_thread_id()];  // ASSIGNMENT: use per-thread task queue with "get_thread_id()"

    while (counter->get() != 0) {
      PPP_DEBUG_EXPR(queue.size());

      if(queue.size()==0){
    	  int highestQIndex=0;
    	  int max=0;
    	  for(int i=0; i<get_thread_count(); i++){
    		  if(i!=get_thread_id() && (g_queues_ptr[i]).size()>max){
    			  max=(g_queues_ptr[i]).size();
    			  highestQIndex=i;
    		  }
    	  }
    	  if(highestQIndex!=get_thread_id()){
			  for(int i=0; i<((g_queues_ptr[highestQIndex]).size())/2; i++){
				  queue.enqueue((g_queues_ptr[highestQIndex]).steal());
			  }
    	  }
      }
       
      // Dequeue from local queue
      Task* task = queue.dequeue();

      // ASSIGNMENT: add task stealing

      if (task != NULL) {
        task->execute(); // overloaded method
        task->post_execute(); // cleanup, method of base class
      }
    }
  }
}


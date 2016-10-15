// Penn Parallel Primitives Library
// Author: Prof. Milo Martin
// University of Pennsylvania
// Spring 2010

#include "ppp.h"
#include "TaskGroup.h"
#include <vector>

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
    TaskQueue& queue = g_queues_ptr[get_thread_id()];  // ASSIGNMENT: use per-thread task queue with "get_thread_id()"
    m_wait_counter.fetch_and_inc();
    t.setCounter(&m_wait_counter);
    queue.enqueue(&t);
  }
  
  void process_tasks(const atomic<int>* counter)
  {
    TaskQueue& queue = g_queues_ptr[get_thread_id()];  // ASSIGNMENT: use per-thread task queue with "get_thread_id()"

    while (counter->get() != 0) {
      PPP_DEBUG_EXPR(queue.size());

      Task* task = NULL;

      if(queue.size()==0){
	  std::vector<int> availableQueues;
    	  for(int i=0; i<get_thread_count(); i++){
    		  if(i!=get_thread_id() && (g_queues_ptr[i]).size()>0){
			  availableQueues.push_back(i);
    		  }
    	  }
    	  if(availableQueues.size()>0){
		  task = (g_queues_ptr[availableQueues[rand() % availableQueues.size()]]).steal();
    	  }
      }
      else{
	task = queue.dequeue();
      }

      // ASSIGNMENT: add task stealing

      if (task != NULL) {
        task->execute(); // overloaded method
        task->post_execute(); // cleanup, method of base class
      }
    }
  }
}


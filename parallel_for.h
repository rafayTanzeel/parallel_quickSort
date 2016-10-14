// Penn Parallel Primitives Library
// Author: Prof. Milo Martin
// University of Pennsylvania
// Spring 2010

#ifndef PPP_FOR_H
#define PPP_FOR_H

#include "ppp.h"
#include "Task.h"
#include "TaskGroup.h"

namespace ppp {

	namespace internal {

		template <typename T>
	    class forTask: public ppp::Task {
	    public:
			forTask(int64_t start, int64_t end, T* functor, int64_t grainsize) { 
				m_start = start;
				m_end = end;
				m_functor = functor;
				m_grainsize = grainsize;
			}
	      
	      	void execute() {

	      		PPP_DEBUG_MSG("Execute: [" + to_string(m_start) + ", " + to_string(m_end) + "]");
		        assert(m_start < m_end);

		        if (m_end-m_start <= 1) {
		          return;
		        }
		        
		        if (m_end-m_start < m_grainsize) {
		          PPP_DEBUG_MSG("std::for: [" + to_string(m_start) + ", " + to_string(m_end) + "]");
		          PPP_DEBUG_MSG("std::for: [" + to_string(&m_functor[m_start]) + ", " + to_string(&m_functor[m_end]) + "]");
		          
		          m_functor->calculate(m_start, m_end);
		          return;
		        }
		        
		        int64_t pivot = (m_start + m_end) / 2;
		        assert(pivot < m_end);
		        
		        PPP_DEBUG_MSG("Split: [" + to_string(m_start) + ", " + to_string(pivot) + "] [" +
		                      to_string(pivot) + ", " + to_string(m_end) + "]");

		        ppp::TaskGroup tg;
		        forTask t1(m_start, pivot, m_functor, m_grainsize);
		        forTask t2(pivot, m_end, m_functor, m_grainsize);
		        tg.spawn(t1);
		        tg.spawn(t2);
		        tg.wait();
	      	}

		private:
			int64_t m_start;
			int64_t m_end;
			T* m_functor;
			int64_t m_grainsize;
	    };
	}
  
  	template <typename T>
  	extern inline
  	void parallel_for(int64_t start, int64_t end, T* functor, int64_t grainsize=0)
	{
	    // ASSIGNMENT: make this parallel via recursive divide and conquer
	    // functor->calculate(start, end);

	    if (grainsize == 0) {
	      grainsize = (end-start+1) / (get_thread_count()*4);
	    }
	    PPP_DEBUG_MSG("parallel_for grainsize: " + to_string(grainsize));
	            
	    internal::forTask<T> t(start, end, functor, grainsize);
	    t.execute();
	    PPP_DEBUG_MSG("parallel_for done");
  	}
}

#endif

// Penn Parallel Primitives Library
// Author: Prof. Milo Martin
// University of Pennsylvania
// Spring 2010

#ifndef PPP_REDUCE_H
#define PPP_REDUCE_H

#include "ppp.h"
#include "Task.h"
#include "TaskGroup.h"


namespace ppp {

	namespace internal {

    template <typename T>
    class reduceTask: public ppp::Task {
    public:

      reduceTask(T* array, int64_t left, int64_t right, int64_t grainsize, atomic<T>* sum) {
        m_array = array;
        m_left = left;
        m_right = right;
        m_grainsize = grainsize;
        m_sum = sum;

      }

//      T getSum(){
//    	  return sum.get();
//      }

      void execute() {
        PPP_DEBUG_MSG("Execute: [" + to_string(m_left) + ", " + to_string(m_right) + "]");
        assert(m_left < m_right);

        if (m_right-m_left <= 1) {
          return;
        }

        if (m_right-m_left < m_grainsize) {
          PPP_DEBUG_MSG("std::sort: [" + to_string(m_left) + ", " + to_string(m_right) + "]");
          PPP_DEBUG_MSG("std::sort: [" + to_string(&m_array[m_left]) + ", " + to_string(&m_array[m_right]) + "]");
//          std::sort(&m_array[m_left], &m_array[m_right]);
          T total;
          total = T(0);
          for (int i=m_left; i<m_right; i++) {
        	  total = total + m_array[i];
          }
          m_sum->fetch_and_add(total);
          return;
        }

        int64_t pivot = (m_left + m_right) / 2;
        assert(pivot < m_right);

        PPP_DEBUG_MSG("Split: [" + to_string(m_left) + ", " + to_string(pivot) + "] [" +
                      to_string(pivot) + ", " + to_string(m_right) + "]");
        ppp::TaskGroup tg;
        reduceTask t1(m_array, m_left, pivot, m_grainsize, m_sum);
        reduceTask t2(m_array, pivot, m_right, m_grainsize, m_sum);
       // std::cout<<1<<tg.(internal::m_wait_counter).get()<<std::endl;

        tg.spawn(t1);
        tg.spawn(t2);
        tg.wait();
      }
    private:
      T* m_array;
      int64_t m_left;
      int64_t m_right;
      int64_t m_grainsize;
      atomic<T>* m_sum;
    };

//    atomic<T> reduceTask::sum=T(0);

  }

  
  template <typename T>
  extern inline
  T parallel_reduce(T* array, int64_t start, int64_t end, int64_t grainsize=0)
  {
    // ASSIGNMENT: make this parallel via recursive divide and conquer

	atomic<T> sum;
	sum.set(T(0));

    if (grainsize == 0) {
    	grainsize = (end-start+1) / (get_thread_count()*4);
    }
	PPP_DEBUG_MSG("parallel_sort grainsize: " + to_string(grainsize));

	internal::reduceTask<T> t(array, start, end, grainsize, &sum);

	t.execute();
	PPP_DEBUG_MSG("parallel_sort done");
	return sum.get();
  }
}

#endif


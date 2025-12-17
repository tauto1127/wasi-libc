#include "pthread_impl.h"

int __pthread_mutex_lock(pthread_mutex_t *m)
{
	pthread_t self = __pthread_self();
	int own = m->_m_lock & 0x3fffffff;
	if ((m->_m_type&15) == PTHREAD_MUTEX_NORMAL
	    && !a_cas(&m->_m_lock, 0, EBUSY)){

		printf("[mtx_unlock] m=%p lock=0x%x own=%d self=%p tid=%d waiters=%d count=%d\n",
			   m, m->_m_lock, own, self, self->tid, m->_m_waiters, m->_m_count);
		// EBUSY状態にする
		return 0;
		}
	// return __pthread_mutex_timedlock(m, 0);
	// 
	// 

	self = __pthread_self();
    int own_before = m->_m_lock & 0x3fffffff;
    printf("[mtx_lock before] m=%p lock=0x%x own=%d self=%p tid=%d waiters=%d count=%d\n",
           m, m->_m_lock, own_before, self, self->tid, m->_m_waiters, m->_m_count);

    int ret = __pthread_mutex_timedlock(m, 0);

    int own_after = m->_m_lock & 0x3fffffff;
    printf("[mtx_lock after ] m=%p lock=0x%x own=%d self=%p tid=%d waiters=%d count=%d ret=%d\n",
           m, m->_m_lock, own_after, self, self->tid, m->_m_waiters, m->_m_count, ret);

    return ret;
}

weak_alias(__pthread_mutex_lock, pthread_mutex_lock);

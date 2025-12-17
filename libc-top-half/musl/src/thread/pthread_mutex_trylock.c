#include "pthread_impl.h"

int __pthread_mutex_trylock_owner(pthread_mutex_t *m)
{
	// ここまで来てないな
	int old, own;
	int type = m->_m_type;
	pthread_t self = __pthread_self();
	int tid = self->tid;

	old = m->_m_lock;
	own = old & 0x3fffffff;
	// printf("mutex=%p lock=%x self=%p tid=%d\n", m, m->_m_lock, self, self->tid);
	if (own == tid) {
		if ((type&8) && m->_m_count<0) {
			old &= 0x40000000;
			m->_m_count = 0;
			goto success;
		}
		if ((type&3) == PTHREAD_MUTEX_RECURSIVE) {
			if ((unsigned)m->_m_count >= INT_MAX) return EAGAIN;
			m->_m_count++;
			return 0;
		}
	}
#ifdef __wasilibc_unmodified_upstream
	if (own == 0x3fffffff) return ENOTRECOVERABLE;
#endif
	if (own || (old && !(type & 4))) {
		printf("[trylock_owner EBUSY] m=%p old=0x%x new=0x%x own=%d self=%p tid=%d type=%d\n",
			   m, old, tid, tid & 0x3fffffff, self, self->tid, type & 15);
		return EBUSY;
	}
	printf("tid=%d EBUSYじゃなかったぞ\n", tid);

	if (type & 128) {
		printf("wasi-libc type & 128 違うな通らない.\n");
		if (!self->robust_list.off) {
			self->robust_list.off = (char*)&m->_m_lock-(char *)&m->_m_next;
#ifdef __wasilibc_unmodified_upstream
			__syscall(SYS_set_robust_list, &self->robust_list, 3*sizeof(long));
#endif
		}
		if (m->_m_waiters) tid |= 0x80000000;
		self->robust_list.pending = &m->_m_next;
	}
	tid |= old & 0x40000000;

	if (a_cas(&m->_m_lock, old, tid) != old) {
		self->robust_list.pending = 0;
		if ((type&12)==12 && m->_m_waiters) return ENOTRECOVERABLE;
		return EBUSY;
	}
	
	    /* ←ここは「CASが成功した」場合だけ通る。好きなログを入れてOK */
    printf("[trylock_owner set, EBUSYじゃなくて_m_lockも書き変わってなかった] m=%p old=0x%x new=0x%x own=%d self=%p tid=%d type=%d\n",
           m, old, tid, tid & 0x3fffffff, self, self->tid, type & 15);

success:
	if ((type&8) && m->_m_waiters) {
		int priv = (type & 128) ^ 128;
#ifdef __wasilibc_unmodified_upstream
		__syscall(SYS_futex, &m->_m_lock, FUTEX_UNLOCK_PI|priv);
#endif
		self->robust_list.pending = 0;
		return (type&4) ? ENOTRECOVERABLE : EBUSY;
	}

	volatile void *next = self->robust_list.head;
	m->_m_next = next;
	m->_m_prev = &self->robust_list.head;
	if (next != &self->robust_list.head) *(volatile void *volatile *)
		((char *)next - sizeof(void *)) = &m->_m_next;
	self->robust_list.head = &m->_m_next;
	self->robust_list.pending = 0;

	if (old) {
		m->_m_count = 0;
		return EOWNERDEAD;
	}

	return 0;
}

int __pthread_mutex_trylock(pthread_mutex_t *m)
{
	// 一旦スキップする．詳細なデバッグのため
	// if ((m->_m_type&15) == PTHREAD_MUTEX_NORMAL) {
    //     int old = a_cas(&m->_m_lock, 0, EBUSY);
    //     if (!(old & EBUSY))
    //         printf("[trylock normal set] m=%p old=0x%x new=0x%x own=%d\n",
    //                m, old, EBUSY, EBUSY & 0x3fffffff);
    //     return old & EBUSY;
    // }
	// if ((m->_m_type&15) == PTHREAD_MUTEX_NORMAL)
	// 	return a_cas(&m->_m_lock, 0, EBUSY) & EBUSY;
	return __pthread_mutex_trylock_owner(m);
}

weak_alias(__pthread_mutex_trylock, pthread_mutex_trylock);

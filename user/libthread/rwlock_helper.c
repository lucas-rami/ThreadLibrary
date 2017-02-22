#include <rwlock.h>
#include <mutex.h>
#include <cond.h>
#include <assert.h>


int wait_for_read( rwlock_t *rwlock ) {

  if ( rwlock->active_writers > 0 || rwlock->waiting_writers > 0 ) {
    return 1;
  }

  return 0;
}

void start_read( rwlock_t *rwlock ) {

  mutex_lock( &rwlock->lock );

  rwlock->waiting_readers++;

  while( wait_for_read( rwlock ) ) {
    cond_wait( &rwlock->read_cvar, &rwlock->lock );
  }

  rwlock->waiting_readers--;

  rwlock->active_readers++;

  rwlock->curr_op = RWLOCK_READ;

  mutex_unlock( &rwlock->lock );
}

int wait_for_write( rwlock_t *rwlock ) {

  if ( rwlock->active_readers > 0 || rwlock->active_writers > 0 ) {
    return 1;
  }

  return 0;
}

void start_write( rwlock_t *rwlock ) {

  mutex_lock( &rwlock->lock );

  rwlock->waiting_writers++;

  while( wait_for_write( rwlock ) ) {
    cond_wait( &rwlock->write_cvar, &rwlock->lock );
  }

  rwlock->waiting_writers--;

  rwlock->active_writers++;

  rwlock->curr_op = RWLOCK_WRITE;

  mutex_unlock( &rwlock->lock );
}

void stop_read( rwlock_t *rwlock ) {

  mutex_lock( &rwlock->lock );

  rwlock->active_readers--;

  if ( rwlock->active_readers == 0 && rwlock->waiting_writers > 0 ) {
    cond_signal( &rwlock->write_cvar );
  }

  mutex_unlock( &rwlock->lock );
}

void stop_write( rwlock_t *rwlock ) {

  mutex_lock( &rwlock->lock );

  rwlock->active_writers--;

  assert( rwlock->active_writers == 0 );

  if ( rwlock->waiting_writers > 0 ) {
    cond_signal( &rwlock->write_cvar );
  } else {
    cond_broadcast( &rwlock->read_cvar );
  }

  mutex_unlock( &rwlock->lock );
}

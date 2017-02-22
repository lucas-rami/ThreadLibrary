#include <rwlock.h>
#include <mutex.h>
#include <cond.h>
#include <simics.h>
#include <assert.h>
#include <rwlock_helper.h>

#define RWLOCK_INITIALIZED 1
#define RWLOCK_UNINITIALIZED 0

int rwlock_init( rwlock_t *rwlock ) {

  if ( !rwlock ) {
    return -1;
  }

  if ( mutex_init( &rwlock->lock ) < 0 ) {
    return -1;
  }

  mutex_lock( &rwlock->lock );

  if ( cond_init( &rwlock->read_cvar ) < 0 || cond_init( &rwlock->write_cvar ) ) {
    return -1;
  }

  rwlock->init = RWLOCK_INITIALIZED;

  mutex_unlock( &rwlock->lock );
  return 0;
}

void rwlock_lock( rwlock_t *rwlock, int type ) {

  if ( !rwlock || ( type != RWLOCK_READ && type != RWLOCK_WRITE ) ) {
    return;
  }

  if ( rwlock->init != RWLOCK_INITIALIZED ) {
    // ERROR! lock on uninitialized rwlock
    lprintf( "ERROR! lock on uninitialized rwlock" );
  }

  if ( type == RWLOCK_READ ) {
    start_read( rwlock );
  } else {
    start_write( rwlock );
  }
}

void rwlock_unlock( rwlock_t *rwlock ) {

  if ( !rwlock ) {
    return;
  }

  if ( rwlock->init != RWLOCK_INITIALIZED ) {
    // ERROR! Unlock on uninitialized rwlock
    // TODO: panic?
    lprintf( "ERROR! unlock on uninitialized rwlock" );
  }

  mutex_lock( &rwlock->lock );

  if ( rwlock->active_readers == 0 && rwlock->active_writers == 0 ) {
    // ERROR! Unlock on not locked rwlock
    // TODO: panic? mutex release?
    lprintf( "ERROR! unlock on uninitialized rwlock" );
  }

  if ( rwlock->curr_op == RWLOCK_READ ) {
    stop_read( rwlock );
  } else {
    stop_write( rwlock );
  }
}

void rwlock_destroy( rwlock_t *rwlock ) {

  if ( !rwlock ) {
    return;
  }

  if ( rwlock->init != RWLOCK_INITIALIZED ) {
    // ERROR! Destroy on uninitialized rwlock
    // TODO: panic?
    lprintf( "ERROR! destroy on uninitialized rwlock" );
  }

  mutex_lock( &rwlock->lock );

  if ( rwlock->active_readers != 0 || rwlock->active_writers != 0 || 
       rwlock->waiting_readers != 0 || rwlock->waiting_writers != 0 ) {
    // ERROR! Destroy on busy rwlock
    // TODO: panic?
    lprintf( "ERROR! destroy on busy rwlock" );
  }

  cond_destroy( &rwlock->read_cvar );

  cond_destroy( &rwlock->write_cvar );
  
  rwlock->init = RWLOCK_UNINITIALIZED;

  mutex_unlock( &rwlock->lock );

  mutex_destroy( &rwlock->lock );
}

void rwlock_downgrade( rwlock_t *rwlock ) {

  if ( !rwlock ) {
    return;
  }

  if ( rwlock->active_writers <= 0 || rwlock->curr_op != RWLOCK_WRITE ) {
    return;
  }

  mutex_lock( &rwlock->lock );

  rwlock->active_writers--;

  assert( rwlock->active_writers == 0 );

  rwlock->active_readers++;

  rwlock->curr_op = RWLOCK_READ;

  cond_broadcast( &rwlock->read_cvar );

  mutex_unlock( &rwlock->lock );
 
}

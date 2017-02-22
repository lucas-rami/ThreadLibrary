#include <rwlock_type.h>

int wait_for_read( rwlock_t *rwlock );
void start_read( rwlock_t *rwlock );
int wait_for_write( rwlock_t *rwlock );
void start_write( rwlock_t *rwlock );
void stop_read( rwlock_t *rwlock );
void stop_write( rwlock_t *rwlock );

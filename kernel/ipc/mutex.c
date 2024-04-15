#include <stdint.h>
#include <lunaris/mutex.h>

void mutex_acquire(struct mutex * m)
{
  if(!m->locked)
    m->locked = 1;
}

void mutex_release(struct mutex * m)
{
  if(m->locked)
    m->locked = 0;
  
}

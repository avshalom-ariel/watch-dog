#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__

#include <stddef.h> /*size_t*/

int MMI(size_t interval_in_seconds, size_t repitition, char **argv);

void DNR(void);


#endif /*__WATCH_DOG_H__*/

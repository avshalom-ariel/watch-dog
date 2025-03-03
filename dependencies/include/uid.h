#ifndef __UID_H__
#define __UID_H__

#include <time.h>   /* time_t */
#include <stddef.h> /* size_t */
#include <sys/types.h> /* pid_t */

struct ilrd_uid {
    time_t timestamp;
    size_t counter;
    pid_t pid;
};

typedef struct ilrd_uid ilrd_uid_t;

/*
Description: Creates an instance of UID_t.
Params: No parameters.
Return Value: the relevent UID (UID_t).
Complexity: O(1) for both time and space
*/
ilrd_uid_t UIDCreate(void);

/*
Description: Compares two UIDs.
Params: two UID_t parameters.
Return Value: non-zero for equal, zero for not-equal.
Complexity: O(1) for both time and space.
*/
int UIDIsEqual(ilrd_uid_t uid1, ilrd_uid_t uid2);

/*
Description: Get invalid ilrd_uid_t.
Params: void.
Return Value: Invalid ilrd_uid_t.
Complexity: O(1) for both time and space.
*/
ilrd_uid_t UIDGetBad(void);

#endif /* __UID_H__ */

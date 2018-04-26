# freeOnExit
Created to simplify automatic memory release

## Get:
```Shell
git clone https://github.com/ox223252/freeOnExit.git
```

## Usage:
```C
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "freeOnExit.h"

void * functionLoop ( void * arg )
{
    while ( 1 )
    {
        printf ( "loop %s\n", ( char * )arg );
        sleep ( 1 );
    }
    pthread_exit ( NULL );
}

void * functionWait ( void * arg )
{
    printf ( "wait %s\n", ( char * )arg );
    sleep ( 3 );
    pthread_exit ( NULL );
}

void function ( void * str )
{
    printf ( "%s", ( char * )str );
}

int main ( void )
{
    static char * c = NULL;
    char * c2 = NULL;

    FILE * f;

    char * data;
    int shmid = 0;

    pthread_t threadId1;
    pthread_t threadId2;

    
    if ( initFreeOnExit ( ) )
    { // error case
        return ( 1 );
    }

    c = malloc ( 10 );
    c2 = malloc ( 10 );
    setOnExit ( sOET_ptr, 2, c, c2 );

    c = ( char * ) malloc ( 10 );
    setFreeOnExit ( c );

    c = ( char * ) malloc ( 10 );
    setOnExit ( sOET_ptr, 1, c );

    // file
    f = fopen ( "file", "rw" );
    setOnExit ( sOET_fd, 1, f );

    // shared memory
    if ( ( shmid = shmget ( 12345, 12, 0644 | IPC_CREAT ) ) == -1 )
    {
        return ( 2 );
    }

    data = shmat ( shmid, (void *)0, 0 );
    if ( !data )
    {
        return ( 3 );
    }
    setOnExit ( sOET_sm, 1, &data );

    // thread
    pthread_create ( &threadId1, NULL, functionWait, "join" );
    setThreadJoinOnExit ( threadId1 );

    pthread_create ( &threadId2, NULL, functionLoop, "kill" );
    setThreadKillOnExit ( threadId2 );

    // function before and after
    setExecAfterAllOnExit ( function, ( void * ) "All done\n" );
    setExecBeforeAllOnExit ( function, ( void * ) "before\n" );

    sleep ( 1 );

    return ( 0 );
}
```

## Execution test:
```C
$ gcc -Wall main.c freeOnExit.c -pthread 
$ valgrind --leak-check=full ./a.out
==54361== Memcheck, a memory error detector
==54361== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==54361== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==54361== Command: ./a.out
==54361==
wait join
loop kill
before
loop kill
loop kill
loop kill
All done
==54361==
==54361== HEAP SUMMARY:
==54361==     in use at exit: 0 bytes in 0 blocks
==54361==   total heap usage: 33 allocs, 33 frees, 4,038 bytes allocated
==54361==
==54361== All heap blocks were freed -- no leaks are possible
==54361==
==54361== For counts of detected and suppressed errors, rerun with: -v
==54361== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

```

## Execution sequence:
- 'function before'
- thread
- pointer
- files
- shared memory
- 'function after'
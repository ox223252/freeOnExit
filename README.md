# freeOnExit
Created to simplify automatic memory release

Designed for Linux

## How to use:
You can use freeOnExit lib with or without pthread and/or dll.
To use pthread/dll define `FOE_WITH_THREAD`/`FOE_WITH_DLL` or comment in the header `FOE_WITHOUT_THREAD`/`FOE_WITHOUT_DLL`.


## Get:
```Shell
git clone https://github.com/ox223252/freeOnExit.git
```

## Usage:
```C
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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
    int fd = 0;

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

    // files
    f = fopen ( "a.out", "rw" );
    setOnExit ( sOET_fd, 1, f );

    fd = open ( "a.out", O_RDWR );
    setCloseOnExit ( fd );

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
$ gcc -Wall main.c freeOnExit.c -pthread -D'FOE_WITH_THREAD'
$ valgrind --leak-check=full ./a.out
==70226== Memcheck, a memory error detector
==70226== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==70226== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==70226== Command: ./a.out
==70226==
wait join
loop kill
before
loop kill
loop kill
loop kill
All done
==70226==
==70226== HEAP SUMMARY:
==70226==     in use at exit: 0 bytes in 0 blocks
==70226==   total heap usage: 35 allocs, 35 frees, 4,046 bytes allocated
==70226==
==70226== All heap blocks were freed -- no leaks are possible
==70226==
==70226== For counts of detected and suppressed errors, rerun with: -v
==70226== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Execution sequence:
- 'function before'
- thread `#ifdef FOE_WITH_THREAD`
- pointer
- files (fopen/fclose)
- files (open/close)
- dll `#ifdef FOE_WITH_DLL`
- shared memory
- 'function after'

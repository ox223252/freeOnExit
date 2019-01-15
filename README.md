# freeOnExit
Created to simplify automatic memory release

## How to use:
You can use freeOnExit lib with or without pthread and/or dll.
To use pthread/dll define `FOE_WITH_THREAD`/`FOE_WITH_DLL`.

## Get:
```Shell
git clone https://github.com/ox223252/freeOnExit.git
```

## Exemple:
```C
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined ( __linux__ ) || defined ( __APPLE__ )
#include <sys/shm.h>
#else
#include <windows.h>
#endif

#include "freeOnExit/freeOnExit.h"

void * functionLoop ( void * arg )
{
    while ( 1 )
    {
        printf ( "loop %s\n", ( char * )arg );
        pthread_testcancel ( );
        sleep ( 1 );
    }
    return ( NULL );
}

void * functionWait ( void * arg )
{
    printf ( "wait %s\n", ( char * )arg );
    sleep ( 3 );
    return ( NULL );
}

void function ( void * str )
{
    printf ( "%s", ( char * )str );
}

#if defined ( _WIN32 ) || defined ( _WIN64 )
// to wait before closing term
void windowsPause ( void * arg )
{
    system ( "pause" );
}
#endif

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

    #if defined ( __linux__ ) ||defined ( __APPLE__ )
    // only available for linux
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
    #endif

    // thread
    pthread_create ( &threadId1, NULL, functionWait, "join" );
    setThreadJoinOnExit ( threadId1 );

    pthread_create ( &threadId2, NULL, functionLoop, "cancel" );
    setThreadCancelOnExit ( threadId2 );

    // function before and after
    setExecAfterAllOnExit ( function, ( void * ) "All done\n" );
    setExecBeforeAllOnExit ( function, ( void * ) "before all on exit\n" );

    #if defined ( __linux__ ) || defined ( __APPLE__ )
        sleep ( 1 );
    #elif defined ( _WIN32 ) || defined ( _WIN64 )
        setExecAfterAllOnExit ( windowsPause, NULL );
        Sleep ( 1000 );
    #endif

    printf ( "exit\n" );

    return ( 0 );
}
```

```Shell
$ gcc -Wall main.c freeOnExit/freeOnExit.c -D'FOE_WITH_THREAD' -pthread
$ i686-w64-mingw32-gcc main.c freeOnExit/freeOnExit.c -D'FOE_WITH_THREAD' -pthread
```
## functions:
```C
int setOnExit ( steOnExitType type, int nbEls, ... );
```
 - **type**: the type of pointer passed as arguments
   - *sOET_ptr*: std malloc pointer
   - *sOET_fd*: std file desciptor open with fopen
   - *sOET_sm*: shared memory pointer get with shmat or getSharedMem (my shared memory interface)
 - **nbEls**: number of pointer passed as arguments
 - **...**: pointer

```C
int setFreeOnExit ( void * ptr );
```

 - **ptr**: pointer allocated with malloc.

```C
void unsetFreeOnExit ( void * ptr );
```

 - **ptr**: pointer allocated with malloc, previously used in setFreeOnExit() function. 

```C
int setFCloseOnExit ( void * fd );
```

 - **ptr**: pointer to file descriptor opened with fopen.

```C
void unsetFCloseOnExit ( void * fd );
```

 - **ptr**: pointer to file descriptor opened with fopen, previously used in setFCloseOnExit() function. 

```C
int setCloseOnExit ( int fd );
```

 - **ptr**: pointer to file descriptor opened with open.

```C
void unsetCloseOnExit ( int fd );
```

 - **ptr**: pointer to file descriptor opened with open, previously used in setCloseOnExit() function. 

```C
int setThreadCancelOnExit ( pthread_t arg );
```

- **ptr**: pointer to pthred_t created by pthread_create.

```C
void unsetThreadCancelOnExit ( pthread_t arg );
```

- **ptr**: pointer to pthred_t created by pthread_create, previously used in setThreadCancelOnExit() function. 

```C
int setThreadJoinOnExit ( pthread_t pth );
```

- **ptr**: pointer to pthred_t created by pthread_create.

```C
void unsetThreadJoinOnExit ( pthread_t pth );
```

- **ptr**: pointer to pthred_t created by pthread_create, previously used in unsetThreadJoinOnExit() function. 



```C
int setExecAfterAllOnExit ( void ( *fn )( void * ), void * param );
```

- **ptr**: pointer to function what will be called before freeing/closing on exit or return event.

```C
void unsetExecAfterAllOnExit ( void ( *fn )( void * ) );
```

- **ptr**: pointer to function, previously used in setExecAfterAllOnExit() function. 

```C
int setExecBeforeAllOnExit ( void ( *fn )( void * ), void * param );
```

- **ptr**: pointer to function what will be called after freeing/closing on exit or return event.

```C
void unsetExecBeforeAllOnExit ( void ( *fn )( void * ) );
```

- **ptr**: pointer to function, previously used in setExecBeforeAllOnExit() function. 

### Only linux
```C
int setDlCloseOnExit ( void * dl );
```

 - **ptr**: pointer to dynamic library opened with dlopen.
 
```C
void unsetDlCloseOnExit ( void * dl );
```

 - **ptr**: pointer to dynamic library opened with dlopen, previously used in setDlCloseOnExit() function. 
 
```C
int setDetachOnExit ( void * sh );
```

 - **ptr**: pointer to shared memory opened with shmat or getSharedMem (my shared memory interface).

```C
void unsetDetachOnExit ( void * sh );
```

 - **ptr**: pointer to shared memory opened with shmat or getSharedMem (my shared memory interface), previously used in setDetachOnExit() function. 


## Execution test:
```C
$ gcc -Wall main.c freeOnExit/freeOnExit.c -D'FOE_WITH_THREAD' -pthread
$ valgrind --leak-check=full ./a.out
==17790== Memcheck, a memory error detector
==17790== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==17790== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==17790== Command: ./a.out
==17790==
loop cancel
wait join
exit
before all on exit
loop cancel
All done
==17790==
==17790== HEAP SUMMARY:
==17790==     in use at exit: 0 bytes in 0 blocks
==17790==   total heap usage: 37 allocs, 37 frees, 4,102 bytes allocated
==17790==
==17790== All heap blocks were freed -- no leaks are possible
==17790==
==17790== For counts of detected and suppressed errors, rerun with: -v
==17790== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Execution sequence:
- 'function before'
- thread `#ifdef FOE_WITH_THREAD`
  - thread cancel
  - thread join
- pointer
- files (fopen/fclose)
- files (open/close)
- dll `#ifdef FOE_WITH_DLL`
- shared memory only for linux/mac
- 'function after'

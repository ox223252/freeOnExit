# freeOnExit
Created to simplify automatic memory release

## Get:
```Shell
git clone https://github.com/ox223252/freeOnExit.git
```

## Usage:
```C
#include <stdlib.h>

#include "lib/freeOnExit/freeOnExit.h"

void main ( void )
{
    char * c = NULL;
    
    if ( initFreeOnExit ( ) )
    { // error case
        return;
    }

    c = ( char * ) malloc ( 10 );
    setFreeOnExit ( c );
    c = ( char * ) malloc ( 10 );
    setFreeOnExit ( c );
    c = ( char * ) malloc ( 10 );
    setFreeOnExit ( c );

    // file
    // f = fopen ( "file", "rw" );
    // setCloseOnExit ( f );

    // shared memory
    // getShareMem ( ( void ** ) &data, 12, 12345 );
    // setDetachOnExit ( ( void * )data );

    // thread
    // pthread_create ( &threadId, NULL, function, &data );
    // setThreadJoinOnExit ( &threadId );
    // setThreadKillOnExit ( &threadId )

    // function before and after
    // setExecAfterAllOnExit ( function, ( void * ) &args );
    // setExecBeforeAllOnExit ( function, ( void * ) &args );
}
```

## Execution test:
```C
$ gcc main.c freeOnExit.c -pthread 
$ valgrind ./a.out
==13547== Memcheck, a memory error detector
==13547== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==13547== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==13547== Command: ./a.out
==13547==
==13547==
==13547== HEAP SUMMARY:
==13547==     in use at exit: 0 bytes in 0 blocks
==13547==   total heap usage: 15 allocs, 15 frees, 150 bytes allocated
==13547==
==13547== All heap blocks were freed -- no leaks are possible
==13547==
==13547== For counts of detected and suppressed errors, rerun with: -v
==13547== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Execution sequence:
- 'function before'
- thread
- pointer
- files
- shared memory
- 'function after'

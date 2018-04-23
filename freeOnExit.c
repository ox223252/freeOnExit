////////////////////////////////////////////////////////////////////////////////
/// \copiright ox223252, 2017
///
/// This program is free software: you can redistribute it and/or modify it
///     under the terms of the GNU General Public License published by the Free
///     Software Foundation, either version 2 of the License, or (at your
///     option) any later version.
///
/// This program is distributed in the hope that it will be useful, but WITHOUT
///     ANY WARRANTY; without even the implied of MERCHANTABILITY or FITNESS FOR
///     A PARTICULAR PURPOSE. See the GNU General Public License for more
///     details.
///
/// You should have received a copy of the GNU General Public License along with
///     this program. If not, see <http://www.gnu.org/licenses/>
////////////////////////////////////////////////////////////////////////////////

#include <sys/socket.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/shm.h>
#include <signal.h>
#include <pthread.h>

static void onExit ( void );

static void **ptr;        // double pointeur qui sauvegarde les differents pointeur à liberer
static uint64_t size = 0; // nombre de pointeur à liberer

static int **fd;          // pointeur qui sauvegarde les differents file descripteur à fermer
static uint64_t fdSize = 0; // nombre de fd à fermer

static void **sh;          // pointeur qui sauvegarde les differents file descripteur à fermer
static uint64_t shSize = 0; // nombre de fd à fermer

static pthread_t **thJ;    // pointeur qui sauvegarde les differents pointeur sur threads
static uint64_t thJSize = 0; // nombre de thread à kill

static pthread_t **thK;    // pointeur qui sauvegarde les differents pointeur sur threads
static uint64_t thKSize = 0; // nombre de thread à kill

typedef void ( *fnPtr )( void * );

static fnPtr * fnA;       // function executed after execut every free on exit
static void ** fnAArg;
static uint64_t fnASize = 0;

static fnPtr * fnB;       // function executed before execut every free on exit
static void ** fnBArg;
static uint64_t fnBSize = 0;


int initFreeOnExit ( void )
{
	ptr = ( void ** ) malloc ( sizeof ( void * ) );
	fd = ( int ** ) malloc ( sizeof ( int * ) );
	sh = ( void ** ) malloc ( sizeof ( void * ) );
	thJ = ( pthread_t ** ) malloc ( sizeof ( pthread_t * ) );
	thK = ( pthread_t ** ) malloc ( sizeof ( pthread_t * ) );
	fnA = ( fnPtr * ) malloc ( sizeof ( fnPtr ) );
	fnAArg = ( void ** ) malloc ( sizeof ( void * ) );
	fnB = ( fnPtr * ) malloc ( sizeof ( fnPtr ) );
	fnBArg = ( void ** ) malloc ( sizeof ( void * ) );

	return ( atexit ( onExit ) );
}

int setFreeOnExit ( void * arg )
{
	void **tmp;           // pointeur temporaire

	if ( !ptr )
	{
		return ( -2 );
	}

	tmp = ( void ** ) realloc ( ptr, ( size + 1 ) * sizeof ( void * ) );
	if ( !tmp )           // verifie que le pointeur à bien été réaloué
	{
		return ( -1 );
	}

	ptr = tmp;
	ptr [ size ] = arg;   // on sauvegarde le parametre
	size++;

	return ( 0 );
}

int setCloseOnExit ( int * arg )
{
    int **tmp;            // pointeur temporaire

    if ( !fd )
    {
        return ( -2 );
    }

    tmp = ( int ** ) realloc ( fd, ( fdSize + 1 ) * sizeof ( int * ) );
    if ( !tmp )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }

    fd = tmp;
    fd [ fdSize ] = arg;   // on sauvegarde le parametre
    fdSize++;

    return ( 0 );
}

int setDetachOnExit ( void * arg )
{
    void **tmp;            // pointeur temporaire

    if ( !sh )
    {
        return ( -2 );
    }

    tmp = ( void ** ) realloc ( sh, ( shSize + 1 ) * sizeof ( void * ) );
    if ( !tmp )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }

    sh = tmp;
    sh [ shSize ] = arg;   // on sauvegarde le parametre
    shSize++;

    return ( 0 );
}

int setThreadJoinOnExit ( void * arg )
{
    pthread_t **tmp;            // pointeur temporaire

    if ( !thJ )
    {
        return ( -2 );
    }

    tmp = ( pthread_t ** ) realloc ( thJ, ( thJSize + 1 ) * sizeof ( pthread_t * ) );
    if ( !tmp )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }

    thJ = tmp;
    thJ [ thJSize ] = arg;   // on sauvegarde le parametre
    thJSize++;

    return ( 0 );
}

int setThreadKillOnExit ( void * arg )
{
    pthread_t **tmp;            // pointeur temporaire

    if ( !thK )
    {
        return ( -2 );
    }

    tmp = ( pthread_t ** ) realloc ( thK, ( thKSize + 1 ) * sizeof ( pthread_t * ) );
    if ( !tmp )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }

    thK = tmp;
    thK [ thKSize ] = arg;   // on sauvegarde le parametre
    thKSize++;

    return ( 0 );
}

int setExecAfterAllOnExit ( void * arg, void * param )
{
    fnPtr *tmp;            // pointeur temporaire
    void **tmp2;            // pointeur temporaire

    if ( !fnA )
    {
        return ( -2 );
    }

    tmp = ( fnPtr * ) realloc ( fnA, ( fnASize + 1 ) * sizeof ( fnPtr ) );
    if ( !tmp )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }
    fnA = tmp;

    tmp2 = ( void  ** ) realloc ( fnAArg, ( fnASize + 1 ) * sizeof ( void  * ) );
    if ( !tmp2 )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }
    fnAArg = tmp2;

    fnA [ fnASize ] = arg;   // on sauvegarde le parametre
    fnAArg [ fnASize ] = param;   // on sauvegarde le parametre
    fnASize++;

    return ( 0 );
}

int setExecBeforeAllOnExit ( void * arg, void * param )
{
    fnPtr *tmp;            // pointeur temporaire
    void **tmp2;            // pointeur temporaire

    if ( !fnB )
    {
        return ( -2 );
    }

    tmp = ( fnPtr * ) realloc ( fnB, ( fnBSize + 1 ) * sizeof ( fnPtr ) );
    if ( !tmp )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }
    fnB = tmp;

    tmp2 = ( void ** ) realloc ( fnBArg, ( fnBSize + 1 ) * sizeof ( void * ) );
    if ( !tmp )           // verifie que le pointeur à bien été réaloué
    {
        return ( -1 );
    }
    fnBArg = tmp2;

    fnB [ fnBSize ] = arg;   // on sauvegarde le parametre
    fnBArg [ fnBSize ] = param;   // on sauvegarde le parametre
    fnBSize++;

    return ( 0 );
}

// fonction apellé à la fin du programme sauf dans un cas d'appel à kill -9
static void onExit ( void )
{
	uint64_t i;           // loop counter
	
	// function before
	for ( i = 0; i < fnBSize; i++ )
	{
		if ( fnB[i] != 0 )     // évite le cas où le thread à deja été libéré
		{
			fnB[i] ( fnBArg[ i ] );
		}
	}

	if ( fnB != NULL )    // evit le cas où on à apellé deux fois init ( et donc atexit)
	{
		free ( fnB );
		free ( fnBArg );
		fnB = NULL;
		fnBArg = NULL;
		fnBSize = 0;
	}

	// threads Join
	for ( i = 0; i < thJSize; i++ )
	{
		if ( thJ[i] != 0 )     // évite le cas où le thread à deja été libéré
		{
			pthread_join ( *thJ[i], NULL );
			thJ[i] = 0;
		}
	}

	if ( thJ != NULL )    // evit le cas où on à apellé deux fois init ( et donc atexit)
	{
		free ( thJ );
		thJ = NULL;
		thJSize = 0;
	}

	// threads Exit
	for ( i = 0; i < thKSize; i++ )
	{
		if ( thK[i] != 0 )     // évite le cas où le thread à deja été libéré
		{
			pthread_kill ( *thK[i], 9 );
			thK[i] = 0;
		}
	}

	if ( thK != NULL )    // evit le cas où on à apellé deux fois init ( et donc atexit)
	{
		free ( thK );
		thK = NULL;
		thKSize = 0;
	}

	// pointer
	for ( i = 0; i < size; i++ )
	{
		if ( ptr[i] != NULL )     // évite le cas où le pointeur à deja été libéré
		{
			free ( ptr[i] );
			ptr[i] = NULL;
		}
	}

	if ( ptr != NULL )    // evit le cas où on à apellé deux fois init ( et donc atexit)
	{
		free ( ptr );
		ptr = NULL;
		size = 0;
	}

	// files
	for ( i = 0; i < fdSize; i++ )
	{
		if ( fd[i] != 0 )     // évite le cas où le pointeur à deja été libéré
		{
			close ( *fd[i] );
			fd[i] = 0;
		}
	}

	if ( fd != NULL )    // evit le cas où on à apellé deux fois init ( et donc atexit)
	{
		free ( fd );
		fd = NULL;
		fdSize = 0;
	}

	// shared memory
	for ( i = 0; i < shSize; i++ )
	{
		if ( sh[i] != 0 )     // évite le cas où le pointeur à deja été libéré
		{
			shmdt ( sh[i] );
			sh[i] = 0;
		}
	}

	if ( sh != NULL )    // evit le cas où on à apellé deux fois init ( et donc atexit)
	{
		free ( sh );
		sh = NULL;
		shSize = 0;
	}

	// function after
	for ( i = 0; i < fnASize; i++ )
	{
		if ( fnA[i] != 0 )     // évite le cas où le thread à deja été libéré
		{
			fnA[i] ( fnAArg[ i ] );
		}
	}

	if ( fnA != NULL )    // evit le cas où on à apellé deux fois init ( et donc atexit)
	{
		free ( fnA );
		free ( fnAArg );
		fnA = NULL;
		fnAArg = NULL;
		fnASize = 0;
	}
}

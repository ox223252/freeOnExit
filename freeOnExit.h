#ifndef __FREEONEXIT_H__
#define __FREEONEXIT_H__
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

#include <pthread.h>

////////////////////////////////////////////////////////////////////////////////
/// \file freeOnExit.h
/// \brief library store and free / close pointer an file descriptor at the end
///   of execution
/// \author ox223252
/// \date 2017-07
/// \copyright GPLv2
/// \version 0.3
/// \warning NONE
/// \bug NONE
////////////////////////////////////////////////////////////////////////////////

typedef enum
{
	sOET_ptr, // pointer
	sOET_fd, // file descriptor (fopen/fclose)
	sOET_sm, // shared memory
}
steOnExitType;

////////////////////////////////////////////////////////////////////////////////
/// \fn int initFreeOnExit ( void );
///
/// \brief init storage pointer for freeing pointer on exit
///
/// \return 0 : OK
///         X : Error
////////////////////////////////////////////////////////////////////////////////
int initFreeOnExit ( void );


////////////////////////////////////////////////////////////////////////////////
/// \fn int setOnExit ( steOnExitType type, int nbEls, ... )
///
/// \param [in] type : type of pointer
/// \param [in] nbEls : nb of pointer
/// \param [in] ... : arg pointers what will be free on exit
///
/// \brief save new pointer(s) need to be free on exit
///
/// \return 0 : OK
///        -1 : new pointer not saved to be free later
///        -2 : initFreeOnExit not made or failed
///        -3 : type unknow
////////////////////////////////////////////////////////////////////////////////
int setOnExit ( steOnExitType type, int nbEls, ... );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setFreeOnExit ( void * ptr );
///
/// \param [in] ptr : arg pointer what will be free on exit
///
/// \brief save new pointer need to be free on exit
///
/// \return 0 : OK
///        -1 : new pointer not saved to be free later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setFreeOnExit ( void * ptr );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setFCloseOnExit ( int * fd );
///
/// \param [in] fd : arg file descriptor what will be closed on exit
///
/// \brief save new file descriptor need to be closed on exit
///
/// \return 0 : OK
///        -1 : new fd not saved to be free later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setFCloseOnExit ( void * fd );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setCloseOnExit ( int * fd );
///
/// \param [in] fd : arg file descriptor what will be closed on exit
///
/// \brief save new file descriptor need to be closed on exit
///
/// \return 0 : OK
///        -1 : new fd not saved to be free later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setCloseOnExit ( int fd );


////////////////////////////////////////////////////////////////////////////////
/// \fn int setDlCloseOnExit ( int * fd );
///
/// \param [in] fd : arg dll descriptor what will be closed on exit
///
/// \brief save new dll descriptor need to be closed on exit
///
/// \return 0 : OK
///        -1 : new fd not saved to be free later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setDlCloseOnExit ( void * dl );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setDetachOnExit ( int * sh );
///
/// \param [in] sh : arg shared memory pointer
///
/// \brief save new shared memory descriptor need to be closed on exit
///
/// \return 0 : OK
///        -1 : new shared mem not saved to be detached later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setDetachOnExit ( void * sh );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setThreadJoinOnExit ( pthread_h * pth );
///
/// \param [in] pth : pthread id of thread
///
/// \brief save new thread id need to be join on exit
///
/// \return 0 : OK
///        -1 : new pthread id not saved to be joined later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setThreadJoinOnExit ( pthread_t pth );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setThreadKillOnExit ( pthread_h * pth );
///
/// \param [in] pth : pthread id of thread
///
/// \brief save new thread id need to be exited on exit
///
/// \return 0 : OK
///        -1 : new pthread id not saved to be joined later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setThreadKillOnExit ( pthread_t pth );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setExecAfterAllOnExit ( void ( * )( void * ), void * param );
///
/// \param [in] fn : pointer on function
/// \param [in] param : function parameter
///
/// \brief add function to the queue of function called after freeing memory
///
/// \return 0 : OK
///        -1 : new fn not saved to be executed later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setExecAfterAllOnExit ( void ( *fn )( void * ), void * param );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setExecBeforeAllOnExit ( void ( * )( void * ), void * param );
///
/// \param [in] fn : pointer on function
/// \param [in] param : function parameter
///
/// \brief add function to the queue of function called before freeing memory
///
/// \return 0 : OK
///        -1 : new fn not saved to be executed later
///        -2 : initFreeOnExit not made or failed
////////////////////////////////////////////////////////////////////////////////
int setExecBeforeAllOnExit ( void ( *fn )( void * ), void * param );

////////////////////////////////////////////////////////////////////////////////
/// \note Execution sequence :
///     - 'function before'
///     - thread
///     - pointer
///     - files
///     - shared memory
///     - 'function after'
////////////////////////////////////////////////////////////////////////////////

#endif

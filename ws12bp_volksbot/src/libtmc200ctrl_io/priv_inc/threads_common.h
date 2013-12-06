#ifndef _THREADS_COMMON_H_
#define _THREADS_COMMON_H_

/**
 * @file threads_common.h
 * @ingroup Privatelibtmc200ctrl_io 
 * @brief Contains the common functionality of the threads, internally used by libtmc200ctrl_io.
 */

#ifndef TRUE
#define TRUE (1)	///<Define if a condition is true.
#endif

#ifndef FALSE
#define FALSE (0)	///<Define if a condition is false.
#endif

#define NUM_THREADS	(2)	///<Number of threads.

/**
 * @brief Enumerates the used threads.
 */
typedef enum {
	SENDTMC200CMDS,	///<SendTMC200Cmds-Thread
	TMC200FEEDBACK	///<TMC200Feedback-Thread
} LIBTMC200CTRL_COMMON_THREADS;

/**
 * @brief Initializes a thread.
 * 
 * @param thread Thread to initialize.
 * @param func thread function.
 * @param arg thread function arguments.
 * @param name Name of the queue (DEBUG mode only).
 * 
 * @return 0 if OK, <0 in case of an error.
 */
#ifdef DEBUG
int initThread(	LIBTMC200CTRL_COMMON_THREADS thread, const char* name,
				void *(*func) (void *), void *arg);

/**
 * @brief Returns the name of the thread.
 * 
 * @param thread The thread.
 * 
 *
 * @remark DEBUG mode only.
 *
 * @return The name of the Thread.
 */
const char* getThreadName(LIBTMC200CTRL_COMMON_THREADS thread);
#else
int initThread(	LIBTMC200CTRL_COMMON_THREADS thread,
				void *(*func) (void *), void *arg);
#endif


/**
 * @brief Pause the thread.
 * 
 * @param thread Thread to pause.
 */
void pauseThread(LIBTMC200CTRL_COMMON_THREADS thread);

/**
 * @brief Resumes the thread.
 * 
 * @param thread Thread to resume.
 */
void resumeThread(LIBTMC200CTRL_COMMON_THREADS thread);

/**
 * @brief Checks if the thread is paused.
 * 
 * @param thread The thread to check.
 */
void checkThreadPaused(LIBTMC200CTRL_COMMON_THREADS thread);

/**
 * @brief Returns if the thread is alive.
 * 
 * @param thread The thread to check.
 *
 * @return 1 if thread is alive, else 0.
 */
int isThreadAlive(LIBTMC200CTRL_COMMON_THREADS thread);

/**
 * @brief Returns if the thread is paused.
 * 
 * @param thread The thread to check.
 *
 * @return 1 if thread is paused, else 0.
 */
int isThreadPaused(LIBTMC200CTRL_COMMON_THREADS thread);

/**
 * @brief Shuts down a thread.
 * 
 * @param thread The thread to shut down.
 *
 */
void shutdownThread(LIBTMC200CTRL_COMMON_THREADS thread);

#endif


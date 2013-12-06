#include <threads_common.h>
#include <pthread.h>

#ifdef DEBUG
#include <string.h>
#include <stdio.h>

static char				threadName[NUM_THREADS][32];
#endif

//Array of thread handles.
static pthread_t		threadObj[NUM_THREADS];
//Whether a thread is alive.
static int				isAlive[NUM_THREADS];
//Whether a thread is paused.
static int				isPaused[NUM_THREADS];
//Mutexes to avoid concurrent access.
static pthread_mutex_t	tThreadMutex[NUM_THREADS] = {PTHREAD_MUTEX_INITIALIZER};

//Mutex and condition to suspend and resume a thread.
static pthread_mutex_t	tWakeUpThreadMutex[NUM_THREADS] = {PTHREAD_MUTEX_INITIALIZER};
static pthread_cond_t 	tWakeUpThreadCond[NUM_THREADS] = {PTHREAD_COND_INITIALIZER};


#ifdef DEBUG
const char* getThreadName(LIBTMC200CTRL_COMMON_THREADS thread) {
	return threadName[thread];
}

int initThread(	LIBTMC200CTRL_COMMON_THREADS thread, const char* name,
				void *(*func) (void *), void *arg)
#else
int initThread(	LIBTMC200CTRL_COMMON_THREADS thread,
				void *(*func) (void *), void *arg)
#endif
{
	#ifdef DEBUG
	strncpy(threadName[thread],name,strlen(name));
	#endif
	
	isPaused[thread]=FALSE;
	isAlive[thread]=TRUE;

	return pthread_create(&threadObj[thread],NULL,func,arg);
}

void pauseThread(LIBTMC200CTRL_COMMON_THREADS thread) {
	pthread_mutex_lock(&tThreadMutex[thread]);
    pthread_mutex_lock(&tWakeUpThreadMutex[thread]);
	isPaused[thread]=TRUE;
    pthread_mutex_unlock(&tWakeUpThreadMutex[thread]);
    pthread_mutex_unlock(&tThreadMutex[thread]);
	#ifdef DEBUG
	printf("[LIBTMC200CTRL %s thread] Paused\n",threadName[thread]);
	#endif    
}

void resumeThread(LIBTMC200CTRL_COMMON_THREADS thread) {
    pthread_mutex_lock(&tThreadMutex[thread]);
    pthread_mutex_lock(&tWakeUpThreadMutex[thread]);
    isPaused[thread]=FALSE;
    pthread_cond_broadcast(&tWakeUpThreadCond[thread]);
    pthread_mutex_unlock(&tWakeUpThreadMutex[thread]);
    pthread_mutex_unlock(&tThreadMutex[thread]);
	#ifdef DEBUG
	printf("[LIBTMC200CTRL %s thread] Resumed\n",threadName[thread]);
	#endif 
}

void checkThreadPaused(LIBTMC200CTRL_COMMON_THREADS thread) {
    pthread_mutex_lock(&tWakeUpThreadMutex[thread]);
    while (isPaused[thread]==TRUE)
    	pthread_cond_wait(&tWakeUpThreadCond[thread], &tWakeUpThreadMutex[thread]);
    pthread_mutex_unlock(&tWakeUpThreadMutex[thread]);
}

int isThreadAlive(LIBTMC200CTRL_COMMON_THREADS thread) {
	int ret;
	pthread_mutex_lock(&tThreadMutex[thread]);
    ret = isAlive[thread];
	pthread_mutex_unlock(&tThreadMutex[thread]);
    return ret;
}

int isThreadPaused(LIBTMC200CTRL_COMMON_THREADS thread) {
	int ret;
	pthread_mutex_lock(&tThreadMutex[thread]);
    ret = isPaused[thread];
    pthread_mutex_unlock(&tThreadMutex[thread]);
    return ret;
}

void shutdownThread(LIBTMC200CTRL_COMMON_THREADS thread) {
	if(isThreadAlive(thread)==FALSE)
		return;

	isAlive[thread]=FALSE;

	if(pthread_self()!=threadObj[thread]) {
		
		if(isThreadPaused(thread)==TRUE)
			resumeThread(thread);

		pthread_join(threadObj[thread],NULL);
		
		#ifdef DEBUG
		printf("[LIBTMC200CTRL %s thread] Completed by main thread\n", threadName[thread]);
		#endif
	}
	#ifdef DEBUG	 
	else		
		printf("[LIBTMC200CTRL %s thread] Cmpleted itself.\n",threadName[thread]);
	#endif
}


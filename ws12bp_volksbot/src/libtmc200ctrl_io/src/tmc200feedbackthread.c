#include <threads_common.h>
#include <queues.h>
#include <string.h>
#include <sys/time.h>

#include <stdlib.h>
#include <pthread.h>
#ifdef DEBUG
#include <stdio.h>
#endif

//Pointer to the function that is executed to process incoming data.
void (*processFeedbackFunc)(TMC200Commands,const char*)=NULL;

/*
 * Whether this thread should synconized wait until
 * the the controller answer was picked up.
 */
static int				waitForTMC200FBCalled=FALSE;

/*
 * Mutex and condition for conditional waiting.
 */
static pthread_mutex_t	tTMC200FBAvailableMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t 	tTMC200FBAvailableCond = PTHREAD_COND_INITIALIZER;

/*
 * Pointer to the recent controller answer.
 */
static char*			currentTMC200FBStr = NULL;

//50 millisecounds  timeout.
#define MILLSEC_WAITFORTMC200FB_TIMEOUT (50000)

char* tmc200ctrl_io_waitForTMC200Feedback() {
	char*	pcRetStr = NULL;
	struct 	timespec tWait;
	struct 	timeval tNow;
	int		ret;

	
	waitForTMC200FBCalled=TRUE;

	//Get the current time.
	gettimeofday(&tNow,NULL);
	tWait.tv_sec = tNow.tv_sec;
	
	/*
	 * Add the time to wait.
	 * Bitshift by 10 bits is mutch faster then an multiply by 1000.
	 */
	tWait.tv_nsec = (tNow.tv_usec+MILLSEC_WAITFORTMC200FB_TIMEOUT)<<10;	

    pthread_mutex_lock(&tTMC200FBAvailableMutex);
    ret = pthread_cond_timedwait(&tTMC200FBAvailableCond,
    							 &tTMC200FBAvailableMutex,&tWait);				 
    pthread_mutex_unlock(&tTMC200FBAvailableMutex);
	
	//Suspend the feedback processing thread.
	pauseThread(TMC200FEEDBACK);

	/*
	 * Only copy the data if 'pthread_cond_timedwait'
	 * returns without an error.
	 */
	if(ret==0) {
		const int len = strlen(currentTMC200FBStr);

		pcRetStr = (char*) malloc(len+1);
	
		strcpy(pcRetStr,currentTMC200FBStr);
	}

	/*
	 * Resume the feedback processing thread.
	 */
	waitForTMC200FBCalled=FALSE;
	resumeThread(TMC200FEEDBACK);
	
	return pcRetStr;
}


void tmc200ctrl_io_setTMC200FeedbackProcessingFunction(void (*func)(TMC200Commands,const char*)) {
	processFeedbackFunc=func;
}

void* TMC200FeedBackThreadFunc(void* data) {
	TMC200CmdQueueItem*	ptItem = NULL;
	#ifdef DEBUG
	printf("[LIBTMC200CTRL] Thread '%s' started.\n",getThreadName(TMC200FEEDBACK));
	#endif

	while(1) {
		//Get the next item.
		ptItem = popItemFromQueue(FEEDBACK);

		if(ptItem!=NULL) {
			//Call the process data function if it was set.
			if(processFeedbackFunc!=NULL) {
				if(waitForTMC200FBCalled==TRUE) {
					/*
					 * Wait until the current feedback
					 * string has been copyed.
					 */
				
					currentTMC200FBStr = ptItem->ptData;
					
					pthread_mutex_lock(&tTMC200FBAvailableMutex);
					pthread_cond_broadcast(&tTMC200FBAvailableCond);
					pthread_mutex_unlock(&tTMC200FBAvailableMutex);				
				
					checkThreadPaused(TMC200FEEDBACK);
				}
		
				/*
				 * Only call if a function is given.
				 */
				if(processFeedbackFunc!=NULL)
					processFeedbackFunc(ptItem->eCmd,ptItem->ptData);
			}
				
			//Recycle the item.
			recycleItemAtQueue(FEEDBACK,ptItem);
		} else { 
			if(queueSize(FEEDBACK)==0) {
				//If the queue is empty.
				if(isThreadAlive(TMC200FEEDBACK)==FALSE)
					//Exit if the thread was canceled.
					break;			

				//Otherwise pause.
				pauseThread(TMC200FEEDBACK);
				checkThreadPaused(TMC200FEEDBACK);
			}						
		}
	}

	#ifdef DEBUG
	printf("[LIBTMC200CTRL] Thread '%s' finished.\n",getThreadName(TMC200FEEDBACK));
	#endif
	
	pthread_exit(0);
}


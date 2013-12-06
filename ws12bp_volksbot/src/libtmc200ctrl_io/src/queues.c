#include <queues.h>
#include <string.h>
#include <pthread.h>

#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#ifdef DEBUG
//Printable queue name for debugging.
static char					queueName[NUM_QUEUES][32];
#endif
//Queue that holds items which have to be processed.
static TMC200CmdQueueItem*	ptQueue[NUM_QUEUES] = {NULL};

//Backup queue that holds items that has been allocated for later use.
static TMC200CmdQueueItem*	ptFreeItems[NUM_QUEUES] = {NULL};

//Amount of Payload that can be stored in an item in bytes.
static int 					iDataSize[NUM_QUEUES] = {0};

//Holds the size of a queue.
static int 					iQueueSize[NUM_QUEUES] = {0};

//Mutex to provide threadsave access.
static pthread_mutex_t		tQueueMutex[NUM_QUEUES] = {PTHREAD_MUTEX_INITIALIZER};

static void deleteItems(TMC200CmdQueueItem* ptHead) {
	/*
	 * Delete all items in a queue by taking the top most,
	 * deleting it and get the next one until all elements are deleted.
	 */

	TMC200CmdQueueItem*	ptTmp=NULL;

	while(ptHead!=NULL) {
		free(ptHead->ptData);
		ptTmp=ptHead;
		ptHead = ptHead->ptNext;
		free(ptTmp);
		ptTmp=NULL;
	}
}

#ifdef DEBUG
void initQueue(LIBTMC200CTRL_COMMON_QUEUES queue,const char* name,int datasize)
#else
void initQueue(LIBTMC200CTRL_COMMON_QUEUES queue,int datasize)
#endif
{
	pthread_mutex_lock(&tQueueMutex[queue]);
	#ifdef DEBUG
	//Copy over the queue name.
	strncpy(queueName[queue],name,strlen(name));
	#endif

	ptQueue[queue] = NULL;
	ptFreeItems[queue] = NULL;
	
	//Set the amount of data an item of this queue can store.
	iDataSize[queue] = datasize;

	pthread_mutex_unlock(&tQueueMutex[queue]);
}

void addItemToQueue(LIBTMC200CTRL_COMMON_QUEUES queue,TMC200CmdQueueItem* item) {
	pthread_mutex_lock(&tQueueMutex[queue]);
	if(ptQueue[queue]==NULL) {	
		/*
		 * If the queue is empty, the item is added as head.
		 */
		ptQueue[queue]=item;

		#ifdef DEBUG
		printf("[LIBTMC200CTRL '%s' tmc200cmdqueue_add] Add to empty list.\n",queueName[queue]);
		#endif
	} else {
		/*
		 * Else it gets append.
		 */
	
		TMC200CmdQueueItem*	ptTmp=ptQueue[queue];

		while(ptTmp->ptNext!=NULL)
			ptTmp=ptTmp->ptNext;
			
		ptTmp->ptNext=item;

		#ifdef DEBUG
		printf("[LIBTMC200CTRL '%s' tmc200cmdqueue_add] Add to none empty list.\n",queueName[queue]);
		#endif
	}
	
	iQueueSize[queue]++;
	
	pthread_mutex_unlock(&tQueueMutex[queue]);
}

TMC200CmdQueueItem* getFreshItemFromQueue(LIBTMC200CTRL_COMMON_QUEUES queue) {
	TMC200CmdQueueItem*	ptRet=NULL;

	pthread_mutex_lock(&tQueueMutex[queue]);
	if(ptFreeItems[queue]==NULL) {
		/*
		 * There are no item left for reuse. Allocate a new one.
		 */
	
		ptRet=(TMC200CmdQueueItem*)malloc(sizeof(TMC200CmdQueueItem));
		ptRet->ptData = (char*)malloc(iDataSize[queue]+1);

		#ifdef DEBUG
		printf("[LIBTMC200CTRL '%s' TMC200CmdQueue_getFreshItem] Allocate new Item.\n",queueName[queue]);
		#endif
	} else {
		/*
		 * Reuse a saved item.
		 */
	
		ptRet = ptFreeItems[queue];
		ptFreeItems[queue]=ptFreeItems[queue]->ptNext;
		#ifdef DEBUG
		printf("[LIBTMC200CTRL '%s' TMC200CmdQueue_getFreshItem] Reuese free Item.\n",queueName[queue]);
		#endif
	}
	
	pthread_mutex_unlock(&tQueueMutex[queue]);

	ptRet->ptNext=NULL;	
	
	return ptRet;
}

TMC200CmdQueueItem* popItemFromQueue(LIBTMC200CTRL_COMMON_QUEUES queue) {
	TMC200CmdQueueItem* ret=NULL;

	/*
	 * Take an Item from the queue.
	 */

	pthread_mutex_lock(&tQueueMutex[queue]);
	if(ptQueue[queue]!=NULL) {
		ret = ptQueue[queue];
		ptQueue[queue]=ptQueue[queue]->ptNext;
		
		iQueueSize[queue]--;
	}
	pthread_mutex_unlock(&tQueueMutex[queue]);

	#ifdef DEBUG
	printf("[LIBTMC200CTRL '%s' tmc200cmdqueue_pop] Called.\n",queueName[queue]);
	#endif

	return ret;
}

void recycleItemAtQueue(LIBTMC200CTRL_COMMON_QUEUES queue,TMC200CmdQueueItem* item) {
	/* 
	 * Same as 'addItemToQueue'
	 */

	item->ptNext=NULL;
	pthread_mutex_lock(&tQueueMutex[queue]);
	if(ptFreeItems[queue]==NULL) {
		ptFreeItems[queue]=item;	
		#ifdef DEBUG
		printf("[LIBTMC200CTRL '%s' tmc200cmdqueue_recycleItem] Add to empty list.\n",queueName[queue]);
		#endif
	} else {
		/* 
		 * .. but prepend the item.
		 */
		item->ptNext = ptFreeItems[queue];
		ptFreeItems[queue]=item;
		
		#ifdef DEBUG
		printf("[LIBTMC200CTRL '%s' tmc200cmdqueue_recycleItem] Add to none empty list.\n",queueName[queue]);
		#endif
	}
	pthread_mutex_unlock(&tQueueMutex[queue]);
}

int queueSize(LIBTMC200CTRL_COMMON_QUEUES queue) {
	int ret=0;
		
	pthread_mutex_lock(&tQueueMutex[queue]);
	ret = iQueueSize[queue];
	pthread_mutex_unlock(&tQueueMutex[queue]);

	#ifdef DEBUG
	printf("[LIBTMC200CTRL '%s' tmc200cmdqueue_size] Called.\n",queueName[queue]);
	#endif

	return ret;
}

void cleanUpQueue(LIBTMC200CTRL_COMMON_QUEUES queue) {
	pthread_mutex_lock(&tQueueMutex[queue]);
	deleteItems(ptQueue[queue]);
	deleteItems(ptFreeItems[queue]);
	pthread_mutex_unlock(&tQueueMutex[queue]);
	#ifdef DEBUG
	printf("[LIBTMC200CTRL '%s' tmc200cmdqueue_cleanUp] Called.\n",queueName[queue]);
	#endif
}

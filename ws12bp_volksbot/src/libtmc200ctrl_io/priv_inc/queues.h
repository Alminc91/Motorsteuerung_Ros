#ifndef _QUEUES_H_
#define _QUEUES_H_

/**
 * @defgroup Privatelibtmc200ctrl_io libtmc200ctrl_io (Internal API)
 * @file queues.h
 * @ingroup Privatelibtmc200ctrl_io 
 * @brief Contains the queues, internally used by libtmc200ctrl_io.
 */

#include <tmc200_commands.h>

#define NUM_QUEUES	(2)					///<Number of queues

#define CMDQUEUE_DATA_SIZE (31)			///<Data size of the queue used by SendTMC200Cmds-Thread.
#define FEEDBACKQUEUE_DATA_SIZE (127)	///<Data size of the queue used by TMC200Feedback-Thread.

/**
 * @brief An queue item.
 */
typedef struct Item {
	char*			ptData;	///<Data hold by the item.
	TMC200Commands 	eCmd;	///<TMC200-Command where the data belongs to.
	struct Item*	ptNext;	///<Pointer to the next Element.
} TMC200CmdQueueItem;

/**
 * @brief Enumerates the used queues.
 */
typedef enum {
	SENDCMDS,	///<SendTMC200Cmds-Thread queue.
	FEEDBACK	///<TMC200Feedback-Thread queue.
} LIBTMC200CTRL_COMMON_QUEUES;

/**
 * @brief Initializes a queue.
 * 
 * @param queue Queue to initialize.
 * @param datasize Number of bytes that can be stored in a queue item.
 * @param name Name of the queue (DEBUG mode only).
 */
#ifdef DEBUG
void initQueue(LIBTMC200CTRL_COMMON_QUEUES queue,const char* name,int datasize);
#else
void initQueue(LIBTMC200CTRL_COMMON_QUEUES queue,int datasize);
#endif

/**
 * @brief Add a item to a queue.
 * 
 * @param queue Queue where the item should be added.
 * @param item The item.
 */
void addItemToQueue(LIBTMC200CTRL_COMMON_QUEUES queue,TMC200CmdQueueItem* item);

/**
 * @brief 	Reuse a (recycled) item.
 * 			Advantage: No new memory has to be allocated.
 
 * @param queue Queue where the item should be added.
 *
 * @return A queue item.
 */
TMC200CmdQueueItem* getFreshItemFromQueue(LIBTMC200CTRL_COMMON_QUEUES queue);

/**
 * @brief Pops an item off a queue.
 * 
 * @param queue Queue where the item should be added.
 *
 * @returns Pointer to an item or a NULL-Pointer, if queue is empty.
 */
TMC200CmdQueueItem* popItemFromQueue(LIBTMC200CTRL_COMMON_QUEUES queue);

/**
 * @brief Recycles an item.
 * 
 * @param queue Queue where the item should be recycled.
 * @param item The item.
 */
void recycleItemAtQueue(LIBTMC200CTRL_COMMON_QUEUES queue,TMC200CmdQueueItem* item);

/**
 * @brief The number of items in the queue.
 * 
 * @param queue The Queue.
 *
 * @return Number of items in the queue.
 */
int queueSize(LIBTMC200CTRL_COMMON_QUEUES queue);

/**
 * @brief Frees all resources used by the queue.
 * 
 * @param queue The queue.
 */
void cleanUpQueue(LIBTMC200CTRL_COMMON_QUEUES queue);

#endif

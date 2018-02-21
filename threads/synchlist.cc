// synchlist.cc
//	Routines for synchronized access to a list.
//
//	Implemented by surrounding the List abstraction
//	with synchronization routines.
//
// 	Implemented in "monitor"-style -- surround each procedure with a
// 	lock acquire and release pair, using condition signal and wait for
// 	synchronization.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchlist.h"
#include "system.h"

//----------------------------------------------------------------------
// SynchList::SynchList
//	Allocate and initialize the data structures needed for a 
//	synchronized list, empty to start with.
//	Elements can now be added to the list.
//----------------------------------------------------------------------

SynchList::SynchList()
{
    list = new List();
    lock = new Lock("list lock"); 
    listEmpty = new Condition("list empty cond");
}

//----------------------------------------------------------------------
// SynchList::~SynchList
//	De-allocate the data structures created for synchronizing a list. 
//----------------------------------------------------------------------

SynchList::~SynchList()
{ 
    delete list; 
    delete lock;
    delete listEmpty;
}

//----------------------------------------------------------------------
// SynchList::Append
//      Append an "item" to the end of the list.  Wake up anyone
//	waiting for an element to be appended.
//
//	"item" is the thing to put on the list, it can be a pointer to 
//		anything.
//----------------------------------------------------------------------

void
SynchList::Append(void *item)
{
    lock->Acquire();		// enforce mutual exclusive access to the list 
    list->Append(item);
    listEmpty->Signal(lock);	// wake up a waiter, if any
    lock->Release();
}

//----------------------------------------------------------------------
// SynchList::Remove
//      Remove an "item" from the beginning of the list.  Wait if
//	the list is empty.
// Returns:
//	The removed item. 
//----------------------------------------------------------------------

void *
SynchList::Remove()
{
    void *item;

    lock->Acquire();			// enforce mutual exclusion
    while (list->IsEmpty())
	listEmpty->Wait(lock);		// wait until list isn't empty
    item = list->Remove();
    ASSERT(item != NULL);
    lock->Release();
    return item;
}

//----------------------------------------------------------------------
// SynchList::Mapcar
//      Apply function to every item on the list.  Obey mutual exclusion
//	constraints.
//
//	"func" is the procedure to be applied.
//----------------------------------------------------------------------

void
SynchList::Mapcar(VoidFunctionPtr func)
{ 
    lock->Acquire(); 
    list->Mapcar(func);
    lock->Release(); 
}


/***** LAB 3 BEGIN *****/
Barrior::Barrior(int arg_totalSynch):totalSynch(arg_totalSynch)
{
    alreadySynch = 0;
    lock = new Lock("barrior_lock");
    condition = new Condition("barrior_condition");
}

Barrior::~Barrior()
{
    delete lock;
    delete condition;
}

void Barrior::Synch()
{
    lock->Acquire();

    if (alreadySynch + 1 < totalSynch)
    {
        //printf("Thread %d, Synched!\n", currentThread->getTID());
        alreadySynch += 1;
        condition->Wait(lock);
    }
    else
    {
        alreadySynch = 0;
        condition->Broadcast(lock);
    }

    lock->Release();
}
/*****  LAB 3 END  *****/

/***** LAB 3 BEGIN *****/
RW_Lock::RW_Lock(int arg_cap): capacity(arg_cap)
{
    change_comp = 0;
    used = 0;
    heldBy = NULL;
    content = new int[arg_cap];
    lock = new Lock("rw_lock");
    condition = new Condition("rw_condition");
}

RW_Lock::~RW_Lock()
{
    delete[] content;
    delete lock;
    delete condition;
}

void RW_Lock::lock_acquire()
{
    lock->Acquire();

    while (used > 0)
    {
        printf("Thread %d waits for the RW Lock.\n", currentThread->getTID());
        condition->Wait(lock);
    }
    printf("Thread %d holds the RW Lock.\n", currentThread->getTID());
    used += 1;
    heldBy = currentThread;

    lock->Release();
}

void RW_Lock::lock_release()
{
    lock->Acquire();

    used -= 1;
    heldBy = NULL;
    printf("Thread %d releases the RW Lock.\n", currentThread->getTID());
    condition->Signal(lock);

    lock->Release();
}

bool RW_Lock::isHeldByCurrentThread()
{
    return heldBy == currentThread;
}

int RW_Lock::locked_write(int val, int idx)
{
    if (isHeldByCurrentThread())
    {
        content[idx] = val;
        return 1;
    }
    return -1;
}

int RW_Lock::read(int idx)
{
    int old_change_comp = -1, res = -1;
    do
    {
        old_change_comp = change_comp;
        res = content[idx];
    } while(old_change_comp != change_comp);
    return res;
}
/*****  LAB 3 END  *****/
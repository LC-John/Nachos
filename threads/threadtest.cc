// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "interrupt.h"
#include <stdio.h>
#include "synch.h"
#include "synchlist.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        /***** LAB 1 BEGIN *****/
        currentThread->Print();
        /*****  LAB 1 END  *****/
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

/***** LAB 1 BEGIN *****/
void
ThreadTest2()
{
    DEBUG('t', "Entering ThreadTest2");

    for (int i = 0; i <= MaxThread; i++)
    {
        printf("Attempting to creat thread%d\n", i);
        char* ch = new char[100];
        sprintf(ch, "thread%d", i);
        Thread *t = new Thread(ch);
        t->Print();
    }
}
/*****  LAB 1 END  *****/

/***** LAB 1 BEGIN *****/
void rawThread(int n)
{
    currentThread->Print();
    printf("Thread Status:\n");
    ts();
    printf("\n");
    currentThread->Yield();
}

void
ThreadTest3()
{
    DEBUG('t', "Entering ThreadTest3");

    Thread *t[2];

    printf("Test checkpoint 1\n");
    printf("Thread Status:\n");
    ts();
    printf("\n");
    for (int i = 0; i < 2; i++)
    {
        t[i] = new Thread("forked thread");
    }
    printf("Test checkpoint 2\n");
    printf("Thread Status:\n");
    ts();
    printf("\n");
    for (int i = 0; i < 2; i++)
    {
        t[i] -> Fork(rawThread, 1000);
    }
    currentThread->Yield();
    printf("Test checkpoint 3\n");
    printf("Thread Status:\n");
    ts();
    printf("\n");
}
/*****  LAB 1 END  *****/

/***** LAB 1 BEGIN *****/
void
ThreadTest0()
{
    DEBUG('t', "Entering ThreadTest0");

    char command[100];
    printf("$ >> ");
    while(scanf("%s", &command))
    {
        if (command[0] == 't' && command[1] == 's')
            ts();
        else if (command[0] == 'n' && command[1] == 't')
            Thread *tmp_t = new Thread("forked thread");
        else if (command[0] == 'e' && command[1] == 'x'
                && command[2] == 'i' && command[3] == 't')
                currentThread->Finish();
        else
            printf("Cannot find such command!\n");
        printf("$ >> ");
    }
}
/*****  LAB 1 END  *****/

/***** LAB 2 BEGIN *****/
void
PriorityThread(int minPri)
{
    int num, nextPri;
    Thread *t;
    nextPri = currentThread->getPriority() - 1;
    
    for (num = 0; num < 5; num++) {
        printf("Thread %d Loop %d\n", currentThread->getTID(), num);
        //currentThread->Print();
        if (num == 0 && nextPri > minPri)
        {
            t = new Thread("Forked Thread", nextPri);
            printf("New thread:\t");
            t->Print();
            t->Fork(PriorityThread, minPri);
        }
    }
}

void
ThreadTest4()
{
    DEBUG('t', "Entering ThreadTest0");

    Thread *t;
    t = new Thread("Forked Thread", 10);
    printf("New thread:\t");
    t->Print();
    t->Fork(PriorityThread, 4);
}
/*****  LAB 2 END  *****/

/***** LAB 2 BEGIN *****/
static int timeSlices = 0;

void
PriorityThread2(int minPri)
{
    int num, nextPri;
    Thread *t;
    nextPri = currentThread->getPriority() - 1;
    
    printf("Thread %d BEGIN!\n", currentThread->getTID());
    for (num = 0; num < 100; num++) {

        if ((num+1) % 20 == 0)
        printf("Thread %d Loop %d, priority %d, time-slice %d/%d\n",
            currentThread->getTID(), num,
            currentThread->getPriority(),
            currentThread->getUsedTimeSlices(),
            currentThread->getTimeSlices());
        IntStatus oldLevel = interrupt->SetLevel(IntOff);
        (void) interrupt->SetLevel(IntOn);
        (void) interrupt->SetLevel(oldLevel);
        //currentThread->Print();
        if (num == 0 && nextPri > minPri)
        {
            t = new Thread("Forked Thread", nextPri, timeSlices);
            printf("New thread %d\n", t->getTID());
            //t->Print();
            t->Fork(PriorityThread2, minPri);
        }
    }
    printf("Thread %d END!\n", currentThread->getTID());
}

void
ThreadTest5()
{
    DEBUG('t', "Entering ThreadTest0");

    Thread *t;
    printf("Time Slices = ");
    scanf("%d", &timeSlices);

    t = new Thread("Forked Thread", 3, timeSlices);
    printf("New thread %d\n", t->getTID());
    //t->Print();
    t->Fork(PriorityThread2, 0);
}
/*****  LAB 2 END  *****/

/***** LAB 2 BEGIN *****/
void PriorityThread3(int dummy)
{
    for (int i = 0; i < 10000; i++)
    {
        if ((i) % 100 == 0)
        {
            printf("thread %d loop %d, p-level %d priority %d, time-slice %d/%d\n",
                currentThread->getTID(), i, scheduler->PriorityLevel(currentThread),
                currentThread->getPriority(), currentThread->getUsedTimeSlices(),
                currentThread->getTimeSlices());
        }
        IntStatus oldLevel = interrupt->SetLevel(IntOff);
        (void) interrupt->SetLevel(IntOn);
        (void) interrupt->SetLevel(oldLevel);
    }
}

void
ThreadTest6()
{
    DEBUG('t', "Entering ThreadTest0");

    Thread *t1, *t2, *t3;
    t1 = new Thread("Forked Thread", 10);
    t2 = new Thread("Forked Thread", 0);
    t3 = new Thread("Forked Thread", 5);
    t1->Fork(PriorityThread3, 0);
    t2->Fork(PriorityThread3, 0);
    t3->Fork(PriorityThread3, 0);
}
/*****  LAB 2 END  *****/

/***** LAB 2 BEGIN *****/
void DeleteThreadListPrint(int arg_item)
{
    Thread *t = (Thread *)arg_item;
    t->Print();
}

void DeleteThread(int iter)
{
    printf("%s, TID = %d, BEGIN!\n",
        currentThread->getName(), currentThread->getTID());
    printf("\tCurrent 2B-Destroyed Queue BEGIN\n");
    threadToBeDestroyed->Mapcar(DeleteThreadListPrint);
    printf("\tCurrent 2B-Destroyed Queue END\n");
    printf("\tCurrent TS BEGIN\n");
    ts();
    printf("\tCurrent TS END\n");
    if (iter > 0)
    {
        Thread *t;
        t = new Thread ("Forked Thread", 0);
        printf("New thread: %s, TID = %d\n",
            t->getName(), t->getTID());
        t -> Fork(DeleteThread, iter-1);
    }
    printf("%s, TID = %d, END!\n",
        currentThread->getName(), currentThread->getTID());
    currentThread->Finish();
}

void
ThreadTest7()
{
    Thread *t;
    t = new Thread ("Forked Thread", 0);
    printf("New thread: %s, TID = %d\n",
            t->getName(), t->getTID());
    t -> Fork(DeleteThread, 5);
}
/*****  LAB 2 END  *****/

/***** LAB 3 BEGIN *****/
Semaphore *semaphore_mutex;
Semaphore *semaphore_full;
Semaphore *semaphore_empty;
int box_cap = 2;
char name[10][50] = {"banana", "peach", "coconut", "melon", "bone"};
int production_produced_by[100], production_name_idx[100];

void production_append(int arg_name_idx, int arg_by)
{
    for (int i = 0; i < box_cap; i++)
        if (production_produced_by[i] < 0 && production_name_idx[i] < 0)
        {
            production_name_idx[i] = arg_name_idx;
            production_produced_by[i] = arg_by;
            return;
        }
    ASSERT(true);
}
int production_get_name()
{
    for (int i = 0; i < box_cap; i++)
        if (production_produced_by[i] >= 0 && production_name_idx[i] >= 0)
            return production_name_idx[i];
    ASSERT(true);
}
int production_get_producer()
{
    for (int i = 0; i < box_cap; i++)
        if (production_produced_by[i] >= 0 && production_name_idx[i] >= 0)
            return production_produced_by[i];
    ASSERT(true);
}
void production_remove()
{
    for (int i = 0; i < box_cap; i++)
        if (production_produced_by[i] >= 0 && production_name_idx[i] >= 0)
        {
            production_name_idx[i] = -1;
            production_produced_by[i] = -1;
            return;
        }
    ASSERT(true);
}
void production_print()
{
    for (int i = 0; i < box_cap; i++)
        if (production_produced_by[i] >= 0 && production_name_idx[i] >= 0)
            printf("\tA %s produced by producer %d\n", name[production_name_idx[i]], production_produced_by[i]);
}

void semaphore_producer(int n)
{
    int arg_name_idx = currentThread->getTID()%5;
    for(int i = 0; i < n; i++)
    {
        if (Random()%2==0)
            currentThread->Yield();
        semaphore_empty->P();
        semaphore_mutex->P();

        production_append(arg_name_idx, currentThread->getTID());
        printf("Producer %d produced a %s, and would produce %d more.\n",
            currentThread->getTID(), name[arg_name_idx], n - i - 1);
        printf("What is in the box:\n");
        production_print();
        printf("\n");

        semaphore_mutex->V();
        semaphore_full->V();
    }
}
void semaphore_consumer(int n)
{
    for (int i = 0; i < n; i++)
    {
        if (Random()%2==0)
            currentThread->Yield();
        semaphore_full->P();
        semaphore_mutex->P();

        printf("Consumer %d consumed a %s produced by %d, and would consume %d more.\n",
            currentThread->getTID(), name[production_get_name()], production_get_producer(), n - i - 1);
        production_remove();
        printf("What is in the box:\n");
        production_print();
        printf("\n");

        semaphore_mutex->V();
        semaphore_empty->V();
    }
}

void
ThreadTest8()
{
    Thread *p[128], *c[128];
    int np = 2, nc = 3;
    int p_cap[128] = {6, 6}, c_cap[128] = {1, 4, 7};
    bool default_setting = true;
    char ch;
    printf("Default settig? [Y/N]\n");
    while(scanf("%c", &ch))
    {
        if (ch == 'Y')
        {
            default_setting = true;
            break;
        }
        else if (ch == 'N')
        {
            default_setting = false;
            break;
        }
        else
            printf("Invalid input\nDefault setting? [Y/N]");
    }
    if (ch == EOF)
        return;
    if (!default_setting)
    {
        printf("Box capacity = ");
        scanf("%d", &box_cap);
        printf("Box capacity = %d\n", box_cap);
        int p_sum = 0, c_sum = 0;
        p_sum = 0;
        c_sum = 0;
        PRODUCING_CONSUMING_INPUT_LABEL:
        printf("Number of producers = ");
        scanf("%d", &np);
        printf("Number of producers = %d\n", np);
        for (int i = 0; i < np; i++)
        {
            printf("Producer No.%d produces:", (i+1));
            scanf("%d", &p_cap[i]);
            printf("Producer No.%d produces: %d\n", (i+1), p_cap[i]);
            p_sum += p_cap[i];
        }
        printf("Number of consumers = ");
        scanf("%d", &nc);
        printf("Number of consumers = %d\n", nc);
        for (int i = 0; i < nc; i++)
        {
            printf("Consumer No.%d consumes:", (i+1));
            scanf("%d", &c_cap[i]);
            printf("Consumer No.%d consumes: %d\n", (i+1), c_cap[i]);
            c_sum += c_cap[i];
        }
        if (p_sum != c_sum)
        {
            printf("Invalid input\nThe sum of producing must equals the sum of consuming!\n");
            printf("Re-input...\n");
            goto PRODUCING_CONSUMING_INPUT_LABEL;
        }
    }
    semaphore_mutex = new Semaphore("mutex", 1);
    semaphore_full = new Semaphore("full", 0);
    semaphore_empty = new Semaphore("empty", box_cap);
    for (int i = 0; i < np; i++)
        p[i] = new Thread("Producer", 0);
    for (int i = 0; i < nc; i++)
        c[i] = new Thread("Consumer", 0);
    for (int i = 0; i < box_cap; i++)
    {
        production_produced_by[i] = -1;
        production_name_idx[i] = -1;
    }

    for (int i = 0; i < np; i++)
        p[i]->Fork(semaphore_producer, p_cap[i]);
    for (int i = 0; i < nc; i++)
        c[i]->Fork(semaphore_consumer, c_cap[i]);
}
/*****  LAB 3 END  *****/

/***** LAB 3 BEGIN *****/
Lock *cl_lock;
Condition *cl_pro_wait, *cl_con_wait;

int production_occupied()
{
    int cnt = 0;
    for (int i = 0; i < box_cap; i++)
        if (production_produced_by[i] >= 0 && production_name_idx[i] >= 0)
            cnt++;
    return cnt;
}

void condition_lock_producer(int n)
{
    int arg_name_idx = currentThread->getTID()%5;
    for(int i = 0; i < n; i++)
    {
        if (Random()%2==0)
            currentThread->Yield();
        cl_lock->Acquire();

        int po;

        while ((po = production_occupied()) >= box_cap)
        {
            printf("Producer %d waiting ...\n\n", currentThread->getTID());
            cl_pro_wait->Wait(cl_lock);
        }

        production_append(arg_name_idx, currentThread->getTID());
        printf("Producer %d produced a %s, and would produce %d more.\n",
            currentThread->getTID(), name[arg_name_idx], n - i - 1);
        printf("What is in the box:\n");
        production_print();
        printf("\n");

        //if (production_isEmptyAddOne())
        cl_con_wait->Broadcast(cl_lock);

        cl_lock->Release();
    }
}

void condition_lock_consumer(int n)
{
    for (int i = 0; i < n; i++)
    {
        if (Random()%2==0)
            currentThread->Yield();
        cl_lock->Acquire();

        int po;
        while ((po = production_occupied()) <= 0)
        {
            printf("consumer %d waiting ...\n\n", currentThread->getTID());
            cl_con_wait->Wait(cl_lock);
        }

        printf("Consumer %d consumed a %s produced by %d, and would consume %d more.\n",
            currentThread->getTID(), name[production_get_name()], production_get_producer(), n - i - 1);
        production_remove();
        printf("What is in the box:\n");
        production_print();
        printf("\n");

        //if (production_isFullMinusOne())
        cl_pro_wait->Broadcast(cl_lock);

        cl_lock->Release();
    }
}

void
ThreadTest9()
{
    Thread *p[128], *c[128];
    int np = 2, nc = 3;
    int p_cap[128] = {6, 6}, c_cap[128] = {1, 4, 7};
    bool default_setting = true;
    char ch;
    printf("Default settig? [Y/N]\n");
    while(scanf("%c", &ch))
    {
        if (ch == 'Y')
        {
            default_setting = true;
            break;
        }
        else if (ch == 'N')
        {
            default_setting = false;
            break;
        }
        else
            printf("Invalid input\nDefault setting? [Y/N]");
    }
    if (ch == EOF)
        return;
    if (!default_setting)
    {
        printf("Box capacity = ");
        scanf("%d", &box_cap);
        printf("Box capacity = %d\n", box_cap);
        int p_sum = 0, c_sum = 0;
        p_sum = 0;
        c_sum = 0;
        PRODUCING_CONSUMING_INPUT_LABEL_2:
        printf("Number of producers = ");
        scanf("%d", &np);
        printf("Number of producers = %d\n", np);
        for (int i = 0; i < np; i++)
        {
            printf("Producer No.%d produces:", (i+1));
            scanf("%d", &p_cap[i]);
            printf("Producer No.%d produces: %d\n", (i+1), p_cap[i]);
            p_sum += p_cap[i];
        }
        printf("Number of consumers = ");
        scanf("%d", &nc);
        printf("Number of consumers = %d\n", nc);
        for (int i = 0; i < nc; i++)
        {
            printf("Consumer No.%d consumes:", (i+1));
            scanf("%d", &c_cap[i]);
            printf("Consumer No.%d consumes: %d\n", (i+1), c_cap[i]);
            c_sum += c_cap[i];
        }
        if (p_sum != c_sum)
        {
            printf("Invalid input\nThe sum of producing must equals the sum of consuming!\n");
            printf("Re-input...\n");
            goto PRODUCING_CONSUMING_INPUT_LABEL_2;
        }
    }
    cl_lock = new Lock("lock");
    cl_pro_wait = new Condition("producer_condition");
    cl_con_wait = new Condition("consumer_condition");
    for (int i = 0; i < np; i++)
        p[i] = new Thread("Producer", 0);
    for (int i = 0; i < nc; i++)
        c[i] = new Thread("Consumer", 0);
    for (int i = 0; i < box_cap; i++)
    {
        production_produced_by[i] = -1;
        production_name_idx[i] = -1;
    }

    for (int i = 0; i < np; i++)
        p[i]->Fork(condition_lock_producer, p_cap[i]);
    for (int i = 0; i < nc; i++)
        c[i]->Fork(condition_lock_consumer, c_cap[i]);
}
/*****  LAB 3 END  *****/

/***** LAB 3 BEGIN *****/
Barrior *synchBarrior;
int totalSynch = 0;

void synchThread(int n)
{
    for (int i = 0; i < n; i++)
    {
        if (Random()%2==0)
            currentThread->Yield();
        synchBarrior->Synch();
        printf("thread %d, loop %d\n", currentThread->getTID(), i);
        synchBarrior->Synch();
    }
}

void unsynchThread(int n)
{
    for (int i = 0; i < n; i++)
    {
        if (Random()%2==0)
            currentThread->Yield();
        //synchBarrior->Synch();
        printf("thread %d, loop %d\n", currentThread->getTID(), i);
        //synchBarrior->Synch();
    }
}

void
ThreadTest10()
{
    Thread *syncht[128], *unsyncht[128];
    totalSynch = 3;
    synchBarrior = new Barrior(totalSynch);
    printf("SYNCHRONIZED SCENE:\n");
    for (int i = 0; i < totalSynch; i++)
        syncht[i] = new Thread("synched thread", 0);
    for (int i = 0; i < totalSynch; i++)
        syncht[i]->Fork(synchThread, 3);
    currentThread->Yield();
    printf("\nUNSYNCHRONIZED SCENE:\n");
    for (int i = 0; i < totalSynch; i++)
        unsyncht[i] = new Thread("unsynched thread", 0);
    for (int i = 0; i < totalSynch; i++)
        unsyncht[i]->Fork(unsynchThread, 3);
}
/*****  LAB 3 END  *****/

/***** LAB 3 BEGIN *****/
RW_Lock *rw_lock;
int rw_capacity = 0;

void rw_read_thread(int n)
{
    for(int i = 0; i < n; i++)
    {
        for (int j = 0; j < rw_capacity; j++)
        {
            while (Random()%2==0)
            {
                //printf("Reader %d Yield...\n", currentThread->getTID());
                currentThread->Yield();
            }
            printf("Reader %d reads item %d as: %d\n", 
                currentThread->getTID(), j, rw_lock->read(j));
        }
        currentThread->Yield();
    }
}

void rw_allread_thread(int dummy)
{
    printf("The items are:\n\t");
    for (int j = 0; j < rw_capacity; j++)
        printf("%d ", rw_lock->read(j));
    printf("\n");
    currentThread->Yield();
}

void rw_write_thread(int n)
{
    for(int i = 0; i < n; i++)
    {
        int n_w = Random() % rw_capacity;
        int idx = Random() % rw_capacity;
        while (n_w <= 0)
            n_w = Random() % rw_capacity;
        while (idx >= rw_capacity - 1)
            idx = Random() % rw_capacity;
        rw_lock->lock_acquire();
        printf("Writer %d starts writing.\n", currentThread->getTID());
        for (int cnt = 0; cnt < n_w && cnt + idx < rw_capacity; cnt++)
        {
            while (Random() % 2 == 0)
            {
                printf("\tWriter %d Yield...\n", currentThread->getTID());
                currentThread->Yield();
            }
            if(rw_lock->locked_write(currentThread->getTID(), cnt + idx) < 0)
                printf("Writer %d writes without holding the RW Lock!\n\tThe RW Lock is held by thread %d\n",
                    currentThread->getTID(), rw_lock->get_heldBy()->getTID());;
            printf("\tWriter %d writes item %d as: %d\n",
                currentThread->getTID(), cnt + idx, currentThread->getTID());
        }
        printf("Writer %d finishes writing.\n", currentThread->getTID());
        rw_lock->lock_release();
        rw_allread_thread(0);
    }
}

void
ThreadTest11()
{
    rw_capacity = 4;
    rw_lock = new RW_Lock(rw_capacity);
    rw_lock->lock_acquire();
    for (int i = 0; i < rw_capacity; i++)
        rw_lock->locked_write(-1, i);
    rw_lock->lock_release();
    printf("The content with %d items is initialized as:\n\t", rw_capacity);
    for (int i = 0; i < rw_capacity; i++)
        printf("%d ", rw_lock->read(i));
    printf("\n");

    Thread *r1 = new Thread("reader", 0);
    Thread *r2 = new Thread("reader", 0);
    Thread *w1 = new Thread("writer", 0);
    Thread *w2 = new Thread("writer", 0);
    Thread *w3 = new Thread("writer", 0);
    r1->Fork(rw_read_thread, 1);
    r2->Fork(rw_read_thread, 1);
    w1->Fork(rw_write_thread, 2);
    w2->Fork(rw_write_thread, 2);
    w3->Fork(rw_write_thread, 2);
}
/*****  LAB 3 END  *****/

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {

    /***** LAB 1 BEGIN *****/
    case 1:
        ThreadTest1(); break;
    case 2:
        ThreadTest2(); break;
        break;
    case 3:
        ThreadTest3(); break;
    case 0:
        ThreadTest0(); break;
    /*****  LAB 1 END  *****/

    /***** LAB 2 BEGIN *****/
    case 4:
        ThreadTest4(); break;
    case 5:
        ThreadTest5(); break;
    case 6:
        ThreadTest6(); break;
    case 7:
        ThreadTest7(); break;
    /*****  LAB 2 END  *****/

    /***** LAB 3 BEGIN *****/
    case 8:
        ThreadTest8(); break;
    case 9:
        ThreadTest9(); break;
    case 10:
        ThreadTest10(); break;
    case 11:
        ThreadTest11(); break;
    /*****  LAB 3 END  *****/
    default:
        printf("No test specified.\n");
        break;
    }
}


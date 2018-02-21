// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "filesys.h"
#include <cstdio>

/***** LAB 5 BEGIN *****/
void CreateFunc()
{
    int point = machine->ReadRegister(4);
    int name;
    int length;
    for (length = 0;; length++)
    {
        machine->ReadMem(point+length, 1, &name);
        if (name == 0)
            break;
    }
    printf("file name length = %d\n", length);
    char *filename = new char[length+1];
    for (int i = 0; i < length+1; i++)
    {
        machine->ReadMem(point+i, 1, &name);
        filename[i] = (char)name;
    }
    printf("file name = %s\n", filename);
    OpenFile *openfile = fileSystem->Open(filename);
    if (openfile != NULL)
    {
        printf("file %s already exists!\n", filename);
        delete openfile;
        delete [] filename;
        return;
    }
    if (!fileSystem->Create(filename, 128))
    {
        printf("fail to create file %s!\n", filename);
        delete [] filename;
        return;
    }
    printf("create file %s!\n", filename);
    delete [] filename;
    return;
}
/*****  LAB 5 END  *****/

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");

	/***** LAB 4 BEGIN *****/
#ifdef PHYSPAGE_ALLOC_TEST
  	printf("%s halt!\n", currentThread->getName());
#endif
	machine->freePhysPage();

#ifdef TLB_FIFO
	printf("USE FIFO TLB SUBSTITUTION!\n");
#endif
#ifdef TLB_LRU
	printf("USE LRU TLB SUBSTITUTION!\n");
#endif
#ifdef TLB_RANDOM
	printf("USE RANDOM TLB SUBSTITUTION!\n");
#endif
#ifdef USE_TLB
	printf("TLB MISS = %d\n", machine->tlb_miss);
	printf("TLB HIT = %d\n", machine->tlb_hit);
	printf("TLB MISS RATE = %.5f\n", (double)machine->tlb_miss/((double)machine->tlb_miss+(double)machine->tlb_hit));
#endif
    /*****  LAB 4 END  *****/
   	interrupt->Halt();
    } 

    /***** LAB 4 BEGIN *****/
    else if ((which == SyscallException) && (type == SC_Exit))
    {
#ifdef PHYSPAGE_ALLOC_TEST
    	printf("%s exit!\n",
    		currentThread->getName());
#endif
    	machine->freePhysPage();
		int NextPC = machine->ReadRegister(NextPCReg);
		machine->WriteRegister(PCReg, NextPC);
    }
    /*****  LAB 4 END  *****/

    /***** LAB 4 BEGIN *****/
    else if (which == PageFaultException)
    {
    	int vaddr = machine->ReadRegister(BadVAddrReg);
    	int vpn = vaddr / PageSize;
    	int tlb_pos = -1;
    	if (machine->tlb != NULL)	// TLB miss
    	{
    		DEBUG('a', "PageFaultException raised.\n");
    		int pos = -1;
    		for (int i = 0; i < TLBSize; i++)
    			if (machine->tlb[i].valid == FALSE)	// find an empyt entry
    			{
    				pos = i;
    				break;
    			}

#ifdef TLB_FIFO
    		if (pos == -1)	// no empty entry, FIFO substitute
    		{
    			pos = TLBSize - 1;
    			for (int i = 0; i < TLBSize - 1; i++)
    				machine->tlb[i] = machine->tlb[i+1];
    		}
#endif

#ifdef TLB_RANDOM
    		if (pos == -1)	// no empty entry, random substitute
    		{
    			pos = Random() % TLBSize;
    		}
#endif

#ifdef TLB_LRU
    		if (pos == -1)	// no empty entry, LRU substitute
    		{
    			int m = -1;
    			for (int i = 0; i < TLBSize; i++)
    				if (machine->LRU_timer[i] > m)
    				{
    					m = machine->LRU_timer[i];
    					pos = i;
    				}
    		}
    		for (int i = 0; i < TLBSize; i++)
    			machine->LRU_timer[i]++;
    		machine->LRU_timer[pos] = 0;
#endif
    		tlb_pos = pos;
    		// substitute
    		if (machine->pageTable[vpn].valid)
    		{
    			machine->tlb[pos].valid = true;
    			machine->tlb[pos].virtualPage = vpn;
    			machine->tlb[pos].physicalPage = machine->pageTable[vpn].physicalPage;
    			machine->tlb[pos].use = false;
    			machine->tlb[pos].dirty = false;
	    		machine->tlb[pos].readOnly = false;
    		}
    	}
    	if (machine->tlb == NULL || machine->pageTable[vpn].valid == FALSE) 	// Page table miss
    	{
#ifndef USE_VMEM
    		// All physical memory pages are in the page table,
    		// thus this kind of PT miss won't happen.
    		ASSERT(FALSE);
#endif
    		

    		OpenFile *file = fileSystem->Open(machine->vmfile[machine->vm_idx]);
    		if (file == NULL)
    		{
    			printf("No virtual memory file found!\n");
    			ASSERT(false);
    		}
    		int pos = machine->allocPhysPage();	// find a free page in bitmap

#ifdef PT_ZERO
    		printf("Page fault... ");
    		if (pos != -1)
    			printf("Load virtual page %d into physical page %d.\n",
    				vpn, pos);
    		else	// no free page, substitute
    		{
    			pos = 0;
    			for (int j = 0; j < machine->pageTableSize; j++)
    				if (machine->pageTable[j].physicalPage == 0)
    				{
    					if (machine->pageTable[j].dirty)
    						file->WriteAt(&(machine->mainMemory[pos*PageSize]),
    							PageSize, machine->pageTable[j].virtualPage*PageSize);
    					machine->pageTable[j].valid = FALSE;
    					printf("Substitute virtual page %d with virtual page %d into physical page %d.\n",
    						j, vpn, pos);
    					break;
    				}
    		}
#endif
    		
    		file->ReadAt(&(machine->mainMemory[pos*PageSize]),
    					 PageSize, vpn*PageSize);
    		machine->pageTable[vpn].valid = TRUE;
    		machine->pageTable[vpn].physicalPage = pos;
    		machine->pageTable[vpn].use = FALSE;
   			machine->pageTable[vpn].dirty = FALSE;
  			machine->pageTable[vpn].readOnly = FALSE;
  			if (machine->tlb != NULL)	// if use tlb, put this entry in tlb
  			{
  				machine->tlb[tlb_pos].valid = true;
    			machine->tlb[tlb_pos].virtualPage = vpn;
    			machine->tlb[tlb_pos].physicalPage = machine->pageTable[vpn].physicalPage;
    			machine->tlb[tlb_pos].use = false;
    			machine->tlb[tlb_pos].dirty = false;
	    		machine->tlb[tlb_pos].readOnly = false;
  			}

    		delete file;
    	}
    }
    /*****  LAB 4 END  *****/
    else if ((which == SyscallException) && (type == SC_Create))
    {
        printf("\nCreate system call!\n");
        CreateFunc();
    }
    else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }
}

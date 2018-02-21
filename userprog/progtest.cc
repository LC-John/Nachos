// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

#ifdef PHYSPAGE_ALLOC_TEST
void MirrorThread(int dummy)
{
	if (dummy == 1)
		printf("Hahaha, I'm the mirror thread!\n");
	else
		printf("Oh, I'm the original thread!\n");
	machine->Run();
}
#endif

void
StartProcess(char *filename)
{
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

#ifdef PHYSPAGE_ALLOC_TEST
    OpenFile *executable2 = fileSystem->Open(filename);
    AddrSpace *space2;
    Thread *thread = new Thread("mirror");
#endif

    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }

#ifdef PHYSPAGE_ALLOC_TEST
    printf("Allocate %s addr space...\n", currentThread->getName());
#endif
    space = new AddrSpace(executable); 
#ifdef PHYSPAGE_ALLOC_TEST
    printf("Allocate mirror addr space...\n");
    space2 = new AddrSpace(executable2);
#endif
    currentThread->space = space;   
#ifdef PHYSPAGE_ALLOC_TEST
    space2->InitRegisters();
    space2->RestoreState();
    thread->space = space2;
    thread->Fork(MirrorThread, 1);
    currentThread->Yield();
#endif

#ifndef USE_VMEM
    delete executable;			// close file
#ifdef PHYSPAGE_ALLOC_TEST
    delete executable2;
#endif
#endif

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam
    ASSERT(FALSE);			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}


void 
SynchConsoleTest(char *in, char *out)
{
    char ch;

    SynchConsole *con = new SynchConsole(in, out, 0);
    
    for (;;) {
    ch = con->SynchGetChar();
    con->SynchPutChar(ch);   // echo it!
    if (ch == 'q') return;  // if q, quit
    }
}
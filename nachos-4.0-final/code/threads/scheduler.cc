#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"

// TODO: Define sorting rules for L1 and L2 ReadyQueues

static int CompareRemainingBurstTime(Thread* a, Thread* b) {
    return a->getRemainingBurstTime() - b->getRemainingBurstTime();
}

static int CompareThreadID(Thread* a, Thread* b) {
    return a->getID() - b->getID();
}

Scheduler::Scheduler()
{
    // TODO: Initialize L1, L2, L3 ReadyQueues

    // L1 uses preemptive SRTN, sorted by remaining burst time
    L1ReadyQueue = new SortedList<Thread*>(CompareRemainingBurstTime);

    // L2 uses FCFS, sorted by thread ID
    L2ReadyQueue = new SortedList<Thread*>(CompareThreadID);

    // L3 uses round-robin
    L3ReadyQueue = new List<Thread*>();

    toBeDestroyed = NULL;
}

Scheduler::~Scheduler()
{
    // TODO: Delete L1, L2, L3 ReadyQueues
    delete L1ReadyQueue;
    delete L2ReadyQueue;
    delete L3ReadyQueue;
}

void Scheduler::ReadyToRun(Thread* thread)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    Statistics* stats = kernel->stats;

    // According to priority of Thread, put them into corresponding ReadyQueue.
    // After inserting Thread into ReadyQueue, reset necessary values.

    int queueLevel = -1;
    int priority = thread->getPriority();

    if (priority >= 100 && priority <= 149) {
        L1ReadyQueue->Insert(thread);
        queueLevel = 1;
        
        // Check for preemption in L1
        Thread* currentThread = kernel->currentThread;
        if (currentThread != NULL && currentThread->getPriority() >= 100 && currentThread->getPriority() <= 149) {
            if (thread->getRemainingBurstTime() < currentThread->getRemainingBurstTime()) {
                kernel->currentThread->Yield();
            }
        }
    } else if (priority >= 50 && priority <= 99) {
        L2ReadyQueue->Insert(thread);
        queueLevel = 2;
    } else if (priority >= 0 && priority <= 49) {
        L3ReadyQueue->Append(thread);
        queueLevel = 3;
    }

    DEBUG('z', "[InsertToQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thread->getID() << "] is inserted into queue L[" << queueLevel << "]");

    thread->setStatus(READY);
}

Thread* Scheduler::FindNextToRun()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    // Find the next Thread to run from ReadyQueues

    Thread* nextThread = NULL;
    int queueLevel = -1;

    if (!L1ReadyQueue->IsEmpty()) {
        nextThread = L1ReadyQueue->RemoveFront();
        queueLevel = 1;
    } else if (!L2ReadyQueue->IsEmpty()) {
        nextThread = L2ReadyQueue->RemoveFront();
        queueLevel = 2;
    } else if (!L3ReadyQueue->IsEmpty()) {
        nextThread = L3ReadyQueue->RemoveFront();
        queueLevel = 3;
    }

    if (nextThread) {
        DEBUG('z', "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is removed from queue L[" << queueLevel << "]");
    }
    return nextThread;
}

void Scheduler::updateQueuePriority(List<Thread*>* queue, int queueLevel)
{
    ListIterator<Thread*> *iter = new ListIterator<Thread*>(queue);
    Thread* thread;
    int oldPriority, newPriority;

    for (; !iter->IsDone(); iter->Next()) {
        thread = iter->Item();
        if (kernel->stats->totalTicks - thread->getWaitTime() > 400) {
            oldPriority = thread->getPriority();
            newPriority = oldPriority + 10;
            DEBUG('z', "[UpdatePriority] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thread->getID() << "] changes priority from [" << oldPriority << "] to [" << newPriority << "]");

            if (newPriority > 149) newPriority = 149;

            thread->setPriority(newPriority);
            thread->setWaitTime(kernel->stats->totalTicks);
            queue->Remove(thread);
            ReadyToRun(thread);
        }
    }
    delete iter;
}

void Scheduler::UpdatePriority()
{
    updateQueuePriority(L1ReadyQueue, 1);
    updateQueuePriority(L2ReadyQueue, 2);
    updateQueuePriority(L3ReadyQueue, 3);
}


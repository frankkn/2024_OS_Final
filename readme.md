# Operating Systems

Final project: Multilevel Feedback Queue Scheduler with Aging Mechanism

NTHU CS342300 (2024 Operating Systems)

## Project Overview

This project implements a multilevel feedback queue (MLFQ) scheduler for an operating system, incorporating an aging mechanism to prevent starvation. The scheduler is designed to handle processes with different priorities and scheduling algorithms based on their priority levels.

## Key Features

### 1. Multilevel Feedback Queue (MLFQ)
- **Three Levels of Queues:**
  - **L1**: Highest priority queue.
  - **L2**: Medium priority queue.
  - **L3**: Lowest priority queue.
- **Priority Ranges:**
  - **L1**: Priorities 100-149.
  - **L2**: Priorities 50-99.
  - **L3**: Priorities 0-49.

### 2. Scheduling Algorithms
- **L1 Queue**: Uses Preemptive Shortest Remaining Time Next (SRTN) scheduling.
  - Ensures that the thread with the shortest remaining burst time is always running.
- **L2 Queue**: Uses First-Come First-Served (FCFS) scheduling.
  - Threads are executed in the order of their arrival.
- **L3 Queue**: Uses Round-Robin scheduling with a time quantum of 200 ticks.
  - Threads are executed in a cyclic order, with each thread running for a maximum of 200 ticks before moving to the next.

### 3. Aging Mechanism
- To prevent starvation, the priority of a process is increased by 10 if it waits in the queue for more than 400 ticks.
- This priority update occurs during the next timer alarm interval, ensuring smooth and timely priority adjustments.

# How to compile and run the test case

$ git clone https://github.com/yourusername/your-repo-name.git  
$ tar -zxf <Compressed-File>  
$ cd nachos-4.0-final/code  
$ make clean  
$ make  
$ cd userprog/  
$ ./nachos -epb ../test/hw2_test1 40 5000 -epb ../test/hw2_test2 80 4000 -d -z  


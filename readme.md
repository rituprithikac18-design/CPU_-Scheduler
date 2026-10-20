# CPU Scheduler & Page Replacement Simulator

A C++ simulation of core operating systems mechanisms — CPU scheduling, memory (page replacement), and process synchronization — built to understand how an OS manages competing processes under limited CPU, memory, and concurrency.

## Overview

This project implements and compares classic OS algorithms end to end rather than treating each topic as an isolated exercise: scheduling, memory management, and synchronization live in the same codebase so their trade-offs can be examined side by side (e.g., how a scheduling policy affects memory pressure, or how lock granularity affects throughput).

## Features

### CPU Scheduling
- First-Come, First-Served (FCFS)
- Shortest Job First (SJF)
- Round Robin
- Priority Scheduling

### Page Replacement
- FIFO (First In, First Out)
- Second-Chance
- Least Recently Used (LRU)
- Optimal Page Replacement (Belady's algorithm, used as a theoretical baseline)

### Synchronization
- Producer-Consumer problem using mutexes and semaphores
- Shared memory implementation (System V IPC)

### Utilities
- Bitwise-operation helpers used internally by the scheduling and paging modules

## What this adds beyond the base algorithms

- **Context-switch cost is modeled**, not ignored — scheduling comparisons account for a fixed overhead per switch, which changes which algorithm performs best at small quantum sizes.
- **Page-fault rate and hit ratio are reported per policy** across the same reference string, so FIFO/LRU/Optimal can be compared directly rather than run in isolation.
- **An unsynchronized version of the producer-consumer demo is included alongside the fixed version**, so the race condition is visible before seeing the fix.

## Project Structure

```
cpu-scheduler-page-replacement-sim/
├── cpuScheduling.cpp   # FCFS, SJF, Round Robin, Priority Scheduling
├── pageReplacement.cpp # FIFO, Second-Chance, LRU, Optimal
├── sync.cpp            # Producer-consumer synchronization (mutexes, semaphores, shared memory)
├── bitwisemult.cpp     # Bitwise-operation utilities
└── README.md
```

## Dependencies

- Standard C++ libraries (`iostream`, `vector`, etc.)
- POSIX Threads (`pthread.h`)
- System V IPC (`sys/ipc.h`, `sys/shm.h`) — Linux/macOS only; not available natively on Windows without WSL

## Build & Run

```bash
git clone https://github.com/<your-username>/cpu-scheduler-page-replacement-sim.git
cd cpu-scheduler-page-replacement-sim

# Compile
g++ -o cpuScheduling cpuScheduling.cpp -lpthread
g++ -o pageReplacement pageReplacement.cpp -lpthread
g++ -o syncExperiment sync.cpp -lpthread
g++ -o bitwisemult bitwisemult.cpp

# Run
./cpuScheduling
./pageReplacement
./syncExperiment
./bitwisemult
```

## Example Usage

### CPU Scheduling
```cpp
int mode = 1; // FCFS
queuer q(processArray, arrayLength);
cpuschedule(&q, mode, &timerInstance, quantumSize);
```

### Page Replacement
```cpp
buffer b(bufferSize);
b.refpage(pagePointer, algorithmNumber, sequenceArray, sequenceLength);
```

### Synchronization
```cpp
create_shared_memory();
// Initialize semaphores and threads
produce(NULL); // Producer thread function
consume(NULL); // Consumer thread function
```

## Sample Output

```
Algorithm: Round Robin (quantum=4, context-switch=1ms)
| P1 | P2 | P3 | P1 | P4 | P2 |
0    4    8    11   15   19   22

Avg Waiting Time:    6.4 ms
Avg Turnaround Time: 11.2 ms
Context Switches:    5
```

## Contributing

Contributions are welcome — please open an issue or submit a pull request.

## License

MIT — see the LICENSE file for details.
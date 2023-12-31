# dominguez-iso1

Static Operating System running in Cortex M4F

| Requirement                        | Subrequirement                | Compliance Status                                  |
| ---------------------------------- | ----------------------------- | -------------------------------------------------- |
| Memory Type                        | Choice of Memory Type         | Static.                                            |
| Number of Supported Tasks          | Support for Up to 8 Tasks     | Supports 8 tasks and is configurable.              |
| Hardware IRQ Management            |                               | Supported for 2 devices.                           |
| Kernel Control Structure           | Last Error                    | Reports 4 errors.                                  |
|                                    | Operating System State        | Supports 3 states.                                 |
|                                    | Scheduling Flag               | Supported                                          |
|                                    | Task Pointers                 | Supported.                                         |
| Task Control Structure             | Task Fields                   | Supported.                                         |
|                                    | Optional: ID Number and Name  | Supports ID numbers, but not names.                |
| Task Execution States              | Task States                   | Supports 4 states.                                 |
| Stack Size (256 bytes)             |                               | Supported and configurable.                        |
| Priority Implementation (4 levels) |                               | Supports up to 4 priority levels.                  |
| Scheduling Policy (Round-Robin)    |                               | Supported.                                         |
| System Tick (1 [ms])               |                               | Supported and configurable.                        |
| OS Hooks                           | tickHook                      | Supported and replaceable.                         |
|                                    | taskIdle                      | Supported and replaceable.                         |
|                                    | returnHook                    | Supported and replaceable.                         |
|                                    | errorHook                     | Supported and replaceable.                         |
| OS API                             | Delay Function (delay)        | Supported.                                         |
|                                    | Binary Semaphores             | Supported, with counter semaphores and timeouts.   |
|                                    | Queues (queue)                | Supported, with user-selectable RAM memory buffer. |
|                                    | Critical Sections             | Supported.                                         |
|                                    | Scheduling Forced (cpu yield) | Supported.                                         |

## Generate documentation
Doxygen documentation is recommended to explore the project.

To use it the following step must be done.

Install packages needed for doc generation and visualization.

```
sudo apt-get install doxygen
sudo apt-get -y install graphviz
chmod +x generate_docs.sh
```
Generate and open documentation running generate_docs script.

```
./generate_docs.sh

```
## Architectures supported

The project support the following architectures:
- LPC4337 (the define ```ARCH_NXP_LPC_43XX``` must be set as global in the project).
- STM32F429 (the define ```ARCH_STM_32_F4XX``` must be set as global in the project).

## Task API
Kernel header from OSAL must be included:
```
#include "osKernel.h"
```
### Create Task
The system allow create up to ```MAX_NUMBER_TASK``` using the method ```osTaskCreate```. 

The system is able to create Tasks in all the life of the app.


### Delay
```
#include "osKernel.h"
```
Delay API block the task for the TICK chosen in the argument when ```osDelay``` method is called. At interrupt context delays
will be ignored.

## Semaphores API
```
#include "osSemaphore.h"
```
Counter semaphores are supported without timeout. A semaphore taken will lock forever a task or until another task give the semaphore.

For a more complete tool check doxy documentation of ```os_semaphore.h```.

## Queue API
```
#include "osQueue.h"
```
### Features
- Queue are supported with timeout. 
- A queue will lock a task for time chosen if is full (at sending) or empty (at receiving), except another task do the oposite action before the timeout. 
- The size of each queue is defined by ```MAX_SIZE_QUEUE``` and is only possible to choose the size of the element. 
- Any timeout will be ignored at interrupt context, so if the operation can't be done the interrupt will continue the following operation.
- The queue API report if the Send/Receive action has completed successfully in any context.

For a more complete tool check doxy documentation of ```os_queue.h```.

## OS Configuration
The OS allow 4 level of priorities at max. Is possible to choose lower priority quantities with ```PRIORITY_LEVELS``` define.

The stack of each task is defined by ```MAX_STACK_SIZE``` and Systic period is defined by ```SYSTICK_PERIOD_MS```.

The Semaphores with counter can have until ```MAX_SEMPH_COUNT``` counts.

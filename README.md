# dominguez-iso1
Static Operating System running in Cortex M4F

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

## Task API
Kernel header from OSAL must be included:
```
#include "osKernel.h"
```
### Create Task
The system allow create up to ```MAX_NUMBER_TASK``` using the method ```osTaskCreate```. The system is able to create Task in all the life of the app.


### Delay
```
#include "osKernel.h"
```
Delay API block the task for the TICK chosen in the argument when ```osDelay``` method is called.

## Semaphores API
```
#include "osSemaphore.h"
```
Counter semaphores are supported without timeout. A semaphore taken will lock forever a task or until another task give the semaphore.
For a more complete tool check doxy documentation of ```os_semaphore.h```

## Queue API
```
#include "osQueue.h"
```
Queue are supported with timeout. A queue will lock a task for time chosen if is full (at sending) or empty (at receiving), except another task do the oposite action before the timeout. The size of each queue is defined by ```MAX_SIZE_QUEUE``` and is only possible to choose the size of the element.

For a more complete tool check doxy documentation of ```os_queue.h```.

## OS Configuration
The OS allow 4 level of priorities at max. Is possible to choose lower priority quantities with ```PRIORITY_LEVELS``` define.
The stack of each task is defined by ```MAX_STACK_SIZE``` and Systic period is defined by ```SYSTICK_PERIOD_MS```.
The Semaphores with counter can have until ```MAX_SEMPH_COUNT``` counts.

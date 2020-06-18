# Multi-reader-writer-with-Synchronization

### IMPLEMENTATION
* I have Implemented mlti Reader Writer with synchronization (no race conditions ). The code is in C.

The **main.c**  file handles every operation of writers and readers, the **rw.c** file has a
client-server relationship with main.c. </br>

The global shared queue is an array which keeps two pointers front & rear for the shared
queue, each element of this shared queue can be accessed via multiple threads at the
same time hence each element has locks in a form of a **binary semaphore** (r2_mutex)
ensuring no race conditions. </br>

Since only one write can update the queue at a certain moment a binary semaphore
write_mutex which ensures no other reader or writer can access the queue at that point
in time. Similarly dequeuing & enqueuing is also atomic among readers/writers hence
binary semaphore **deq_mutex & enq_mutex** is implemented to ensure the same. </br>

To ensure multiple readers can read a certain element concurrently, r_num is implement
which stores the number of threads accessing a certain element, also r1_mutex is
implemented to ensure there are no race conditions in r_num. </br>
Main.c spawns new threads for every reader and writer and waits for user commands from rw.c.

### COMPILE AND TEST

* To compile use command ‘make’
  * ./main to run the server (MUST BE STEP 1)
  * ./rw to run reader or writers
* Ensure that only one main is running.
* Once main program is running, on a new terminal run the rw program. The program can handle almost 100 readers and 100 writers.

### USER INPUTS
* For writer :
  * To enqueue enter (1) followed by the element to be enqueued
  * To overwrite enter (2) followed by the index of the array to be overridden.
  
        $ Select Operation to be performed
         1 - overwrite data
         2 - enqueue data
         
* For reader:
  * To read a certain element from queue enter (1) followed by the index
  * To dequeue a element from queue enter (2)
  
        Select Operation to be performed
        1 - read data
        2 - dequeue data
    
### EXPECTED OUTPUT
Once given the command the program prints out the output of the program to the
corresponding terminal running it.

### ERROR HANDLING
* Prompts the user if index requested for any operation is out of bounds of the queue followed by an error message </br>
* Doesn’t allow writer to enqueue to a full queue. </br>
* Doesn’t allow reader to dequeue an empty queue. </br>
* Prompts if user enters anything other than 1 or 2 to request operations for readers and writers. </br>

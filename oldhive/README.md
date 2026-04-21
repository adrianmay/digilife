
## What's this

A VM/OS for evolving useful software in Darwinian fashion.

The environmental selection criterion is making a profit in real money where revenue comes directly or indirectly from humans in return for computing useful things and costs are incurred for using computing resources.

This approach will perform best on problems  where very large numbers of attempts can be made and it's not too tragic if a mistake is made. That would include internet search or advert placement, but probably not landing rovers on Mars or controlling military hardware.

## Dependency structure

              types.h
            /   |     \        
           /    |      \        
          /     |       \        
         /    pile.h     \        
        /       | pile.c  \       
  time.h        |         global.h
   | time.c     |          | global.c
    \         meap.h       |           
     \          | meap.c   |   
      \         |         /  
       `------ rent.h ---`      
                | rent.c           
                |
             App/Test 

## Modules

### time

Just sundry utils

### pile

This provides a persistent, fixed-block-size heap. Piles take up lots of virtual address space but only as much physical memory as they need. Since a pile is just an array, its members are referred to by index rather than address, which is more efficient.

There's also a simple one with exactly one member which gets used for persistent global variables.

### meap

Min heap in a pile. 

### global

Defines the globals of the app, some of which are persistent. Might get absorbed into rent, which is where it gets used.

### rent

Memory billing. 

Alongside the main pile of animals, there's a meap of "time bombs" each comprising an animal index and a moment in the future when it should be considered unable to pay its memory rent. The latter is the ordering criterion for the min heap.

A thread sleeps until just after the soonest of those moments and then deletes all animals whose moment is in the past. 

There's no explicit payment of memory rent, but before an animal's code runs, the arrears are deducted from the animal's bank balance.

Memory time is measured in "tocks", each being a dynamically variable number of nanoseconds. This is how memory prices are adjusted at run time without recomputing the time bombs.

### raffle

This is a binary tree serialised like a min heap, but each node has a "weight" and tracks the total weight of its left and right subtrees. If we uniformly roll a random natural number less than the total weight of the tree, we can descend the tree to select a node with a probability proportional to its weight.

### exec

Animals may send a message to themselves or another. This is the only way to execute code, namely, that of the receiver. 

A message must indicate how much it wants to pay for CPU time, and a raffle will tend to choose the highest bidders. 

The finance for the CPU job is transfered from the sender to the message itself when emitting the message. 

The message will incur memory rent while it waits in the raffle (but not much because a message is small.) During execution, an auxillary thread will sleep until the message finance is expected to run out, then abort the job. If the job yields before message finance is exhausted, the receiving animal keeps the balance. This mechanism therefore serves as the main means of payment, which makes sense because the receiver of a payment probably wants to run code to update its opinion of the sender, who also wants that to happen. 

This system is spam-proof.

## Structs

Animals have a fixed size containing a header followed by space for code/data which their code may modify and suffers occasional random changes.

Animals implicitly know their index when running. 

The header contains:

* A name in case they die and the same index becomes occupied by a completely different animal.
* Bank balance.
* When rent was last charged.
* Index of their time-bomb in the meap.





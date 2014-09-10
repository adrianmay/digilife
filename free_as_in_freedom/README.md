This preamble demonstrates *completely* free evolution. Being free of human influence, it's not likely to do anything that coincides with our interests, but it demonstrates the starting point of purely Darwinian evolution from which we'll depart.

It boots a PC into protected mode with one userland physical segment that's aliased as both code and data, initialised with random contents and executed. All 256 exceptions/interrupts are vectored to just jump back into the chaos, after scrambling some byte near where the exception occured. There's also a timer set at 100Hz which scrambles some randomly chosen byte. 

During the demo, the screen shows a histogram of the byte values in the chaos. On several occasions, the histogram is seen to be dominated by a handful of values, before gradually returning to apparent randomness. One assumes that those values would make sense if disassembled. Tweaks to the tank size and aggresion of scrambling may be required. 

A more successful run would be characterised by successive periods of domination followed by intrusion of a specific set of alternative values. If the domination is intruded upon by random values then it's simply dying.

There seem to be bugs in the OS such that runs just freeze after roughly an hour.

Thanks to: 

 Gregor Brunmar's MuOS for the startup, GDT and screen output stuff.

 Alexander Blessing's Flick for the interrupt handling.

 Brandon Friesen for the otherwise elusive secrets of hardware interrupts in PM.

 Alexei A. Frounze for the task handling

 Stanislav Shwartsman for fantastic support in the context of bochs.


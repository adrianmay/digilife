
================================================
Commit: 43734d6e21026449914db07a533de2409703ff25
================================================

First result.

After about 10 hours in bochs on a 2GHz machine, this version discovers the program:

 lodsd eax, dword ptr ds:[esi] ; ad
 xchg dword ptr ds:[esi], eax ; 8706
 inc ebp                   ; 45

aligned on 4-byte boundaries. This populates the whole memory, continuously repairs VM-damage and appears to rule out further evolution.

The inc ebp is irrelevant here. The program copies the dword at si to si+4. If si and bp coincide, the program will copy itself and the new copy will be executed next, thus filling the memory to the end. If the direction flag is cleared, then the copy will be made at si-4, but won't be executed next. 

This is a replicator but not specifically a self-replicator. It succeeds in populating the whole memory because there's a slight cheat in the VM whereby jump-ins after timer interrupts and faults always initialise si to pc. 

It seems that early regions of memory can only be repaired if mutations cause si to point to them (or the direction flag to be cleared) when the program is run, so there remains some hope of evolution in those regions.

(Jump-ins always occur on 4-byte boundaries in this version, so no other interpretations of the op-code sequence are relevant.)

On a machine with this si=pc 'cheat', the program can be seen as a rather inefficient self-replicator. 

A weakness of this version is that VM-damage changes entire bytes rather than bits. With that fixed, more damage can be tolerated with more interesting results. In this way, it is hoped that a true self replicator can be made to emerge on a machine without the si=pc cheat. 

It's also worth noting that the program fits into 4 bytes and can thus be copied in a single instruction. (This version targets 32 bit CPUs.) If we graduate to 64 bit CPUs, longer programs could be copied in one go, but perhaps that's just what we don't want: interesting programs would take longer to emerge by chance, and no progress would be made towards programs of unlimited length.




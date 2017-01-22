# Cache-And-Virtual-Memory-Simulators
Simulators for a direct mapped cache, a four-way set associative cache, and virtual memory. 

dmcache.cpp simulates a direct mapped cache.
sacache.cpp simulates a four-way set associative chace that uses a least recently used (LRU) replacement policy.

Both take a filename as an input from the command line. The file will be an ASCII file where each line consists of the following format: 

~~~~
Bytes 1-2: 16 Bit Address
Byte 3: Read/Write
Byte 4: 8 bits of data
~~~~

vm.cpp simulates a virtual memory system that has a 16 line page table (with 4KB pages), a physical address space of 1GB, and a virtual address space of 4GB. Only four pages can be in RAM at a time. It uses a clock replacement algorithm.

~~~~
The first 16 lines in the file are the addresses of the first bytes of each page in the page table.
The rest of the lines are the addresses that are needed by the executable.
~~~~

Virtual Memory Manager - Programming Assignment #6

AUTHOR: Kamron Swingle

COURSE: CPSC-380 Operating Systems

DATE: Fall 2025

PROGRAM DESCRIPTION:

This program simulates a virtual memory manager that translates logical

addresses to physical addresses. The program implements:

- Page Table: 256 entries (one per page)

- TLB: 16 entries with LRU (Least Recently Used) replacement

- Physical Memory: 128 frames (32,768 bytes total)

- Page/Frame Size: 256 bytes each

- Page Replacement: FIFO (First In First Out) policy

- Demand Paging: Pages loaded from BACKING_STORE.bin on page faults

The program reads logical addresses from a file, translates them using the

TLB and page table, handles page faults by loading from backing store, and

outputs the physical address and value at that location.

COMPILATION:

gcc -o vmmgr vmmgr.c

EXECUTION:

./vmmgr addresses.txt

REQUIRED FILES:

1. vmmgr.c - Source code for the virtual memory manager

2. addresses.txt - Input file containing 1000 logical addresses (0-65535)

3. BACKING_STORE.bin - Binary backing store file (65,536 bytes)

SAMPLE OUTPUT:

/.. More lines above ../

Virtual address: 37606 Physical address: 10470 Value: 36

Virtual address: 18426 Physical address: 30202 Value: 17

Virtual address: 21238 Physical address: 20982 Value: 20

Virtual address: 11983 Physical address: 3535 Value: -77

Virtual address: 48394 Physical address: 3594 Value: 47

Virtual address: 11036 Physical address: 3868 Value: 0

Virtual address: 30557 Physical address: 4189 Value: 0

Virtual address: 23453 Physical address: 4509 Value: 0

Virtual address: 49847 Physical address: 4791 Value: -83

Virtual address: 30032 Physical address: 4944 Value: 0

Virtual address: 48065 Physical address: 18113 Value: 0

Virtual address: 6957 Physical address: 27693 Value: 0

Virtual address: 2301 Physical address: 21245 Value: 0

Virtual address: 7736 Physical address: 13112 Value: 0

Virtual address: 31260 Physical address: 5148 Value: 0

Virtual address: 17071 Physical address: 5551 Value: -85

Virtual address: 8940 Physical address: 5868 Value: 0

Virtual address: 9929 Physical address: 6089 Value: 0

Virtual address: 45563 Physical address: 6395 Value: 126

Virtual address: 12107 Physical address: 6475 Value: -46

Statistics:

-----------------

TLB hits = 55

TLB hit rate = 5.50%

Number of Translated Addresses = 1000

Page Faults = 538

Page Fault Rate = 53.80%

SOURCES:

Reading from file: https://www.tutorialspoint.com/cprogramming/c_file_io.htm and https://www.geeksforgeeks.org/c/basics-file-handling-c/

Virtual Memory: https://www.geeksforgeeks.org/operating-systems/virtual-memory-in-operating-system/

Address Translation and Paging: https://www.geeksforgeeks.org/operating-systems/paging-in-operating-system/

TLB: https://www.geeksforgeeks.org/operating-systems/translation-lookaside-buffer-tlb-in-paging/

Replacement Algorithms:

FIFO: https://www.geeksforgeeks.org/dsa/program-page-replacement-algorithms-set-2-fifo/

LRU: https://www.geeksforgeeks.org/system-design/lru-cache-implementation/

Bitwise Manipulation: https://www.geeksforgeeks.org/c/bitwise-operators-in-c-cpp/

Structs: https://www.tutorialspoint.com/cprogramming/c_typedef.htm

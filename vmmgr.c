/**
 * Virtual Memory Manager (vmmgr.c)
 * Programming Assignment #6
 * 
 * Author: Kamron Swingle
 * Course: CPSC-380 Operating Systems
 * Date: Fall 2025
 * 
 * Description:
 * This program simulates a virtual memory manager that translates logical addresses
 * to physical addresses using a page table and a TLB (Translation Lookaside Buffer).
 * It handles page faults by loading pages from a backing store file into physical memory.
 * The program uses a FIFO page replacement policy and an LRU policy for TLB replacement.
 * It reads logical addresses from a text file and outputs the corresponding physical addresses
 * and values stored in physical memory. It also provides statistics on TLB hits and page faults
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Maximum defined constants
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE 32768
#define NUM_FRAMES 128
#define TLB_SIZE 16

// page table, physical memory, and other global variables
int page_table[PAGE_TABLE_SIZE];
char physical_memory[PHYSICAL_MEMORY_SIZE];
int next_frame = 0;
int page_faults = 0;
int address_count = 0;

// TLB entry structure for each TLB slot
typedef struct {
    int page_number;
    int frame_number;
    int valid;  // 1 if entry is valid, 0 if empty
    int access_counter;
} TLB_Entry;

// TLB array
TLB_Entry tlb[TLB_SIZE];

// Statistics
int tlb_hits = 0;

int global_time = 0;

int fifo_pointer = 0;

// Mapping from frame number to page number for eviction purposes
int frame_to_page[NUM_FRAMES];

// Function prototypes
void update_tlb(int page_number, int frame_number, int current_time);
void load_page_from_backing_store(int page_number, int frame_number);

int main(int argc, char* argv[]) {
    // Open the address file and validate it
    FILE* address_file;
    int logical_address;

    if (argc != 2) {
        fprintf(stderr, "Error, usage: <program> <file.txt>\n");
        return 1;
    }

    // Open the address file
    address_file = fopen(argv[1], "r");
    if (address_file == NULL) {
        fprintf(stderr, "Error opening text file.\n");
        return 1;
    }

    // Check if BACKING_STORE.bin exists with a test open, if error then exit and close address_file
    FILE* backing_test = fopen("BACKING_STORE.bin", "rb");
    if (backing_test == NULL) {
        fprintf(stderr, "Error: BACKING_STORE.bin not found.\n");
        fclose(address_file);
        return 1;
    }
    fclose(backing_test);

    // Initialize page table - all entries invalid
    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
        page_table[i] = -1;
    }

    // Initialize TLB - all entries invalid
    for (int i = 0; i < TLB_SIZE; ++i) {
        tlb[i].valid = 0;
    }

    // Initialize frame to page mapping
    for (int i = 0; i < NUM_FRAMES; ++i) {
        frame_to_page[i] = -1;
    }

    // Read each logical address from the file
    while (fscanf(address_file, "%d", &logical_address) == 1) {
        // Increment global time for LRU tracking
        global_time++;

        // Mask to 16 bits to simulate 16-bit address space
        logical_address = logical_address & 0xFFFF; // Keep only the right 16 bits

        int page_number = (logical_address >> 8) & 0xFF; // Shift right by 8 to get the UPPER/LEFT 8 bits (the page number)
        int offset = logical_address & 0xFF; // want the remaining 8 bits

        // Increment total address count
        address_count++;

        int frame_number = -1; // we dont know the frame number yet
        // Check TLB for the page number
        int tlb_index = -1;
        for (int i = 0; i < TLB_SIZE; ++i) { // First we check the tlb for the frame
            // If we find it, we set the frame number and break
            if (tlb[i].valid && page_number == tlb[i].page_number) {
                // TLB hit
                frame_number = tlb[i].frame_number;
                // Update access counter for LRU
                tlb_index = i;
                // Update access counter
                tlb[i].access_counter = global_time;
                // Increment TLB hits
                tlb_hits++;
                // Exit the loop
                break;
            }
        }

        // If not found in TLB, check page table
        if (frame_number == -1) {
            // Not found in TLB, check page table
            frame_number = page_table[page_number];
            // If not found in page table, we have a page fault
            if (frame_number == -1) {
                // Page fault handling
                page_faults++;

                // Check if there is a free frame
                if (next_frame < NUM_FRAMES) {
                    // There is a free frame available
                    frame_number = next_frame;
                    // Load the page from backing store into physical memory
                    next_frame++;
                    // Load the page from backing store into physical memory
                    load_page_from_backing_store(page_number, frame_number);
                    // Update page table and frame to page mapping
                    page_table[page_number] = frame_number;
                    // Update frame to page mapping
                    frame_to_page[frame_number] = page_number;

                } else { // use fifo to evict someone
                    int frame_to_replace = fifo_pointer; // find which frame to replace
                    int evicted_page = frame_to_page[frame_to_replace]; // find the evicted page from the victim frame

                    // Now replace it
                    frame_number = frame_to_replace;

                    // Update page table to reflect eviction
                    page_table[evicted_page] = -1;
                    // Load the new page into the evicted frame
                    load_page_from_backing_store(page_number, frame_to_replace);
                    // Update page table and frame to page mapping
                    page_table[page_number] = frame_to_replace;
                    // Update frame to page mapping
                    frame_to_page[frame_to_replace] = page_number;
                    // Move fifo pointer (with wrap around)
                    fifo_pointer = (fifo_pointer + 1) % NUM_FRAMES;

                    // Invalidate any TLB entry that maps to the evicted page
                    for (int i = 0; i < TLB_SIZE; ++i) {
                        if (tlb[i].valid && tlb[i].page_number == evicted_page) {
                            tlb[i].valid = 0;
                            break;
                        }
                    }
                }
            }
        }

        // Update TLB if it was not a TLB hit
        if (tlb_index == -1) {
            // Not a TLB hit, update the TLB
            update_tlb(page_number, frame_number, global_time);
        }

        // Calculate physical address
        int physical_address = (frame_number << 8) | offset;

        // Retrieve the value from physical memory
        signed char value = physical_memory[physical_address];

        // Print the results
        printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
    }

    // Close the address file
    fclose(address_file);

    // Print statistics
    double page_fault_rate = (double) page_faults / address_count * 100.0;
    double tlb_hit_rate = (double) tlb_hits / address_count * 100.0;

    printf("\nStatistics:\n");
    printf("-----------------\n");

    printf("TLB hits = %d\n", tlb_hits);
    printf("TLB hit rate = %.2f%%\n", tlb_hit_rate);
    printf("Number of Translated Addresses = %d\n", address_count);
    printf("Page Faults = %d\n", page_faults);
    printf("Page Fault Rate = %.2f%%\n\n", page_fault_rate);

    return 0;
}

// Function to load a page from the backing store into physical memory
void load_page_from_backing_store(int page_number, int frame_number) {
    // Checking here again, just to be safe
    FILE* backing_store = fopen("BACKING_STORE.bin", "rb");
    if (backing_store == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin\n");
        exit(1);
    }

    // Seek to the correct position in the backing store
    fseek(backing_store, page_number * FRAME_SIZE, SEEK_SET);

    // Read the page into the physical memory at the correct frame
    fread(&physical_memory[frame_number * FRAME_SIZE], sizeof(char), FRAME_SIZE, backing_store);

    // Close the backing store file
    fclose(backing_store);
}

// Function to update the TLB using LRU replacement policy
void update_tlb(int page_number, int frame_number, int current_time) {
    for (int i = 0; i < TLB_SIZE; ++i) { // Find an empty spot on the TLB to add it
        if (!tlb[i].valid) {
            // Empty spot found
            tlb[i].page_number = page_number;
            // Update TLB entry
            tlb[i].frame_number = frame_number;
            // Mark as valid
            tlb[i].valid = 1;
            // Update access counter
            tlb[i].access_counter = current_time;
            return;
        }
    }

    // No empty spot found, need to replace an entry using LRU
    int smallest_counter = tlb[0].access_counter;
    // Find the TLB entry with the smallest access_counter
    int index_to_replace = 0;
    // if no empty spots exist, find the one with the smallest access_counter
    for (int i = 0; i < TLB_SIZE; ++i) {
        if (tlb[i].access_counter < smallest_counter) {
            // Found a smaller access counter
            smallest_counter = tlb[i].access_counter;
            // Update index to replace
            index_to_replace = i;
        }
    }

    // Replace the found TLB entry
    tlb[index_to_replace].page_number = page_number;
    // Update TLB entry
    tlb[index_to_replace].frame_number = frame_number;
    // Mark as valid
    tlb[index_to_replace].valid = 1;
    // Update access counter
    tlb[index_to_replace].access_counter = current_time;
}
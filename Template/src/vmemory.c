//Implement the API modeling the translation of virtual page address to a
//physical frame address. We assume a 32 bit virtual memory and physical memory.
//Access to the page table is only via the CR3 register.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "vmemory.h"

#define OUT_TLB "../bin/tlb_out.txt"
#define TLB_SIZE 8
#define ADDRESS_SIZE 32
#define OFFSET 0xfff 	  //111111111111 - 12 bit offset
#define MSB_32 0xFFC00000 //11111111110000000000000000000000
#define MSB_22 0x3FF000   //1111111111000000000000
#define MSB_MASK 0xFFFFF000 //11111111111111111111000000000000

bool FIFO_policy = true;
bool LRU_policy = false;
int rank[8]={-1,-1,-1,-1,-1,-1,-1,-1};
//structure to store the TLB and page table into
struct frame
{
	int pageNum;
	int frameNum;
};

///global variables
int **cr3;
struct frame TLB[8];
struct frame PT[1024][1024];
int TLBentries = 0;
int TLBhits = 0;
int total=0;
int max=-1;
//
// More static functions can be implemented here
//
int minindex(int * rank)
{
	int min=rank[0];
	int id=0;
	for (int i=0;i<TLB_SIZE;i++)
	{if (rank[i]<rank[id]){id=i;}}
	return id;
	}

// The implementation of get_vpage_cr3 is provided in 
// an object file, so no need to re-implement it
void initialize_vmanager(int policy) 
{
	// Set LRU policy when passed as a parameter
	if (policy)
		FIFO_policy = false;
	cr3 = get_vpage_cr3();
	
}

//
// The implementation of following functions is required
//
int translate_virtual_address(unsigned int v_addr)
{
	unsigned int offset=(v_addr & OFFSET);
	unsigned int page1=(v_addr & MSB_32)>>22;
	unsigned int page2=(v_addr & MSB_22)>>12;
	
	if (cr3[page1]==NULL) {return -1;}
	//if (cr3[page1][0]==-1) {printf("w");return -1;}
	unsigned int p_addr=((cr3[page1][page2])<<12)+offset;
	//TODO
	return p_addr;
}

void print_physical_address(int frame, int offset)
{
	//TODO
	if( frame==-1){printf("-1/n");return;}
	int toprint=(frame<<12)+offset;
	printf("0x%08x\n", toprint);
	return;
}


//  implement a function that takes an int parameter containing the virtual address 20 MSB and 
//  returns the physical frame base address mapped to that particular virtual address. 
//  TLB miss should return -1.
int get_tlb_entry(int virtualMSB)
{
	int page =((virtualMSB & MSB_MASK) >> 8);
	int firstTry = -1;
	total++;
	//traverse the TLB and check if entries are in the TLB 
	for(int i=0;i<8;i++)
	{
		if(TLB[i].pageNum == virtualMSB){
			firstTry = TLB[i].frameNum;
			TLBhits++;
			
			if (!FIFO_policy) {rank[i]=++max;}
			return firstTry;
		}
	} 

//	if the address is not in the TLB then return -1
	if(firstTry == -1){
		return -1;
	}
}

//  this function should take the 20 MSB of a virtual address, 
// i.e., the virtual page number, and the 20 MSB of a physical address, 
// i.e., the physical frame number, as parameters and populate the TLB.
void populate_tlb(int virtualAddress, int physicalAddress)
{
	if (FIFO_policy){
	int entry;
	for (entry=0;entry<TLBentries-1;entry++)
	{
		if(TLB[entry].pageNum == virtualAddress)
		{
			return;
		} 
	}

//if previous loop doesn't break we enter another loop where we shift array values 
// and then insert the new value
	if(entry == TLBentries-1){
		for(int i=0;i<entry;i++){
			
			TLB[i] = TLB[i+1];
		}
	}
	
	TLB[entry].pageNum=virtualAddress;
	TLB[entry].frameNum=physicalAddress;

//increment the count of entries if there is still room in TLB
	if(TLBentries<8){
		TLBentries++;
	}
}
	else 
	{

		int imin=minindex(rank);
	TLB[imin].pageNum=virtualAddress;
	TLB[imin].frameNum=physicalAddress;
	rank[imin]=++max;
		}


	return;
}
float get_hit_ratio()
{
	float ratio=(float)(TLBhits)/total;
	
	//TODO
	return ratio;
}

//Write to the file in OUT_TLB
void print_tlb()
{
	
	FILE * output = fopen(OUT_TLB, "w+");
	char s[50];
	for (int i=0; i<8;i++){
		sprintf(s,"0x%05x 0x%05x \n\n",TLB[i].pageNum,TLB[i].frameNum);
		
		fputs(s,output);
		}
	fclose(output);
	//TODO
	return;
}

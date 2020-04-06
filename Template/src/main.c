//main.c program to translate the virtual addresses from the virtual.txt
//into the physical frame address. Project 3 - CSci 4061

#include <stdio.h>
#include <stdlib.h>
#include "vmemory.h"
#include <fcntl.h>
#include <unistd.h>
#define INPUT_FILE "../bin/virtual.txt"

int extractnumber(char*a){
	
	char b[50];
	for (int i=2;i<50;i++)
	{b[i-2]=a[i];}
	int num = (int)strtol(b, NULL, 16);
	
	return num;
	}
int main(int argc, char* argv[])
{
	if (argc > 2) {
		printf("Too many arguments, enter up to one argument\n");
		exit(-1);
	}
	

	int policy = (argc == 2) ? 1:0;
	initialize_vmanager(policy);
	 FILE * input = fopen(INPUT_FILE, "r");
	 
	 //int fd= open(INPUT_FILE, RDONLY);
	char s[50];
	//fgets(s, 50, input);
	
	while(fgets(s, 50, input)!=NULL)
	{
		int vaddr=extractnumber(s);
		int vaddr_20=(vaddr & 0xFFFFF000)>>12;
		int paddr=translate_virtual_address(vaddr);
		int paddr_20=(paddr & 0xFFFFF000)>>12;
		
		
		int tbl_paddr_20= get_tlb_entry( vaddr_20 );
		printf("0x%08x \n",paddr);
		if(tbl_paddr_20!=paddr_20)
		{populate_tlb(vaddr_20, paddr_20);}	
	 
	} 

	 
	print_tlb();
	printf("Hit ratio: %f",get_hit_ratio());
	//TODO: Fill the algorithm by reading from INPUT_FILE
	
	//Free the page table
	free_resources();
	fclose(input);
	return 0;
}

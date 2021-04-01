//
// Virual Memory Simulator Homework
// One-level page table system with FIFO and LRU
// Two-level page table system with LRU
// Inverted page table with a hashing system 
// Submission Year: 2020
// Student Name: 이민훈
// Student Number: B611146
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#define PAGESIZEBITS 12			// page size = 4Kbytes
#define VIRTUALADDRBITS 32		// virtual address space size = 4Gbytes
int numProcess;
int nFrame;
struct procEntry {
	char *traceName;			// the memory trace name
	int pid;					// process (trace) id
	int ntraces;				// the number of memory traces
	int num2ndLevelPageTable;	// The 2nd level page created(allocated);
	int numIHTConflictAccess; 	// The number of Inverted Hash Table Conflict Accesses
	int numIHTNULLAccess;		// The number of Empty Inverted Hash Table Accesses
	int numIHTNonNULLAcess;		// The number of Non Empty Inverted Hash Table Accesses
	int numPageFault;			// The number of page faults
	int numPageHit;				// The number of page hits
	struct pageTableEntry *firstLevelPageTable;
	int eof;
	FILE *tracefp;
}*procTable;
struct pp
{
        int valid;
        int pid;
        unsigned virtual;
        unsigned node_index;
        struct pageTableEntry *prev;
	struct pageTableEntry *next;
        struct pageTableEntry *secondPageTable;
        struct phymem *address; 
};
struct pageTableEntry
{
	int valid;
	int pid;
	unsigned virtual;
	unsigned node_index;
	struct pageTableEntry *prev;
	struct pageTableEntry *next;
	struct pp link;
	struct pageTableEntry *secondPageTable;
	struct phymem *address; 
};
struct pageTableEntry node_queue;
struct pageTableEntry *hashTable;
struct pageTableEntry *node;
struct phymem
{
	int pid;
	int len;
	unsigned node_index;
	int index;
	int virtual;
	int virtual_fist;
	struct phymem * prev;
	struct phymem * next;
}*phymemory;

struct phymem use;
struct phymem valid;

void oneLevelVMSim() {
	int i;
	for(i=0; i < numProcess; i++)
		procTable[i].firstLevelPageTable = (struct pageTableEntry *) malloc(sizeof(struct pageTableEntry) * (1L <<(VIRTUALADDRBITS - PAGESIZEBITS))); 
	while(1){
		int count =0;
		if(count ==numProcess)
			break;

		for(i = 0; i < numProcess; i++) {
			unsigned addr;
			char rw;
			unsigned phy_addr;
			unsigned virtual_addr;
			unsigned offset;
			fscanf(procTable[i].tracefp,"%x  %c",&addr,&rw);
			if(!feof(procTable[i].tracefp))
			{
				/*		struct phymem *current = &use;
						printf("process %d virtual addr: %x use [ ", i, addr);
						int count =0;
						while(current->next != &use && count !=10)
						{       
						current = current->next;
						printf("%d%d  ", current->index ,current->pid);
						count++;
						}
						printf(" ] \n");
						*/
				procTable[i].ntraces ++;				
				virtual_addr = (addr >> PAGESIZEBITS) ;  // 앞주소 
				offset = addr % PAGESIZEBITS ; // offset 주소 
				if(procTable[i].firstLevelPageTable[virtual_addr].valid == 1)  // hit 되서 주소 찾아가는 작업 하고/  맞는 프로세스 인지 검사  
				{
					procTable[i].numPageHit ++;		
				}
				else    //fault 
				{	// valid queue 에서 하나 꺼내옴 !
					procTable[i].numPageFault ++;
					if( valid.next == &valid)
					{
						struct phymem *tmp2 = use.next;
						use.prev->next = tmp2;
						tmp2->prev = use.prev;
						use.prev = tmp2;
						//		use.next->next->prev = &use;
						use.next = use.next->next;
						tmp2->next = &use;
						use.next->prev = &use;
						int tmp = use.prev->pid;
						int j=0;
						j = use.prev->virtual;
						procTable[tmp].firstLevelPageTable[j].address  =NULL;
						procTable[tmp].firstLevelPageTable[j].valid =0;
					}
					else // 남은 메모리가 있을때 
					{	
						struct phymem *tmp = valid.next;
						use.prev->next  = valid.next;
						valid.next->prev = use.prev;
						use.prev = valid.next;
						valid.next = valid.next->next;
						tmp ->next = &use;
						valid.next->prev = &valid;
					}
					use.prev->virtual = virtual_addr;
					procTable[i].firstLevelPageTable[virtual_addr].address = use.prev;
					procTable[i].firstLevelPageTable[virtual_addr].address->pid = i;
					procTable[i].firstLevelPageTable[virtual_addr].valid = 1;					 
				}
				/*				struct phymem *current = &use;
								printf("use [ ");
								count =0;
								while(current->next != &use && count !=10)
								{
								current = current->next;
								printf("%d  ", current->index ,current->pid);
								count++;
								}
								printf(" ] \n");
								*/				// printf("%d\n",procTable[i].firstLevelPageTable[virtual_addr].address->index);
			}
			else
				procTable[i].eof = 1;
		}
		for(i=0; i<numProcess;i++)
		{
			if(procTable[i].eof ==1)
				count ++;
		}
		if(count ==numProcess)
			break;
	}	
	for(i=0; i < numProcess; i++) {
		printf("**** %s *****\n",procTable[i].traceName);
		printf("Proc %d Num of traces %d\n",i,procTable[i].ntraces);
		printf("Proc %d Num of Page Faults %d\n",i,procTable[i].numPageFault);
		printf("Proc %d Num of Page Hit %d\n",i,procTable[i].numPageHit);
		assert(procTable[i].numPageHit + procTable[i].numPageFault == procTable[i].ntraces);
	}

}
void oneLevelVMSim1() {
	int i;
	for(i=0; i < numProcess; i++)
		procTable[i].firstLevelPageTable = (struct pageTableEntry *) malloc(sizeof(struct pageTableEntry) * (1L <<(VIRTUALADDRBITS - PAGESIZEBITS))); 
	while(1){
		int count =0;
		if(count ==numProcess)
			break;

		for(i = 0; i < numProcess; i++) {
			unsigned addr;
			char rw;
			unsigned phy_addr;
			unsigned virtual_addr;
			unsigned offset;
			fscanf(procTable[i].tracefp,"%x  %c",&addr,&rw);
			if(!feof(procTable[i].tracefp))
			{	
				procTable[i].ntraces ++;				
				virtual_addr = (addr >> PAGESIZEBITS) ;  // 앞주소 
				offset = addr % PAGESIZEBITS ; // offset 주소 
				if(procTable[i].firstLevelPageTable[virtual_addr].valid == 1)  // hit 되서 주소 찾아가는 작업 하고/  맞는 프로세스 인지 검사  
				{
					procTable[i].numPageHit ++;
					struct phymem *tmp = procTable[i].firstLevelPageTable[virtual_addr].address;
					tmp->prev->next = tmp->next;
					tmp->next->prev = tmp->prev;
					tmp->next = &use;
					tmp->prev = use.prev;
					use.prev->next = tmp;
					use.prev = tmp;		
				}
				else    //fault 
				{	// valid queue 에서 하나 꺼내옴 !
					procTable[i].numPageFault ++;
					if( valid.next == &valid)
					{
						struct phymem *tmp2 = use.next;
						use.prev->next = tmp2;
						tmp2->prev = use.prev;
						use.prev = tmp2;
						//		use.next->next->prev = &use;
						use.next = use.next->next;
						tmp2->next = &use;
						use.next->prev = &use;
						int tmp = use.prev->pid;
						int j=0;
						j = use.prev->virtual;
						procTable[tmp].firstLevelPageTable[j].address  =NULL;
						procTable[tmp].firstLevelPageTable[j].valid =0;
					}
					else // 남은 메모리가 있을때 
					{	
						struct phymem *tmp = valid.next;

						use.prev->next  = valid.next;
						valid.next->prev = use.prev;
						use.prev = valid.next;
						valid.next = valid.next->next;
						tmp ->next = &use;
						valid.next->prev = &valid;
					}

					use.prev->virtual = virtual_addr;
					procTable[i].firstLevelPageTable[virtual_addr].address = use.prev;
					procTable[i].firstLevelPageTable[virtual_addr].address->pid = i;
					procTable[i].firstLevelPageTable[virtual_addr].valid = 1;					 
				}
				//		printf("%d\n",procTable[i].firstLevelPageTable[virtual_addr].address->index <<12+ offset);
			}
			else
				procTable[i].eof = 1;
		}
		for(i=0; i<numProcess;i++)
		{
			if(procTable[i].eof ==1)
				count ++;
		}
		if(count ==numProcess)
			break;
	}	
	for(i=0; i < numProcess; i++) {
		printf("**** %s *****\n",procTable[i].traceName);
		printf("Proc %d Num of traces %d\n",i,procTable[i].ntraces);
		printf("Proc %d Num of Page Faults %d\n",i,procTable[i].numPageFault);
		printf("Proc %d Num of Page Hit %d\n",i,procTable[i].numPageHit);
		assert(procTable[i].numPageHit + procTable[i].numPageFault == procTable[i].ntraces);
	}

}

void twoLevelVMSim(int firstLevelBits) {
	int i;
	for(i=0; i < numProcess; i++)
		procTable[i].firstLevelPageTable = (struct pageTableEntry *) malloc(sizeof(struct pageTableEntry) * (1L << firstLevelBits)); 
	unsigned secondLevelBits = VIRTUALADDRBITS-firstLevelBits-PAGESIZEBITS;

	while(1){
		int count =0;
		if(count ==numProcess)
			break;
		for(i = 0; i < numProcess; i++) {
			unsigned addr;
			char rw;
			unsigned phy_addr;
			unsigned virtual_addr;
			unsigned offset;
			fscanf(procTable[i].tracefp,"%x  %c",&addr,&rw);
			//		printf("%x\n",addr);
			if(!feof(procTable[i].tracefp))
			{	
				procTable[i].ntraces ++;				
				virtual_addr = addr >> (VIRTUALADDRBITS - firstLevelBits) ; 
				//		printf("%x\n",virtual_addr); // 앞주소 
				unsigned second_addr = ((addr % (1L<< (VIRTUALADDRBITS -firstLevelBits))) >> PAGESIZEBITS);
				offset = addr % PAGESIZEBITS ; // offset 주소 
				//		printf("%x\n",second_addr);

				if(procTable[i].firstLevelPageTable[virtual_addr].valid == 1)  // hit 되서 주소 찾아가는 작업 하고/  맞는 프로세스 인지 검사  
				{ // second page Table 까지 hit 일 경우. 
					if(procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].valid == 1)
					{
						procTable[i].numPageHit ++;
						struct phymem *tmp = procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].address;
						tmp->prev->next = tmp->next;
						tmp->next->prev = tmp->prev;
						tmp->next = &use;
						tmp->prev = use.prev;
						use.prev->next = tmp;
						use.prev = tmp;		
					}
					else	//second page table 
					{
						procTable[i].numPageFault ++; 
						if( valid.next == &valid)
						{   
							struct phymem *tmp2 = use.next;
							use.prev->next = tmp2;
							tmp2->prev = use.prev;
							use.prev = tmp2;
							use.next = use.next->next;
							tmp2->next = &use;
							use.next->prev = &use;
							int tmp = use.prev->pid;
							int j=0;
							int s;
							j = use.prev->virtual_fist;
							s = use.prev->virtual;
							procTable[tmp].firstLevelPageTable[j].secondPageTable[s].address  =NULL;
							procTable[tmp].firstLevelPageTable[j].secondPageTable[s].valid =0; 
						}   
						else // 남은 메모리가 있을때 
						{    
							struct phymem *tmp = valid.next;    
							use.prev->next  = valid.next;
							valid.next->prev = use.prev;
							use.prev = valid.next;
							valid.next = valid.next->next;
							tmp ->next = &use;
							valid.next->prev = &valid;
						}   
						use.prev ->virtual_fist = virtual_addr;
						use.prev->virtual= second_addr;
						procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].address = use.prev;
						procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].address->pid = i;
						procTable[i].firstLevelPageTable[virtual_addr].valid = 1;
						procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].valid = 1;   			
					}
				}
				else    //fist level 에서 fault 할당 해줘야댐  
				{	
					//			printf(" 할당\n ");
					procTable[i].firstLevelPageTable[virtual_addr].secondPageTable = (struct pageTableEntry *) malloc(sizeof(struct pageTableEntry)* (1L <<secondLevelBits)); 
					procTable[i].numPageFault ++;
					procTable[i].num2ndLevelPageTable++;
					if( valid.next == &valid)
					{
						struct phymem *tmp2 = use.next;
						use.prev->next = tmp2;
						tmp2->prev = use.prev;
						use.prev = tmp2;
						use.next = use.next->next;
						tmp2->next = &use;
						use.next->prev = &use;
						int tmp = use.prev->pid;
						int j=0;
						int s;
						j = use.prev->virtual_fist;
						s = use.prev->virtual;
						procTable[tmp].firstLevelPageTable[j].secondPageTable[s].address  =NULL;
						procTable[tmp].firstLevelPageTable[j].secondPageTable[s].valid =0;
					}
					else // 남은 메모리가 있을때 
					{	
						struct phymem *tmp = valid.next;						
						use.prev->next  = valid.next;
						valid.next->prev = use.prev;
						use.prev = valid.next;
						valid.next = valid.next->next;
						tmp ->next = &use;
						valid.next->prev = &valid;
					}
					use.prev->virtual_fist = virtual_addr;
					use.prev->virtual= second_addr;
					procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].address = use.prev;
					procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].address->pid = i;
					procTable[i].firstLevelPageTable[virtual_addr].valid = 1;
					procTable[i].firstLevelPageTable[virtual_addr].secondPageTable[second_addr].valid = 1;					 
				}
				//		printf("%d\n",procTable[i].firstLevelPageTable[virtual_addr].address->index <<12+ offset);
			}
			else
				procTable[i].eof = 1;
		}
		for(i=0; i<numProcess;i++)
		{
			if(procTable[i].eof ==1)
				count ++;
		}
		if(count ==numProcess)
			break;
	}	
	for(i=0; i < numProcess; i++) {
		printf("**** %s *****\n",procTable[i].traceName);
		printf("Proc %d Num of traces %d\n",i,procTable[i].ntraces);
		printf("Proc %d Num of second level page tables allocated %d\n",i,procTable[i].num2ndLevelPageTable);
		printf("Proc %d Num of Page Faults %d\n",i,procTable[i].numPageFault);
		printf("Proc %d Num of Page Hit %d\n",i,procTable[i].numPageHit);
		assert(procTable[i].numPageHit + procTable[i].numPageFault == procTable[i].ntraces);
	}
}

void invertedPageVMSim() {
	int i;
	hashTable = (struct pageTableEntry *) malloc(sizeof(struct pageTableEntry) * (nFrame)); 
	for(i=0;i<nFrame;i++)
		hashTable[i].link = NULL;
	hashTable[nFrame-1].link = NULL;


	while(1){
		int count =0;
		if(count ==numProcess)
			break;

		for(i = 0; i < numProcess; i++) {
			unsigned addr;
			char rw;
			unsigned phy_addr;
			unsigned virtual_addr;
			unsigned offset;
			fscanf(procTable[i].tracefp,"%x  %c",&addr,&rw);
			if(!feof(procTable[i].tracefp))
			{	

				procTable[i].ntraces ++;				
				virtual_addr = (addr >> PAGESIZEBITS) ;  // 앞주소 
				offset = addr % PAGESIZEBITS ; // offset 주소 
				unsigned hash = ((virtual_addr+i)%(nFrame));
		//		printf(" hash  : %x    virtual :  %x \n",hash,virtual_addr);
				if(hashTable[hash].link !=NULL)  // hit 되서 주소 찾아가는 작업 하고/  맞는 프로세스 인지 검사  
				{
				//	printf("여기\n");
					
					procTable[i].numIHTNonNULLAcess++;					
					int check = 0;
					struct pageTableEntry *current = hashTable[hash].link;
					while(current != NULL)
					{
						if((current->valid ==1)&&(current->virtual == virtual_addr))
						{
			//				printf("1\n");
							procTable[i].numPageHit ++;
							struct phymem *tmp = current->address;
							tmp->prev->next = tmp->next;
							tmp->next->prev = tmp->prev;
							tmp->next = &use;
							tmp->prev = use.prev;
							use.prev->next = tmp;
							use.prev = tmp;
							check =1;
							break;
						}
						current = current->next;	
					}
					if(check ==0)
					{
			//			printf("2\n");
						procTable[i].numPageFault++;
						struct pageTableEntry * node_tmp = node_queue.next;
						node_queue.next= node_queue.next->next;
					        node_tmp->next = hashTable[hash].link;	
						hashTable[hash].link = node_tmp; 
						if( valid.next == &valid)
						{   
							struct phymem *tmp2 = use.next;
							use.prev->next = tmp2;
							tmp2->prev = use.prev;
							use.prev = tmp2;
							use.next = use.next->next;
							tmp2->next = &use;
							use.next->prev = &use;
							int j = use.prev->node_index;
							node[j].address  =NULL;
							node[j].valid =0;
						}
						else
						{
							struct phymem *tmp = valid.next;
							use.prev->next  = valid.next;
							valid.next->prev = use.prev;
							use.prev = valid.next;
							valid.next = valid.next->next;
							tmp ->next = &use;
							valid.next->prev = &valid;
						}

						use.prev->virtual = virtual_addr;
						use.prev->node_index = node_tmp->node_index;
						node_tmp->address = use.prev;
						node_tmp->virtual = virtual_addr;
						node_tmp->pid = i;
						node_tmp->valid = 1;					
					}

				}
				else    //fault 
				{	// valid queue 에서 하나 꺼내옴 !
			//		printf("asd\n");
					procTable[i].numIHTNULLAccess++;
					procTable[i].numPageFault++;				
					struct pageTableEntry * node_tmp = node_queue.next;
					node_queue.next= node_queue.next->next;
					hashTable[hash].link = node_tmp;
					node_tmp->next = NULL;
					if( valid.next == &valid)
					{   
						struct phymem *tmp2 = use.next;
						use.prev->next = tmp2;
						tmp2->prev = use.prev;
						use.prev = tmp2;
						use.next = use.next->next;
						tmp2->next = &use;
						use.next->prev = &use;
					int 	j = use.prev->node_index;
						node[j].address =NULL;
						node[j].valid =0; 
					}   
					else 
					{    
						struct phymem *tmp = valid.next;
						use.prev->next  = valid.next;
						valid.next->prev = use.prev;
						use.prev = valid.next;
						valid.next = valid.next->next;
						tmp ->next = &use;
						valid.next->prev = &valid;
					}

					use.prev->virtual = virtual_addr;
					use.prev->node_index = node_tmp->node_index;
					node_tmp->address = use.prev;
					node_tmp->virtual = virtual_addr;
					node_tmp->pid = i;
					node_tmp->valid = 1;
				}
				//		printf("%d\n",procTable[i].firstLevelPageTable[virtual_addr].address->index <<12+ offset);
			}
			else
				procTable[i].eof = 1;
		}
		for(i=0; i<numProcess;i++)
		{
			if(procTable[i].eof ==1)
				count ++;
		}
		if(count ==numProcess)
			break;
	}	
	for(i=0; i < numProcess; i++) {
		printf("**** %s *****\n",procTable[i].traceName);
		printf("Proc %d Num of traces %d\n",i,procTable[i].ntraces);
		printf("Proc %d Num of Inverted Hash Table Access Conflicts %d\n",i,procTable[i].numIHTConflictAccess);
		printf("Proc %d Num of Empty Inverted Hash Table Access %d\n",i,procTable[i].numIHTNULLAccess);
		printf("Proc %d Num of Non-Empty Inverted Hash Table Access %d\n",i,procTable[i].numIHTNonNULLAcess);
		printf("Proc %d Num of Page Faults %d\n",i,procTable[i].numPageFault);
		printf("Proc %d Num of Page Hit %d\n",i,procTable[i].numPageHit);
		assert(procTable[i].numPageHit + procTable[i].numPageFault == procTable[i].ntraces);
		assert(procTable[i].numIHTNULLAccess + procTable[i].numIHTNonNULLAcess == procTable[i].ntraces);
	}
}

int main(int argc, char *argv[]) {
	int i,c, simType;

	// initialize procTable for Memory Simulations
	numProcess = argc-4;
	simType = atoi(argv[1]);
	int firstLevelBits = atoi(argv[2]);
	int phyMemSizeBits = atoi(argv[3]);
	nFrame = 1L << (phyMemSizeBits - PAGESIZEBITS);
	procTable = (struct procEntry *) malloc(sizeof(struct procEntry)* numProcess);  // 할당 
	phymemory = (struct phymem *) malloc(sizeof(struct phymem) * nFrame);	
	valid.next = &phymemory[0];
	valid.prev = &phymemory[nFrame-1];
	for(i=0;i<nFrame-1;i++)
	{
		phymemory[i].index = i;
		phymemory[i].next = &phymemory[i+1];
		phymemory[i+1].prev = &phymemory[i];
	}
	phymemory[nFrame-1].index = nFrame-1;
	phymemory[nFrame-1].next = &valid;
	phymemory[0].prev = &valid;
	// use 큐 초기화 
	use.next = &use;
	use.prev = &use;
	//valid queue 초기화 

	unsigned num = numProcess * (1L<<(VIRTUALADDRBITS - PAGESIZEBITS));
	node = (struct pageTableEntry *) malloc(sizeof(struct pageTableEntry) * (num));
	for(i = 0; i < num-1; i++) 
	{
		node[i+1].prev = &node[i];
		node[i].next = &node[i+1];
		node[i].node_index = i;
	}
	node[num-1].next = &node_queue;
	node[num-1].node_index = num-1;
	node_queue.next = &node[0];
	node[0].prev = &node_queue;
	node_queue.prev= &node[num-1];
	for(i = 0; i < numProcess; i++) {

		procTable[i].traceName = argv[i+optind+3];
		procTable[i].pid = i ; // pid 삽입 
		// opening a tracefile for the process
		printf("process %d opening %s\n",i,argv[i + optind + 3]);
		procTable[i].tracefp = fopen(argv[i + optind + 3],"r");
		if (procTable[i].tracefp == NULL) {
			printf("ERROR: can't open %s file; exiting...",argv[i+optind+3]);
			exit(1);
		}
	}
	//	파일 다 읽었음 

	printf("Num of Frames %d Physical Memory Size %ld bytes\n",nFrame, (1L<<phyMemSizeBits));

	if (simType == 0) {
		printf("=============================================================\n");
		printf("The One-Level Page Table with FIFO Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		oneLevelVMSim();
	}

	if (simType == 1) {
		printf("=============================================================\n");
		printf("The One-Level Page Table with LRU Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		oneLevelVMSim1();
	}

	if (simType == 2) {
		printf("=============================================================\n");
		printf("The Two-Level Page Table Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		twoLevelVMSim(firstLevelBits);
	}

	if (simType == 3) {
		printf("=============================================================\n");
		printf("The Inverted Page Table Memory Simulation Starts .....\n");
		printf("=============================================================\n");
		invertedPageVMSim();
	}

	for(i = 0; i < numProcess; i++)
		fclose(procTable[i].tracefp);	
	return(0);
}

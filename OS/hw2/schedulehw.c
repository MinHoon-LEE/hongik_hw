// CPU Schedule Simulator Homework
// Student Number : B611146
// Name : 이민훈 
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>

#define SEED 10

// process states
#define S_IDLE 0			
#define S_READY 1
#define S_BLOCKED 2
#define S_RUNNING 3
#define S_TERMINATE 4

int NPROC, NIOREQ, QUANTUM;

struct ioDoneEvent {
	int procid;  // 호출한 proccess 의 id 
	int doneTime;   // DoneTime = current time + ioServtime 
	int len; 
	struct ioDoneEvent *prev;
	struct ioDoneEvent *next;
} ioDoneEventQueue, *ioDoneEvent;

struct process {
	int id;
	int len;		// for queue
	int targetServiceTime;
	int serviceTime;
	int startTime;
	int endTime;
	char state;
	int priority;
	int saveReg0, saveReg1;
	struct process *prev;
	struct process *next;
} *procTable;

struct process idleProc;
struct process readyQueue;
struct process blockedQueue;
struct process *runningProc;

int cpuReg0, cpuReg1;
int currentTime = 0;
int *procIntArrTime, *procServTime, *ioReqIntArrTime, *ioServTime;

void compute() {
	// DO NOT CHANGE THIS FUNCTION
	cpuReg0 = cpuReg0 + runningProc->id;
	cpuReg1 = cpuReg1 + runningProc->id;
	//printf("In computer proc %d cpuReg0 %d\n",runningProc->id,cpuReg0);
}

void initProcTable() {
	int i;	for(i=0; i < NPROC; i++) {
		procTable[i].id = i;
		procTable[i].len = 0;
		procTable[i].targetServiceTime = procServTime[i];
		procTable[i].serviceTime = 0;
		procTable[i].startTime = 0;
		procTable[i].endTime = 0;
		procTable[i].state = S_IDLE;
		procTable[i].priority = 0;
		procTable[i].saveReg0 = 0;
		procTable[i].saveReg1 = 0;
		procTable[i].prev = NULL;
		procTable[i].next = NULL;
	}
	idleProc.id = -1;
}

struct ioDoneEvent *current;
struct ioDoneEvent *sibal;
struct proccess *currProc;

void procExecSim(struct process *(*scheduler)()) {
	int pid, qTime=0, cpuUseTime = 0, nproc=0, termProc = 0, nioreq=0;
	char schedule = 0, nextState = S_IDLE;
	int nextForkTime, nextIOReqTime;
	int flg =0;
	int i;
//	for( i=0;i<NPROC;i++)
//	printf(" %d\n",procIntArrTime[i]);
	int case1 = 0;
//	runningProc = idleProc;  // 처음 값 
	runningProc = &idleProc;
		
	nextForkTime = procIntArrTime[nproc];
	nextIOReqTime = ioReqIntArrTime[nioreq];
// 	schelduler Flag 만들어서 마지막에 scheduler를 부를지 정한다
	int Sch_Flag =0;
// 	N 값이랑 io 판별해주는 I 값 만들어 주기 
	int N =0 , I = 0;
	int R_Flag =0;
	int T_Flag =0;
	while(1) {
		flg =0; Sch_Flag =0; R_Flag =0; T_Flag=0;
		if(nproc ==NPROC)
			break;
		currentTime++;  // 현재 시각 
		qTime++; // 퀀텀 시간 이랑 비교 하는 변수 
		runningProc->serviceTime++;
		if (runningProc != &idleProc ) cpuUseTime++;
	//	printf("%d processing Proc %d servT %d targetServT %d nproc %d cpuUseTime %d qTime %d\n ", currentTime, runningProc->id,runningProc->serviceTime,runningProc->targetServiceTime,nproc,cpuUseTime,qTime);
		// 현재 idle  상태가 아니라면 usetime 증가 

		// MUST CALL compute() Inside While loop
		cpuReg0 = runningProc->saveReg0;
     		cpuReg1 = runningProc->saveReg1;
		compute(); 
		runningProc->saveReg0 = cpuReg0;
		runningProc->saveReg1 = cpuReg1;
//		printf(" next fork time : %d\n ",nextForkTime);	
		if (currentTime == nextForkTime) {
		 /* CASE 2 : a new process created */
		// N 값을 만들어서 NPROC 보다 작은 때 까지만 더해 준다 . if 문 하나 더 만들기   ( IOrequest 도 마찬가지 ) 
			if(N<NPROC)
			{
		// qTime =0 초기화 
		// 원래 있던 proccess 	-> readyQueue 에 들어가고 	

				 case1 =1;
				procTable[N].next = &readyQueue;
				procTable[N].prev = readyQueue.prev;
				(readyQueue.prev)->next = &procTable[N];
				readyQueue.prev = &procTable[N];
				procTable[N].startTime = currentTime;
				readyQueue.len ++;
//				printf(" targetservice time  :%d\n ",readyQueue.prev->targetServiceTime);
				procTable[N].startTime = currentTime;
				if(runningProc->state == S_RUNNING)
				{ 
					flg =1 ;
		/*			runningProc->next = &readyQueue;
					runningProc->prev = readyQueue.prev;
					(readyQueue.prev)->next =runningProc;
					readyQueue.prev = runningProc;
					runningProc->state = S_READY;*/
		// proctable[N].starttime = current time 
		// nextForkTime = procIntArrTIme++
//					printf(" next fork time : %d \n" ,nextForkTime);
				//	 R_Flag =1; T_Flag =1 ;
				}
				N++;
				nextForkTime += procIntArrTime[N];
		//		printf("next fork timee %d \n ",nextForkTime); 
//				printf(" ------%d ----- \n",N-1);
				Sch_Flag = 1;
			}
		}
		if (qTime == QUANTUM ) { /* CASE 1 : The quantum expires */
		//	if( -> ruuning porcces state 가 'R' (running) 일때  )
		//	printf(" ----------퀀텀 ------------\n");
			Sch_Flag =1; case1 =2;
			if(runningProc->state= S_RUNNING)
			{
				runningProc->priority--;
				 flg =1 ;
		/*		runningProc->next = &readyQueue;
				runningProc->prev = readyQueue.prev;
				(readyQueue.prev)->next = runningProc;
				readyQueue.prev = runningProc;
				runningProc->state = S_READY;
				Sch_Flag =1; R_Flag =1; T_Flag =1 ;
				readyQueue.len ++;*/
			}	
		}

		/*	if(flg ==1)
      			{	runningProc->next = &readyQueue;
                                runningProc->prev = readyQueue.prev;
                                (readyQueue.prev)->next = runningProc;
                                readyQueue.prev = runningProc;
                                runningProc->state = S_READY;
                                Sch_Flag =1; R_Flag =1; T_Flag =1 ;
			flg =0;	}*/
		
		// qTime =0 초기화 
		// case 2번 과 동일하게 
		// 퀀텀이 만료됨가 동시에 완료 처리 / + iorequest ? 한다면 ? 
		//
		
		current = ioDoneEventQueue.next; 
		while (current != &ioDoneEventQueue) 
		{
	//		printf(" < %d  > -> ",current->doneTime);
			/* CASE 3 : IO Done Event */ // nioreq 증가 ? 
			if( current->doneTime == currentTime)
			{
		//		printf(" state : %d",procTable[current->procid].state);	
				if(procTable[current->procid].state !=S_TERMINATE)
				{
				procTable[current->procid].prev->next = procTable[current->procid].next;
				procTable[current->procid].next->prev = procTable[current->procid].prev;

				case1 = 3;
				procTable[current->procid].prev = readyQueue.prev;
				readyQueue.prev->next = &procTable[current->procid];
				readyQueue.prev = &procTable[current->procid];
				procTable[current->procid].next = &readyQueue;
				readyQueue.len ++;
				procTable[current->procid].state = S_READY;
				


				current->prev->next = current->next;
				current->next->prev = current->prev;
			/*		currProc = blockedQueue.next; 
				while(currProc == &blockedQueue)
				{
					if(current->procid == currProc->id)
					{
						currProc->prev->next = currProc->next;
						currProc->next->prev = currProc->prev;
						currProc->next = readyQueue; 
						currProc->prev = readyQueue.prev;
						readyQueue.prev->next = currProc;
						readyQueue.prev = currProc;		
					}
				}
				
		*/	
		//		printf(" iodone \n");	
				flg =1;
				Sch_Flag =1;
				nioreq++;
//				printf( " 만료된 io id  ===== %d\n",current->procid);
//				printf(" io 끝난 갯수. -- : %d\n " ,nioreq);
					}
					
				flg =1;
				}
				
			current = current->next;
			} 
//printf("\n");

			if(flg ==1)
                        {      
				if(runningProc != &idleProc){ 
				runningProc->next = &readyQueue;
                                runningProc->prev = readyQueue.prev;
                                (readyQueue.prev)->next = runningProc;
                                readyQueue.prev = runningProc;
                                runningProc->state = S_READY;
                                Sch_Flag =1; R_Flag =1; T_Flag =1 ;
                        flg =0;
//			printf(" 여기다 ! "); 
//			printf( " iodoneTime  = %d\n",ioDoneEvent[I].doneTime);
			readyQueue.len ++;}
			}
		
		// IoDone event 에 있는거 NULL 까지 검사하고 prev / next  이어 주기 
		sibal  = ioDoneEventQueue.next;

		// Block queue 에 있는거 Ready queue 로 
		if (cpuUseTime == nextIOReqTime) { /* CASE 5: reqest IO operations (only when the process does not terminate) */
			if(I<NIOREQ)
			{
	
				if( runningProc->state == S_READY)
				{
					runningProc->prev->next = runningProc->next;
					runningProc->next->prev = runningProc->prev;
					T_Flag =0;
					readyQueue.len --;
				}
//				printf(" case 4");
				runningProc->state = S_BLOCKED;
			 	ioDoneEvent[I].procid = runningProc->id;
//				printf(" \n\n\n  %d id \n\n\n",runningProc->id);	
				ioDoneEvent[I].doneTime = currentTime + ioServTime[I];
		//		printf("=========================== iodone time %d\n\n",ioDoneEvent[I].doneTime);
		/*		ioDoneEvent[I].next = &ioDoneEventQueue;
				ioDoneEvent[I].prev = ioDoneEventQueue.prev;
				ioDoneEventQueue.prev->next = &ioDoneEvent[I];
				ioDoneEventQueue.prev = &ioDoneEvent[I];
			*/
				if(qTime != QUANTUM)
				runningProc->priority++;

				while(1)
				{
					if(ioDoneEvent[I].doneTime>=sibal->doneTime)
						sibal = sibal->next;
					else
						break;
				} 
				sibal->prev->next = &ioDoneEvent[I];
				ioDoneEvent[I].next = sibal;
				ioDoneEvent[I].prev = sibal->prev;
				sibal->prev = &ioDoneEvent[I];

	
				runningProc->next = &blockedQueue;
				runningProc->prev = blockedQueue.prev;
				(blockedQueue.prev)->next = runningProc;
				blockedQueue.prev =runningProc;
				//ready 큐에 들어가 있는경우 ?
				case1 = 4;
				Sch_Flag =1;R_Flag = 1; 
				I++;
				nextIOReqTime +=  ioReqIntArrTime[I];
			
// 	schelduler Flag 만들어서 마지막에 scheduler를 부를지 정한다
			}	
		// Block queue 에 보내고 
		// qTime 초기화 
		}
	//	printf(" cpu use time ---- %d\n",cpuUseTime);
	//	printf(" io done time ----- %d\n",ioDoneEvent[I-1].doneTime);
	//	printf( " io ---------- %d \n",I);
		// proccess.state -> Block 
//		printf("<%d><%d>",currentTime,cpuUseTime);
//		if(runningProc == &idleProc)
//			printf( "< idle proc >");
//		else
//			printf(" < proc>");
//		printf(" //    %d    //",runningProc->id);
//		printf( " %d : service time  // %d : target service time \n",runningProc->serviceTime,runningProc->targetServiceTime);
		if (runningProc->serviceTime == runningProc->targetServiceTime) { /* CASE 4 : the process job done and terminates */
		// proccess state -> Terminate
		// nproc ++ ? 
			nproc++;	
		// runningprocces.endtime = current time 
			runningProc->endTime = currentTime;
			if(runningProc->state == S_READY)
			{
				runningProc->prev->next = runningProc->next;
				runningProc->next->prev = runningProc->prev;
				R_Flag =0;
			}
//			printf("--------------- nproc  =  %d\n ",nproc);
		case1 =5;
			runningProc->state = S_TERMINATE; 	
		// nproc ++ ? 
	//	printf("case 5");
		Sch_Flag =1 ;
		}
		// call scheduler() if needed
		// Flag 값 보고 scheduler() 호출 하기 
//		printf(" sch_Flag %d",Sch_Flag);
		if(runningProc->state != S_RUNNING)
		{
			Sch_Flag =0;
			qTime =0;
	//		printf( " \n\n case : %d\n\n ",case1);
			runningProc = scheduler();
	//		printf(" %d ready큐 안에 갯수\n ",readyQueue.len);
			if(runningProc != &idleProc)
			runningProc->state = S_RUNNING;
	//		printf(" target service time ( schedule ) %d \n",runningProc->targetServiceTime);
		}
//		printf("queuenext id : %d\n",readyQueue.next->id);
//		printf("ruuningProcess : %d\n",runningProc->id);
	//	if( nproc == NPROC )
	//		break;
	} // while loop
}

//RR,SJF(Modified),SRTN,Guaran
//teed Scheduling(modified),Simple Feed Back Scheduling
struct process* RRschedule() {	
	struct process *schedule;
	schedule = readyQueue.next;
	readyQueue.next->next->prev = &readyQueue;
	readyQueue.next = readyQueue.next->next;
	
//	if((readyQueue.next == &readyQueue) && (readyQueue.prev == &readyQueue))
	if(schedule == &readyQueue)
		return &idleProc; 
	else
	{ readyQueue.len --;	return schedule;}
}
struct process* SJFschedule() {
	struct process *schedule;
	schedule = readyQueue.next;
	if(schedule == &readyQueue)
                return &idleProc;
	int min = schedule->targetServiceTime;


	while(schedule != &readyQueue)
	{
		if(min > schedule->targetServiceTime)
			min = schedule->targetServiceTime;
		schedule = schedule->next;
	}
	schedule =readyQueue.next;
	while(schedule != &readyQueue)
	{
		if( schedule->targetServiceTime == min)
		{
		      	schedule->prev->next = schedule->next;
       			schedule->next->prev  = schedule->prev;
			return schedule;
		}
		schedule = schedule->next;
	}
}
struct process* SRTNschedule() {
	struct process *schedule;
        schedule = readyQueue.next;
        if(schedule == &readyQueue)
                return &idleProc;
        int min = (schedule->targetServiceTime - schedule->serviceTime);

        while(schedule != &readyQueue)
        {
                if(min > (schedule->targetServiceTime - schedule->serviceTime))
                        min = (schedule->targetServiceTime - schedule->serviceTime);
                schedule = schedule->next;
        }
        schedule =readyQueue.next;
        while(schedule != &readyQueue)
        {
                if( (schedule->targetServiceTime - schedule->serviceTime) == min)
                {
                        schedule->prev->next = schedule->next;
                        schedule->next->prev  = schedule->prev;
                        return schedule;
                }
                schedule = schedule->next;
        }
}
struct process* GSschedule() {
	struct process *schedule;
        schedule = readyQueue.next;
        if(schedule == &readyQueue)
                return &idleProc;
         double min = ((double)schedule->serviceTime/ (double)schedule->targetServiceTime);

        while(schedule != &readyQueue)
        {
                if(min > ((double)schedule->serviceTime / (double)schedule->targetServiceTime))
                        min = ((double)schedule->serviceTime / (double)schedule->targetServiceTime);
                schedule = schedule->next;
        }
        schedule =readyQueue.next;
        while(schedule != &readyQueue)
        {
                if( ((double)schedule->serviceTime / (double)schedule->targetServiceTime) == min )
                {
                        schedule->prev->next = schedule->next;
                        schedule->next->prev  = schedule->prev;
                        return schedule;
                }
                schedule = schedule->next;
        }	
}
struct process* SFSschedule() {
	 struct process *schedule;
        schedule = readyQueue.next;
        if(schedule == &readyQueue)
                return &idleProc;
        int max = schedule->priority;

        while(schedule != &readyQueue)
        {
                if(max < schedule->priority)
                        max = schedule->priority;
                schedule = schedule->next;
        }
        schedule =readyQueue.next;
        while(schedule != &readyQueue)
        {
                if( schedule->priority == max)
                {
                        schedule->prev->next = schedule->next;
                        schedule->next->prev  = schedule->prev;
                        return schedule;
                }
                schedule = schedule->next;
        }
}

void printResult() {
	// DO NOT CHANGE THIS FUNCTION
	int i;
	long totalProcIntArrTime=0,totalProcServTime=0,totalIOReqIntArrTime=0,totalIOServTime=0;
	long totalWallTime=0, totalRegValue=0;
	for(i=0; i < NPROC; i++) {
		totalWallTime += procTable[i].endTime - procTable[i].startTime;
		/*
		printf("proc %d serviceTime %d targetServiceTime %d saveReg0 %d\n",
			i,procTable[i].serviceTime,procTable[i].targetServiceTime, procTable[i].saveReg0);
		*/
		totalRegValue += procTable[i].saveReg0+procTable[i].saveReg1;
		/* printf("reg0 %d reg1 %d totalRegValue %d\n",procTable[i].saveReg0,procTable[i].saveReg1,totalRegValue);*/
	}
	for(i = 0; i < NPROC; i++ ) { 
		totalProcIntArrTime += procIntArrTime[i];
		totalProcServTime += procServTime[i];
	}
	for(i = 0; i < NIOREQ; i++ ) { 
		totalIOReqIntArrTime += ioReqIntArrTime[i];
		totalIOServTime += ioServTime[i];
	}
	
	printf("Avg Proc Inter Arrival Time : %g \tAverage Proc Service Time : %g\n", (float) totalProcIntArrTime/NPROC, (float) totalProcServTime/NPROC);
	printf("Avg IOReq Inter Arrival Time : %g \tAverage IOReq Service Time : %g\n", (float) totalIOReqIntArrTime/NIOREQ, (float) totalIOServTime/NIOREQ);
	printf("%d Process processed with %d IO requests\n", NPROC,NIOREQ);
	printf("Average Wall Clock Service Time : %g \tAverage Two Register Sum Value %g\n", (float) totalWallTime/NPROC, (float) totalRegValue/NPROC);
	
}


int main(int argc, char *argv[]) {
	// DO NOT CHANGE THIS FUNCTION
	int i;
	int totalProcServTime = 0, ioReqAvgIntArrTime;
	int SCHEDULING_METHOD, MIN_INT_ARRTIME, MAX_INT_ARRTIME, MIN_SERVTIME, MAX_SERVTIME, MIN_IO_SERVTIME, MAX_IO_SERVTIME, MIN_IOREQ_INT_ARRTIME;
	
	if (argc < 12) {
		printf("%s: SCHEDULING_METHOD NPROC NIOREQ QUANTUM MIN_INT_ARRTIME MAX_INT_ARRTIME MIN_SERVTIME MAX_SERVTIME MIN_IO_SERVTIME MAX_IO_SERVTIME MIN_IOREQ_INT_ARRTIME\n",argv[0]);
		exit(1);
	}
	
	SCHEDULING_METHOD = atoi(argv[1]);
	NPROC = atoi(argv[2]);
	NIOREQ = atoi(argv[3]);
	QUANTUM = atoi(argv[4]);
	MIN_INT_ARRTIME = atoi(argv[5]);
	MAX_INT_ARRTIME = atoi(argv[6]);
	MIN_SERVTIME = atoi(argv[7]);
	MAX_SERVTIME = atoi(argv[8]);
	MIN_IO_SERVTIME = atoi(argv[9]);
	MAX_IO_SERVTIME = atoi(argv[10]);
	MIN_IOREQ_INT_ARRTIME = atoi(argv[11]);
	
	printf("SIMULATION PARAMETERS : SCHEDULING_METHOD %d NPROC %d NIOREQ %d QUANTUM %d \n", SCHEDULING_METHOD, NPROC, NIOREQ, QUANTUM);
	printf("MIN_INT_ARRTIME %d MAX_INT_ARRTIME %d MIN_SERVTIME %d MAX_SERVTIME %d\n", MIN_INT_ARRTIME, MAX_INT_ARRTIME, MIN_SERVTIME, MAX_SERVTIME);
	printf("MIN_IO_SERVTIME %d MAX_IO_SERVTIME %d MIN_IOREQ_INT_ARRTIME %d\n", MIN_IO_SERVTIME, MAX_IO_SERVTIME, MIN_IOREQ_INT_ARRTIME);
	
	srandom(SEED);
	
	// allocate array structures
	procTable = (struct process *) malloc(sizeof(struct process) * NPROC);
	ioDoneEvent = (struct ioDoneEvent *) malloc(sizeof(struct ioDoneEvent) * NIOREQ);
	procIntArrTime = (int *) malloc(sizeof(int) * NPROC);
	procServTime = (int *) malloc(sizeof(int) * NPROC);
	ioReqIntArrTime = (int *) malloc(sizeof(int) * NIOREQ);
	ioServTime = (int *) malloc(sizeof(int) * NIOREQ);

	// initialize queues
	readyQueue.next = readyQueue.prev = &readyQueue;
	
	blockedQueue.next = blockedQueue.prev = &blockedQueue;
	ioDoneEventQueue.next = ioDoneEventQueue.prev = &ioDoneEventQueue;
	ioDoneEventQueue.doneTime = INT_MAX;
	ioDoneEventQueue.procid = -1;
	ioDoneEventQueue.len  = readyQueue.len = blockedQueue.len = 0;
	
	// generate process interarrival times
	for(i = 0; i < NPROC; i++ ) { 
		procIntArrTime[i] = random()%(MAX_INT_ARRTIME - MIN_INT_ARRTIME+1) + MIN_INT_ARRTIME;
	}
	
	// assign service time for each process	
	for(i=0; i < NPROC; i++) {
		procServTime[i] = random()% (MAX_SERVTIME - MIN_SERVTIME + 1) + MIN_SERVTIME;
		totalProcServTime += procServTime[i];	
	}
	
	ioReqAvgIntArrTime = totalProcServTime/(NIOREQ+1);
	
	// generate io request interarrival time
	for(i = 0; i < NIOREQ; i++ ) { 
		ioReqIntArrTime[i] = random()%((ioReqAvgIntArrTime - MIN_IOREQ_INT_ARRTIME)*2+1) + MIN_IOREQ_INT_ARRTIME;
	}
	
	// generate io request service time
	for(i = 0; i < NIOREQ; i++ ) { 
		ioServTime[i] = random()%(MAX_IO_SERVTIME - MIN_IO_SERVTIME+1) + MIN_IO_SERVTIME;
	}
	
#ifdef DEBUG
	// printing process interarrival time and service time
	printf("Process Interarrival Time :\n");
	for(i = 0; i < NPROC; i++ ) { 
		printf("%d ",procIntArrTime[i]);
	}
	printf("\n");
	printf("Process Target Service Time :\n");
	for(i = 0; i < NPROC; i++ ) { 
		printf("%d ",procTable[i].targetServiceTime);
	}
	printf("\n");
#endif
	
	// printing io request interarrival time and io request service time
	printf("IO Req Average InterArrival Time %d\n", ioReqAvgIntArrTime);
	printf("IO Req InterArrival Time range : %d ~ %d\n",MIN_IOREQ_INT_ARRTIME,
			(ioReqAvgIntArrTime - MIN_IOREQ_INT_ARRTIME)*2+ MIN_IOREQ_INT_ARRTIME);
			
#ifdef DEBUG		
	printf("IO Req Interarrival Time :\n");
	for(i = 0; i < NIOREQ; i++ ) { 
		printf("%d ",ioReqIntArrTime[i]);
	}
	printf("\n");
	printf("IO Req Service Time :\n");
	for(i = 0; i < NIOREQ; i++ ) { 
		printf("%d ",ioServTime[i]);
	}
	printf("\n");
#endif
	
	struct process* (*schFunc)();
	switch(SCHEDULING_METHOD) {
		case 1 : schFunc = RRschedule; break;
		case 2 : schFunc = SJFschedule; break;
		case 3 : schFunc = SRTNschedule; break;
		case 4 : schFunc = GSschedule; break;
		case 5 : schFunc = SFSschedule; break;
		default : printf("ERROR : Unknown Scheduling Method\n"); exit(1);
	}
	initProcTable();
	procExecSim(schFunc);
	printResult();
	
}

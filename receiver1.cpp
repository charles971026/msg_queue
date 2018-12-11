#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
using namespace std;



int main(){
	//create a message queue
	int qid = msgget(ftok(".",'u'), IPC_EXCL|IPC_CREAT|0600);
	
	//create message buffer
	struct buf {
		long mtype; 
		char message[50];
	};
	
	buf msg;
	
	//size of the message
	int size = sizeof(msg)-sizeof(long); 
	
	//termiantion flag
	string cmpr = "-1"; 
	
	//string to distinguish if the received message is from 997 sender
	string sender997 = "997sender"; 
	
	//counter for termination; when it reaches 2, program terminates
	int termination = 0; 
	
	
	while(termination != 2){
		
		//receive a message with mtype 111 
		msgrcv(qid, (struct msgbuf *)&msg, size, 111, 0);
		
		//check if it's the termination flag
		if(strcmp(msg.message, cmpr.c_str()) == 0){
			termination++;
			continue;
		}
		
		//print out the received message
		cout << msg.message << endl;
		cout << endl;
		
		//check if the received message is from 997 sender
		string message(msg.message);
		if(message.find(sender997,13) != string::npos){
			//send ack message to 997sender
			msg.mtype = 997;
			strcpy(msg.message, "From the first receiver: Event was received");
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);
		}
			
	}
	
	//terminate the program
	cout << "terminating receiver 1" <<  endl;
	msgctl (qid, IPC_RMID, NULL);
	exit(0);
	
	
}




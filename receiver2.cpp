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

	//get the message queue id
	int qid = msgget(ftok(".",'u'), 0);
	
	//counter for message
	int counter = 0;
	
	//create message buffer
	struct buf {
		long mtype; 
		char message[50];
	};
	
	buf msg;
	
	//string to distinguish if the received message is from 997 sender
	string sender997 = "997sender";
	
	//size of message
	int size = sizeof(msg)-sizeof(long);
	
	//counter for start; when it reaches 2, the program starts
	int start = 0;
	
	//receive starting flag from the senders
	msgrcv(qid, (struct msgbuf *)&msg, size, 222, 0);
	start++;
	msgrcv(qid, (struct msgbuf *)&msg, size, 222, 0);
	start++;
	
	//send message to both senders to start sending messages
	msg.mtype = 997;
	strcpy(msg.message, "start");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	
	msg.mtype = 257;
	strcpy(msg.message, "start");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	

	while(counter < 5000 && start == 2){
		//receive a message from senders
		msgrcv(qid, (struct msgbuf *)&msg, size, 222, 0);
		
		//print out the message
		cout << msg.message << endl;
		
		//check if the received message is from 997 sender
		string message(msg.message);
		if(message.find(sender997,13) != string::npos){
			//send ack message to 997 sender
			msg.mtype = 997;
			strcpy(msg.message, "From the second receiver: Event was received");
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);
		}
		
		//increase the counter
		counter++;
		cout << "counter: " << counter << endl;
		cout << endl;
		
		
	}
	
	//send termination flag to 997 sender and 257 sender
	msg.mtype = 997;
	strcpy(msg.message, "-1");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	
	msg.mtype = 257;
	strcpy(msg.message, "-1");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	
	//terminate the program
	cout << "terminating receiver 2" << endl;
	exit(0);
	
	
}

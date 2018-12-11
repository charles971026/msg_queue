#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <random>

using namespace std;

int main(){
	//get the message queue id
	int qid = msgget(ftok(".",'u'), 0);
	
	//create message buffer
	struct buf {
		long mtype; // required
		char message[50]; // mesg content
	};
	buf msg;
	
	//size of message
	int size = sizeof(msg)-sizeof(long);
	
	//generate a random number from 0 to 2^32-1
	default_random_engine generator(time(0));
	uniform_int_distribution<unsigned long int> distribution(0,pow(2,32) -1 );
	unsigned long int random_number = distribution(generator);
	
	//send receiver 2 that the program started
	msg.mtype = 222;
	strcpy(msg.message, "257 start");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	
	//receive a message from receiver 2 that informs 257 sender to start sending messages
	msgrcv(qid, (struct msgbuf *)&msg, size, 257, 0);
	
	//string to distinguish the termination flag
	string cmpr = "-1";
	
	while(true){
		if(random_number%257 == 0){
			
			//send message to receiver 2
			msg.mtype = 222;
			string src = "Message from 257sender: \nPID - " + to_string(getpid()) + "\n" + "Value - " + to_string(random_number);  
			strcpy(msg.message, src.c_str());
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);
			
			//send message to itself
			msg.mtype = 257;
			strcpy(msg.message, "to 257 itself");
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);
			
			//receive termination flag
			msgrcv(qid, (struct msgbuf *)&msg, size, 257, 0);
			if(strcmp(msg.message, cmpr.c_str()) == 0){
				break;
			}
		}
		//regenerate the random number
		random_number = distribution(generator);
	}
	
	//terminate 257 sender 
	cout << "terminating 257sender" << endl;
	exit(0);
}

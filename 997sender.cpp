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
	
	//string to distinguish the termination flag
	string cmpr = "-1";
	
	//keep track of whether receiver 2 is on or off
	bool is_receiver2_on = true;
	
	//send receiver 2 that the program started
	msg.mtype = 222;
	strcpy(msg.message, "997 start");
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	
	//receive a message from receiver 2 that informs 997 sender to start sending messages
	msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0);
	
	//stop sending message if the random number happens to be less than 100
	while(random_number >= 100){
		if(random_number%997 == 0){
			
			//send message to receiver 1
			msg.mtype = 111;
			string src = "Message from 997sender: \nPID - " + to_string(getpid()) + "\n" + "Value - " + to_string(random_number);  
			strcpy(msg.message, src.c_str());
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);
			
			//receive ack message
			msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0);
			
			//print the ack message
			cout <<msg.message << endl;
			cout << endl;
			
			if(is_receiver2_on){
				//send message to receiver 2 
				msg.mtype = 222;
				src = "Message from 997sender: \nPID - " + to_string(getpid()) + "\n" + "Value - " + to_string(random_number);
				strcpy(msg.message, src.c_str());
				msgsnd(qid, (struct msgbuf *)&msg, size, 0);
				
				//receive ack message
				msgrcv(qid, (struct msgbuf *)&msg, size, 997, 0);
				
				//if ack message is termination flag
				if(strcmp(msg.message, cmpr.c_str()) == 0){
					is_receiver2_on = false;
					continue;
				}
				//print the ack message
				cout << msg.message << endl;
				cout << endl;
			} 
			

		}
		//regenerate the random number
		random_number = distribution(generator);
	}
	//print the random number less than 100
	cout << random_number << endl;
	cout << endl;
	
	//send termination flag to receiver 1
	msg.mtype = 111;
	string src = "-1";
	strcpy(msg.message, src.c_str());
	msgsnd(qid, (struct msgbuf *)&msg, size, 0);
	
	//terminate the 997 sender
	cout << "termianting 997 sender" << endl; 
	exit(0);
}

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
#include <signal.h>

using namespace std;

//declare get_info function
void get_info(int qid, msgbuf *exitmsg, int size, long mtype);

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
	
	//send termination flag when 251 sender is killed
	strcpy(msg.message, "-1");
	get_info(qid, (struct msgbuf *)&msg, size, 111);
	

	while(true){
		if(random_number%251 == 0){
		
			//send message to receiver 1
			msg.mtype = 111;
			string src = "Message from 251 sender: \nPID - " + to_string(getpid()) + "\n" + "Value - " + to_string(random_number); 
			strcpy(msg.message, src.c_str());
			msgsnd(qid, (struct msgbuf *)&msg, size, 0);
			
		}
		//generate the random number
		random_number = distribution(generator);
	}
	
	
	//terminate the 251 sender
	cout << "terminating 251 sender" << endl;
	exit(0);
}

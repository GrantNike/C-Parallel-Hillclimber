/*
Grant Nike
6349302
gn17az
Nov 21
LabEx9
*/

//Optimizing Egg Holder funtion using Hill-Climber threads 
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <pthread.h>
#include <cmath>

int NUMTHREADS;
volatile bool continuing;
volatile int occupied;
//Best Answer so far, includes lowest minimum, and the x and y associated with it
volatile double lowestMin = 10000000.0;
volatile double bestX;
volatile double bestY;
pthread_mutex_t lock; //Our mutual exclusion lock

double eggHolder(double x, double y){
	return (-(y+47)*std::sin(std::sqrt(std::abs((x/2)+y+47)))-(x*std::sin(std::sqrt(std::abs(x-y-47)))));
}

//At this level, this header signature can be ignored
void* hillClimber(void* unnecessary) {
	srand((int)time(0));
	//X and Y start as random numbers between -512 and 512
	double X =  (double)((rand()%1024)+(-512));
	double Y = (double)((rand()%1024)+(-512));
	double Pos;
	
	while (continuing) {
		srand((int)time(0));
		Pos = eggHolder(X,Y);
		//If thread's current position is lower than the lowest Min, update the lowest Min
		pthread_mutex_lock(&lock);
		if(Pos < lowestMin){
			lowestMin = Pos;
			bestX = X;
			bestY = Y;
		}
		pthread_mutex_unlock(&lock);
		//Generates random modifiers for X and Y, 
		//making sure they are in the bounds of (-512,512)
		double X1;
		double Y1;
		do{
			X1 = X + (( (double)( (rand()%100)+(-50) ) )/10.0);
			Y1 = Y + (( (double)( (rand()%100)+(-50) ) )/10.0);
		}while(X1>512 || X1<-512 || Y1>512 || Y1< -512);
		
		double X2;
		double Y2;
		do{
			X2 = X + (( (double)( (rand()%100)+(-50) ) )/10.0);
			Y2 = Y + (( (double)( (rand()%100)+(-50) ) )/10.0);
		
		}while(X2>512 || X2<-512 || Y2>512 || Y2< -512);
		
		double X3;
		double Y3;
		do{
			X3 = X + (( (double)( (rand()%100)+(-50) ) )/10.0);
			Y3 = Y + (( (double)( (rand()%100)+(-50) ) )/10.0);
		}while(X3>512 || X3<-512 || Y3>512 || Y3< -512);
		
		double X4;
		double Y4;
		do{
			X4 = X + (( (double)( (rand()%100)+(-50) ) )/10.0);
			Y4 = Y + (( (double)( (rand()%100)+(-50) ) )/10.0);
		}while(X4>512 || X4<-512 || Y4>512 || Y4< -512);
		
		//Sets the values of bestX and bestY to the values that best minimize the eggHolder function so far
		double newX = X1;
		double newY = Y1;
		if(eggHolder(newX,newY)>eggHolder(X2,Y2)){
			newX = X2;
			newY = Y2;
		}
		
		if(eggHolder(newX,newY)>eggHolder(X3,Y3)){
			newX = X3;
			newY = Y3;
		}
		
		if(eggHolder(newX,newY)>eggHolder(X4,Y4)){
			newX = X4;
			newY = Y4;
		}
		
		//If the best new position is lower than the current position, move to that position 
		if(eggHolder(newX,newY)<Pos){
			X = newX;
			Y = newY;
		}
		//Else move Climber to random position and start over
		else{
			srand((int)time(0));
			X =  (double)((rand()%1024)+(-512));
			Y = (double)((rand()%1024)+(-512));
		}
	}
	pthread_mutex_lock(&lock);
	occupied--;
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
}

//Climbers only stop when a SIGNT signal is recieved 
void interrupted(int sig){
	continuing=false;
	std::cout<<"\nComputations complete.\nHalting now..."<<std::endl<<std::endl;
	std::cout<<"Current Global Best Height is: f(x,y) = "<<lowestMin<<std::endl;
	std::cout<<"With coordinates X = "<<bestX<<", Y = "<<bestY<<std::endl;
	std::cout<<"########################################################"<<std::endl;
}

int menu(){
	int choice;  
	std::cout<<"Enter a number(1-8) of Hill Climbers to run [0 to Quit]: ";
	std::cin>>NUMTHREADS;
	std::cout<<std::endl;
	//If user enters within the bounds, continue
	if(NUMTHREADS < 1 || NUMTHREADS > 8){
		choice = 2;
	}
	//Quit
	else{
		choice = 1;
	}
	return choice;
}

int main() {
    bool Done = false;
	while(!Done){
		switch(menu()){
			case 1:{
				srand(time(NULL));
				pthread_t ct[NUMTHREADS];//our child threads
				continuing=true;
				std::cout<<"About to commence; PID: "<<getpid()<<std::endl;
				
				//interrupted function is run when  a SIGINT signal is recieved
				if (signal(SIGINT,interrupted)==SIG_ERR) {
					std::cout<<"Unable to change signal handler."<<std::endl;
					return 1;
				}

				for (int i=0;i<NUMTHREADS;i++) {
					pthread_mutex_lock(&lock);//reserve lock
					pthread_create(&ct[i], NULL, &hillClimber, NULL);
					occupied++;
					pthread_mutex_unlock(&lock);//release lock
				}
				
				std::cout<<NUMTHREADS<<" Hill-Climbers are climbing the Egg Holder function..."<<std::endl;
				std::cout<<"Ctrl-C to stop climbers and view progress."<<std::endl;
				
				while (occupied>0)
					sleep(1);
				break;
			}
			//If user enters less than 1 or greater than 8 they quit the program
			case 2:{
				Done = true;
				break;
			}
		}
	}
}
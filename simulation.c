/*

 joseph@NTU
 Date: 20170725

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define MAX_N_CHANNEL 20
#define MAX_N_user 20
#define MAX_N_Round 50
#define BW 1  
enum {FALSE=0,TRUE=1};
int N_user=1;
int N_CHANNEL=1;
float prob=0.1f;  
unsigned int payload_size;
enum e_channel_state{IDLE=0,BUSY=1,DIRTY=2};
enum e_user_state{WAIT=0,TX=1,BACKOFF=2,COMPLETE=3};
int n_complete=0;
char *str_Cstate[]={"Idle","Busy","Dirty"};
char *str_Ustate[]={"Wait","TX","BACKOFF","Complete"};


FILE *fp;  //record 
FILE *fp2;  //record 
#define WRITE_FILE 1

#define DEBUG 0

#define dbg_printf(fmt, ...)\
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__);\
	   }while (0)


typedef struct {
  
   enum e_channel_state state; 
   int num_user; // # of user in this channel
   int num_active_user; // # of active user in this channel
   int channel_bw; //fixed
   char available; // available is fasle if PU is in use   
               
}Channel;


typedef struct {
  
   int id;  
   int curr_ch;
   int complete_ch;
   unsigned long complete_time;
   char data_size;
   char collision;
   int num_collision;
   enum e_user_state user_state; 
               
}User;

//Function list ---------
void init_channel(Channel *c);
void init_user(User *user);
int select_channel();
void update_channel_state(Channel *c,User *user);
void show_channel_state(Channel *c);
void show_user_state(User *user,int flag_more);
void show_user_report(User *user);
int p();
//-------------------------
int main()
{
    
    Channel ch[MAX_N_CHANNEL];
    User user[MAX_N_user]; 
   // payload size determintes that how many time slots are required to transfer
   // payload size is proportinal to the number of channels 
    char filename[64]="result.txt"; //overall
	char filename2[64]="plot.txt"; //for plot
    unsigned long t;
    srand((unsigned) time(NULL));
    init_channel(ch);
    init_user(user);
    float  temp;
    int j=-1;
	int N_round;
	double total_T=0;
	
			#if WRITE_FILE==1 
			
			if ((fp = fopen(filename, "w+")) == NULL)
			{
				 fprintf(stderr,"Error! Can't create %s\n",filename);
				 exit(1);
			} 
			#endif 
	
		for(N_CHANNEL=1;N_CHANNEL<MAX_N_CHANNEL+1;N_CHANNEL+=2) {	
			dbg_printf("N_CHANNEL=%d====>\n",N_CHANNEL);

			for(N_user=1;N_user<MAX_N_user+1;N_user+=2) {
				 dbg_printf("N_user=%d==>\n",N_user);
				 
				#if WRITE_FILE==1 
				sprintf(filename2,"C%dU%d.txt",N_CHANNEL,N_user);
				if ((fp2 = fopen(filename2, "w+")) == NULL)
				{
					 fprintf(stderr,"Error! Can't create %s\n",filename2);
					 exit(1);
				} 
				#endif 
				 
				
			//	for(prob=0.1f;prob<=1.01f;prob+=0.1f) {
                for(prob=0.1f;prob<=0.91f;prob+=0.1f) {
					dbg_printf("prob=%6.2f==>\n",prob);
					  
					total_T=0;
					

					
						printf("111\n");
					
					for (N_round=1;N_round<=MAX_N_Round;N_round++) {
						 dbg_printf("N_Round=%d==>\n",N_round);
						 
						payload_size=N_CHANNEL;  
						init_channel(ch);
						init_user(user);
						n_complete=0;
				
				
						/*************************************************************************************/ 
						//at first, randomly select a channel for each user
						for (j=0;j<N_user;j++)  {
							user[j].curr_ch=select_channel();
							dbg_printf("%d %s\n",user[j].curr_ch,str_Ustate[user[j].user_state] );
						}
						
						update_channel_state(ch,user);
						// start to simulation
						for (t=0;t<100000;t++) 
						{  
						   dbg_printf("t=%d==>\n",t);
							  
						   //user select channel randomly
						   for (j=0;j<N_user;j++)  {            
							
								if (user[j].curr_ch==-1) continue;
							 
							   switch(user[j].user_state) {
								   
									case WAIT:
									
										if(ch[user[j].curr_ch].state==IDLE) {
											if (p()) user[j].user_state=TX;   //toss a coin to determine whether or not to enter  
															
										} 				
									
										break;
									
									case TX:
										
										user[j].data_size--;
										
										if (user[j].data_size==0) {
											user[j].user_state=(user[j].collision==TRUE)?(user[j].num_collision++,BACKOFF):COMPLETE;
											
										   }
										/*	
										if (user[j].user_state==COMPLETE){  
										   user[j].curr_ch=-1; //exit
										   user[j].complete_time=t;
										   n_complete++;
										   }
									 */
													
										break;
									case BACKOFF:
										 user[j].data_size=payload_size;
										 user[j].user_state=WAIT;
										 user[j].collision=FALSE; //clear collision state
										break;
								
									case COMPLETE:
										user[j].complete_ch=user[j].curr_ch;
										user[j].curr_ch=-1; //exit
										user[j].complete_time=t-1;
										n_complete++;
										break;
								
							   }
							
						   } //For loop--user
						   
						   
							show_user_state(user,0);
							update_channel_state(ch,user);
						  
							if (n_complete>=N_user) break;
						
						} //For loop--timeslot
					  
					   { //----show statistics -----
						  
						  unsigned long last_slot=t-1;
						  double T=(double)(N_user*payload_size)/last_slot;
						  total_T+=T;
						
					  
					   printf("N_CHANNEL=%d,N_user=%d,p=%4.2f,payload=%d,BW=%d,last_slot=%ld\n",N_CHANNEL,N_user,prob,payload_size,BW,last_slot);
					   show_user_report(user); 
					   printf("Throughtput=%10.3lf\n",T);
					  
						}
					   /**********************************************************************************************/ 
					
					
					} // round for average 
					
					{ //write overall to one file
					char wbuf[128];
				
			        #if WRITE_FILE==1  
						#define PRINT_FMT "%d\t%d\t%4.2f\t%d\t%d\t%d\t%6.3lf\n" 							
			        	printf("****N_CHANNEL=%d,N_user=%d,p=%4.2f,payload=%d,BW=%d,Round=%d,Avg T=%6.3lf***\n",N_CHANNEL,N_user,prob,payload_size,BW,MAX_N_Round,total_T/MAX_N_Round);
			            sprintf(wbuf,PRINT_FMT,N_CHANNEL,N_user,prob,payload_size,BW,MAX_N_Round,total_T/MAX_N_Round);   
				 	   fprintf(fp,"%s",wbuf);
					#endif

					}
                    
                    			
                    { //write for the different of channels
					char wbuf2[128];
				
			        #if WRITE_FILE==1   
						#define PRINT_FMT2 "%d\t%d\t%4.2f\t%6.3lf\n" 
			            sprintf(wbuf2,PRINT_FMT2,N_CHANNEL,N_user,prob,total_T/MAX_N_Round);   
				 	    fprintf(fp2,"%s",wbuf2);
					   
					#endif

			        }	
                    
                    
                    
                    					
				} // test probability from 0.1~1
				
							
			 #if WRITE_FILE==1 
			 if (fclose(fp2) != 0)
				fprintf(stderr,"Error closing file\n");
			 #endif 	
			
			} //test N_user from 1 to MAX N_user

		} //test N_channel from 1 to MAX N_channel	
		
    #if WRITE_FILE==1 
  	  printf("<<<<< Close file >>>>>>>\n");
	  if (fclose(fp) != 0)
		fprintf(stderr,"Error closing file\n");

	#endif 	

  getchar();  
  

 return 0;    
}

//------------------------------




void update_channel_state(Channel *c,User *user)
{
   int i;
   int j;
   
   init_channel(c); //reset channel state for each time slot
   
   for (i=0;i<N_user;i++){ 
     int ch=user[i].curr_ch;
	 if (ch==-1)continue;
     c[ch].num_user++;
      // printf("%d %d\n",user[i].curr_ch,user[i].user_state );
     if (user[i].user_state==TX) {  c[ch].num_active_user++; }

   } 

    for (i=0;i<N_CHANNEL;i++){  
     if (c[i].num_active_user>=2) {
          c[i].state=DIRTY; //collision occurs
		   for (j=0;j<N_user;j++){ //inform collision to all users on this channel  
		     if (user[j].curr_ch==i) {
                user[j].collision=TRUE;
               
                }
  	        }
		  
		  
     } else if (c[i].num_active_user==1)
          c[i].state=BUSY; //busy
     else  c[i].state=IDLE; //idle
     
   //  printf("c[%d].state=%d (%d)\n",i,c[i].state,c[i].num_active_user);
      
   }  
  
  
  


}

void show_channel_state(Channel *c)
{
  int i;
  for (i=0;i<N_CHANNEL;i++){
    dbg_printf("ch %d:state=%s,user=%d,active=%d,bw=%d MHz,availabel=%d\n",i,str_Cstate[c[i].state],
     c[i].num_user, c[i].num_active_user,c[i].channel_bw, c[i].available);
   }

}


void show_user_state(User *user,int flag_more)
{
  int i;
  for (i=0;i<N_user;i++){ 
     if (!flag_more & user[i].curr_ch==-1) continue;
         dbg_printf("user %d,ch=%d,state=%s,data=%d,n_collision=%d\n",
         i,user[i].curr_ch,str_Ustate[user[i].user_state],user[i].data_size,user[i].num_collision);
    }

}

void show_user_report(User *user)
{
  int i;
  for (i=0;i<N_user;i++){ 
         printf("user %d,complete_ch=%d,state=%s,complete_time=%d,n_collision=%d\n",
         i,user[i].complete_ch,str_Ustate[user[i].user_state],user[i].complete_time, user[i].num_collision);
    }

}





void init_channel(Channel *c)
{
  int i;
  for (i=0;i<N_CHANNEL;i++){
     c[i].state=IDLE;
     c[i].num_user=0;
     c[i].num_active_user=0;
     c[i].channel_bw=BW;
     c[i].available=1;
   }

}

void init_user(User *user)
{
  int i;
  for (i=0;i<N_user;i++){
     user[i].id=i;
     user[i].curr_ch=-1;
     user[i].data_size=payload_size;
     user[i].user_state=WAIT;//pending
	 user[i].collision=FALSE;
	 user[i].complete_ch=-1;
	 user[i].complete_time=10000000; //will update
	 user[i].num_collision=0;
   }

}

int select_channel()
{
   
   return rand() % N_CHANNEL;
   
   
  
}

int p()
{
   //we need to consider p=f(N_USER,N_CHANNEL,...)
   //now p is constant
   float x;
   x=(((float)rand()/(float)(RAND_MAX)) * 1.0);  //random range[0.0,1.0]
   dbg_printf("x=%f\n",x);
   return (x<prob)?1:0;  //return 1 enter the channel
   
}








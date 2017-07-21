#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define N_CHANNEL 5
#define N_user 3
#define BW 1

float prob=0.4;  
unsigned int payload_size=3;
enum e_channel_state{IDLE=0,BUSY=1,DIRTY=2};
enum e_user_state{WAIT=0,TX=1,COLLISION=2,COMPLETE=3};

char *str_Cstate[]={"Idle","Busy","Dirty"};
char *str_Ustate[]={"Wait","TX","Collision","Complete"};
typedef struct {
  
   char state;  //0:idle; 1: busy; 2:collision
   int num_user; // # of user in this channel
   int num_active_user; // # of active user in this channel
   int channel_bw; //fixed
   char available; // available is fasle if PU is in use   
               
}Channel;


typedef struct {
  
   int id;  
   int curr_ch;
   char data_size;
   
   int user_state; // 0: waiting 1:tx ,2:collision 
               
}User;

//Function list ---------
void init_channel(Channel *c);
void init_user(User *user);
int select_channel();
void update_channel_state(Channel *c,User *user);
void show_channel_state(Channel *c);
int p();
//-------------------------
int main()
{
    
    Channel ch[N_CHANNEL];
    User user[N_user];
    unsigned int t;
    srand((unsigned) time(NULL));
    init_channel(ch);
    init_user(user);
    float  temp;
    int j=-1;

    for (t=0;t<1;t++) 
    {  
       //user select channel randomly
       for (j=0;j<N_user;j++)  {            
          user[j].curr_ch=select_channel();
         
       
          if (p()) user[j].user_state=TX;   //toss a coin to determine whether or not to enter  
          else user[j].user_state=WAIT ;
          
          printf("%d %s\n",user[j].curr_ch,str_Ustate[user[j].user_state] );
       } //For loop--user
      
        update_channel_state(ch,user);
    
    } //For looop--timeslot
    
    
    
    
  getchar();  
 return 0;    
}

//------------------------------


void update_channel_state(Channel *c,User *user)
{
   int i;
   for (i=0;i<N_user;i++){ 
     int ch=user[i].curr_ch;
     c[ch].num_user++;
      // printf("%d %d\n",user[i].curr_ch,user[i].user_state );
     if (user[i].user_state==TX) {  c[ch].num_active_user++; }

   } 

    for (i=0;i<N_CHANNEL;i++){  
     if (c[i].num_active_user>=2)
          c[i].state=DIRTY; //collision
     else if (c[i].num_active_user==1)
          c[i].state=BUSY; //busy
     else  c[i].state=IDLE; //idle
     
   //  printf("c[%d].state=%d (%d)\n",i,c[i].state,c[i].num_active_user);
   }  
  
   show_channel_state(c) ;


}

void show_channel_state(Channel *c)
{
  int i;
  for (i=0;i<N_CHANNEL;i++){
    printf("ch%d:state=%s,user=%d,active=%d,bw=%d MHz,availabel=%d\n",i,str_Cstate[c[i].state],
     c[i].num_user, c[i].num_active_user,c[i].channel_bw, c[i].available);
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
 //  printf("x=%f\n",x);
   return (x<prob)?1:0;  //return 1 enter the channel
   
}








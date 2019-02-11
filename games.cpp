/* Date:Fri, 08 Feb 2019 22:42:50 +0800
 *   ---By ME  *_*
 * 
 * 
 */
#include <stdio.h>
#include "test.h"
#include <termio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <termios.h>  
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <cmath>

#define random_1(a,b) ((rand()%(b-a))+a)
#define random_2(a,b) ((rand()%(b-a+1))+a)

int tty_fd = -1;//??
struct termios save_tty,save_stdin,nt;//??
char map[24][80],obs[24][80],chartmp,temp;//map char buffer,obstacle char buffer
double TimePerFrame = 1000/50;//每帧固定的时间差,此处限制fps为50帧每秒
int speed_x = 0,speed_y = 0;//speed data
int player_x=19,player_y=7;//player's (x,y)
struct timeval tv;//about time
unsigned long lastTime,nowTime,deltaTime;//about time
bool tmpbool=false;//key up ^
int randnum,obsTmp,tmpUpDate=0;//temporary up variable
int thing_x=15,thing_y=79;
int upnum;
unsigned long lastmove,nowmove,deltamove,allmove=0;

void begin(){
	printf("Press any key to begin....\n");
	system("stty -echo");
	system("stty -icanon");
	getchar();
	system("stty echo");
	system("stty icanon");
}

void upAction(){
	if(tmpbool){
		gettimeofday(&tv,NULL);
		nowmove=tv.tv_usec/1000;
		deltamove=nowmove-lastmove;
		allmove+=deltamove;
		lastmove=nowmove;
		if(allmove>=90){
			speed_x=-1;
			if(upnum>3){
			speed_x=1;
			}
			allmove=0;
			upnum++;
		}
		if(upnum==7){
			tmpbool=false;
			upnum=0;
		}
		
	}
}

void initview(){
	for(int i=0;i<24;i++){
		for(int j=0;j<80;j++){
			obs[i][j]=' ';
			map[i][j]=' ';
		}
	}
	map[player_x][player_y]='@';
	for (int i=0;i<80;i++){
		obs[20][i]='#';
	}
	CLEAR();
	gettimeofday(&tv,NULL);
	lastTime=tv.tv_usec;
}

void obsAction(){
	obsTmp++;
	if(obsTmp%10==0){
		for(int i=0;i<20;i++){
			for(int j=0;j<80;j++){
				obs[i][j]=obs[i][j+1];
			}
			obs[i][79]=' ';
		}
	}
	if(obsTmp==5000){
		obsTmp=0;
	}
}

void updata(){
	randnum=random_2(0,100);
	thing_x=random_2(15,16);
	if(randnum==2){
		obs[thing_x][79]='#';
		obs[thing_x][78]='#';
		obs[thing_x][77]='#';
		obs[thing_x][76]='#';
		obs[thing_x][75]='#';
	}
	upAction();
	obsAction();
	for(int i=0;i<24;i++){
		for(int j=0;j<80;j++){
			map[i][j]=obs[i][j];
		}
	}
	map[player_x][player_y]=' ';
	int a=player_y+speed_y;
	if(a==-1||a==80){
		speed_y=0;
	}
	if(map[player_x+1][player_y]==' '&&!tmpbool){
		speed_x=1;
	}
	if(map[player_x+speed_x][player_y+speed_y]==' '){
		player_x+=speed_x;
		player_y+=speed_y;
	}
	map[player_x][player_y]='@';
	//printf("x=%d,y=%d",player_x,player_y);
	//printf("speedx=%d\n,speedy=%d\n",speed_x,speed_y);
	//printf("movetime=%ld,allmove=%ld",deltamove,allmove);
	//printf("num=%d",upnum);
	temp='~';
	speed_x=0;
	speed_y=0;
}

void upview(){
	//system("cls");
	for(int i=0;i<24;i++){
		for(int j=0;j<80;j++){
			printf("%c",map[i][j]);
		}
		printf("\n");
	}
}

int main(int argc,char*argv[])
{
	begin();
	int tty_fd=open("/dev/tty",O_RDONLY);
	tcgetattr(0,&save_stdin);
	nt=save_stdin;
	nt.c_lflag &= ~(ECHO|ICANON);//设为0所以用&
	tcsetattr(0,TCSANOW,&nt);
	
	int flags = fcntl(tty_fd,F_GETFL);
	flags |= O_NONBLOCK;//设为1所以用|
	if(fcntl(tty_fd,F_SETFL,flags)==-1){
		exit(1);
	}
	flags = fcntl(0,F_GETFL);
	//flags |= O_NONBLOCK;
	if(fcntl(0,F_SETFL,flags)==-1){
		exit(1);
	}
	
	initview();
	while(1){
		gettimeofday(&tv,NULL);
		nowTime=tv.tv_usec;
		deltaTime=nowTime-lastTime;
		//printf("nowtime=%ld,lastTime=%ld  ",nowTime,lastTime);
		lastTime=nowTime;
		
		read(tty_fd,&temp,1);
		switch(temp){
			case 'w':
			{
				tmpbool=true;
				gettimeofday(&tv,NULL);
				lastmove=tv.tv_usec/1000;
			}break;
			case 'a':
			{
				speed_y=-1;
			}break;
			case 'd':
			{
				speed_y=1;
			}break;
			default:;
		}

		updata();
		upview();
		printf("time=%ldus  ",deltaTime);
		if(deltaTime<=20000){
			usleep(TimePerFrame*1000-deltaTime);
		}
	}
	return 0;
}

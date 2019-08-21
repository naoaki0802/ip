#include<stdio.h>
#include<stdarg.h>//server側のプログラム
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<sys/types.h>
#include<arpa/inet.h> //inet_aton
#include<stdlib.h>//exit()
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include <termios.h>

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
} 

int mygetch( ) {
struct termios oldt,
newt;
int ch;
tcgetattr( STDIN_FILENO, &oldt );
newt = oldt;
newt.c_lflag &= ~( ICANON | ECHO );
tcsetattr( STDIN_FILENO, TCSANOW, &newt );
ch = getchar();
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
return ch;
}

 int main(int argc, char **argv){
    int N=5000;
   int fp=open("osakocut02.raw",O_RDONLY);
   int a=88200;
     char datao[a];
   while(1){
     int o =  read(fp, datao, a);
      if(o == 0){
       break;
       }
      if(o == -1){
       perror("read");
       exit(1);
     }
     }
   close(fp);
   int fp1=open("noise.raw",O_RDONLY);
   int a1=44100*4.1;
     char datan[a1];
   while(1){
     int o1 =  read(fp1, datan, a1);
      if(o1 == 0){
       break;
       }
      if(o1 == -1){
       perror("read");
       exit(1);
     }
     }
   close(fp1);
    int fp2=open("uc.raw",O_RDONLY);
   int a2=44100*5.5;
     char datau[a2];
   while(1){
     int o2 =  read(fp2, datau, a2);
      if(o2 == 0){
       break;
       }
      if(o2 == -1){
       perror("read");
       exit(1);
     }
     }
   close(fp2);
   char  data[N];
   char data2[N];
   int port2 = atoi(argv[1]);//コマンドライン引数でポート番号
   int ss = socket(PF_INET, SOCK_STREAM, 0);
   if(ss == -1){
     perror("socket");
     exit(1);
   }
   struct sockaddr_in addr;
   addr.sin_family = AF_INET;//IPv4のアドレス
   addr.sin_port = htons(port2);//ポートに接続
   addr.sin_addr.s_addr = INADDR_ANY;
   bind(ss, (struct sockaddr *)&addr, sizeof(addr));
   listen(ss, 10);
   struct sockaddr_in client_addr;
   socklen_t len = sizeof(struct sockaddr_in);
   int s = accept(ss, (struct sockaddr *)&client_addr, &len);
   close(ss);
   int c;
    FILE *gid;
    gid = popen("rec -t raw -b 8 -c 1 -e s -r 44100 -","r");//record
    FILE *gid2;
    gid2 = popen("play -t raw -b 8 -c 1 -e s -r 44100 -","w");//play
	  int i;
	  long int x=10000000000;
	  long int xx=10000000000;
	  long int xxx=10000000000;
   while(1){
      int r2 = fread(data,1,N,gid);
      if(kbhit()){
	c=mygetch();
      }
      if(c=='1' && x>=a/r2){
	x=0;
      } 
      if(x<a/r2){
           for(i=0;i<r2;i++){
	data[i]=data[i]+2*datao[i+x*r2];
	}
	   x=x+1;
      }
      
       if(c=='2' && xx>=a1/r2){
	xx=0;
      }
      
      if(xx<a1/r2){
           for(i=0;i<r2;i++){
	data[i]=0.3*data[i]+0.5*datan[i+xx*r2];
	}
	   xx=xx+1;
      }
      if(c=='3' && xxx>=a2/r2){
	xxx=0;
      }
      
      if(xxx<a2/r2){
           for(i=0;i<r2;i++){
	data[i]=3*data[i]+0.3*datau[i+xxx*r2];
	}
	   xxx=xxx+1;
      }
      write(s, data, r2);
      if(x>=a/r2){
	c='0';
      }
       if(xx>=a1/r2){
	c='0';
      }
        if(xxx>=a2/r2){
	c='0';
      }
      int r3 = read(s,data2,N);//送られた音をdata2に格納する
      fwrite(data2,1,N,gid2);//popenでplayする
   }
   close(s);
   return 0;
 }
      

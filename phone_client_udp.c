#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<sys/types.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<unistd.h>
#define size 4096
int recv_all(int, unsigned char*, int, int, struct sockaddr*, socklen_t*);
int read_all(int, unsigned char*, int);

int main(int argc,char **argv){
  unsigned char data[size],EOD[size],test[size],EOD2[size];
  int n;
  for (int i = 0; i < size; i++) {
	  EOD[i] = 15;
  }
  int s=socket(PF_INET,SOCK_DGRAM,0);
  if(s==-1){perror("socket");exit(1);}
  
  struct sockaddr_in addr;
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=inet_addr(argv[1]);
  addr.sin_port=htons(atoi(argv[2]));
  bind(s, (struct sockaddr*)&addr, sizeof(addr));
  socklen_t addr_len = sizeof(addr);
  n = sendto(s, EOD, size, 0, (struct sockaddr*)&addr, addr_len);
  if (n == -1) { perror("sendto"); exit(1); }

  int flag;
  
  /*while (1) {
	flag = 0;
    //n = sendto(s, EOD, size, 0, (struct sockaddr*)&addr, addr_len);
    //if (n == -1) { perror("sendto"); exit(1); }
  n = recv_all(s, test, size, 0, (struct sockaddr*)&addr, &addr_len);
  if (n == -1) { perror("recvfrom"); exit(1); }
  if (n == size) {
  flag = 1;
  for (int i = 0; i < size; i++) {
  if (test[i] != 15) {
  flag = 0; break;
  }
  }
  }
  if (flag == 1) break;
  }*/
  //usleep(3000);
  FILE *cmdout=popen("rec -t raw -b 16 -c 1 -e s -r 8820 --buffer 2048 -","r");
  FILE *cmdin=popen("play -t raw -b 16 -c 1 -e s -r 8820 --buffer 2048 -","w");

  int in=fileno(cmdin);
  if(in==-1){perror("fileno in");exit(1);}
  printf("in = %d\n",in);
  int out=fileno(cmdout);
  if(out==-1){perror("fileno out");exit(1);}
  printf("out = %d\n",out);


  while(1){
    n=recvfrom(s,data,size,0,(struct sockaddr*)&addr,&addr_len);
    if(n==-1){perror("recv");exit(1);}
    if(n==0){
      for(int i=0;i<2048;i++)
	data[i]=0;
      n=2048;
    }
    n=write(in,data,n);
    if(n==-1){perror("write");exit(1);}
    n=read_all(out,data,size);
    if(n==-1){perror("read");exit(1);}
    n=sendto(s,data,n,0,(struct sockaddr*)&addr,addr_len);
    if(n==-1){perror("send");exit(1);}
  }

  pclose(cmdin);
  pclose(cmdout);

  //close(s);

  return 0;
}

int recv_all(int s, unsigned char *buf, int len, int pro, struct sockaddr* addr, socklen_t* addr_len) {
	int received = 0;
	while (received < len) {
		int n = recvfrom(s, buf + received, len - received, pro, addr, addr_len);
		if (n == -1) { perror("recv"); exit(1); }
		else if (n == 0) { fprintf(stderr, "EOF!\n"); return received; }

		received += n;
	}
	return received;
}

int read_all(int f, unsigned char *buf, int len) {
	int al_read = 0;
	while (al_read < len) {
		int n = read(f, buf + al_read, len - al_read);
		if (n == -1) { perror("read"); exit(1); }
		if (n == 0) { fprintf(stderr, "EOF!\n"); return al_read; }
		al_read += n;
	}
	return al_read;
}

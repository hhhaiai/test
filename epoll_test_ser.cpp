#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/epoll.h>
#include<fcntl.h>
using namespace std;

int Socket(int af,int type,int protocol){
	int fd =socket(af,type,protocol);
	if(fd < 0){
		cout<<"socket error"<<endl;
		exit(-1);
	}
	return fd;
}

int Bind(int fd,struct sockaddr *addr,int len){
	int ret = bind(fd,addr,len);
	if(ret < 0){
		cout<<"bind error"<<endl;
		exit(-1);
	}
	return ret;
}

int Listen(int fd,int size){
	int ret = listen(fd,1024);
	if(ret < 0){
		cout<<"listen error"<<endl;
		exit(-1);
	}
	return ret;
}

int Epoll_create(int size){
	int ret = epoll_create(size);
	if(ret < 0){
		cout<<"epoll_create error"<<endl;
		exit(-1);
	}
	return ret;
}

int main(int argc,char **argv){
	int ser_fd,ep_fd;
	struct sockaddr_in ser_addr;
	
	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(12345);
	
	ser_fd = Socket(AF_INET,SOCK_STREAM,0);
	Bind(ser_fd,(struct sockaddr *) &ser_addr,sizeof(ser_addr));
	Listen(ser_fd,1024);
	
	int flags = fcntl(ser_fd,F_GETFL,0);
	fcntl(ser_fd,F_SETFL,flags | O_NONBLOCK);

	ep_fd = Epoll_create(1024);

	close(ser_fd);
	return(0);
}

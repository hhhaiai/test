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

#define EPOLL_SIZE 1024
#define LISTEN_SIZE 1024
#define PORT 12345

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

int Accept(int ser_fd,struct sockaddr *cli_addr,socklen_t *sock_len){
	int cli_fd = accept(ser_fd,cli_addr,sock_len);
	if(cli_fd < 0){
		cout<<"accept error"<<endl;
		exit(-1);
	}
	return cli_fd;
}

int Epoll_create(int size){
	int fd = epoll_create(size);
	if(fd < 0){
		cout<<"epoll_create error"<<endl;
		exit(-1);
	}
	return fd;
}

int Epoll_ctl(int epfd,int op,int fd,struct epoll_event *event){
	int ret = epoll_ctl(epfd,op,fd,event);
	if(ret < 0){
		cout<<"epoll_ctl error"<<endl;
		exit(-1);
	}
	return ret;
}

int Epoll_wait(int ep_fd,struct epoll_event *events,int epoll_size,int timeout){
	int event_num = epoll_wait(ep_fd,events,epoll_size,timeout);
	if(event_num < 0){
		cout<<"epoll_wait error"<<endl;
		exit(-1);
	}
	return event_num;
}

int main(int argc,char **argv){
	int ser_fd,cli_fd,ep_fd,event_num;
	char addr[20],buf[10];
	struct sockaddr_in ser_addr,cli_addr;
	struct epoll_event event,events[EPOLL_SIZE];
	socklen_t sock_len = sizeof(struct sockaddr);
	
	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ser_addr.sin_port = htons(PORT);
	
	ser_fd = Socket(AF_INET,SOCK_STREAM,0);
	Bind(ser_fd,(struct sockaddr *) &ser_addr,sizeof(ser_addr));
	Listen(ser_fd,LISTEN_SIZE);
	
	int flags = fcntl(ser_fd,F_GETFL,0);
	fcntl(ser_fd,F_SETFL,flags | O_NONBLOCK);

	event.data.fd = ser_fd;
	event.events = EPOLLIN | EPOLLET;
	ep_fd = Epoll_create(EPOLL_SIZE);
	Epoll_ctl(ep_fd,EPOLL_CTL_ADD,ser_fd,&event);
	
	for(;;){
		event_num = Epoll_wait(ep_fd,events,EPOLL_SIZE,-1);
		
		for(int i = 0;i < event_num;i++){
			if(events[i].data.fd == ser_fd){
				cli_fd = Accept(events[i].data.fd,(struct sockaddr *) &cli_addr,&sock_len);
	
				flags = fcntl(cli_fd,F_GETFL,0);
				fcntl(cli_fd,F_SETFL,flags | O_NONBLOCK);
				
				event.data.fd = cli_fd;
				event.events = EPOLLIN | EPOLLET;
				Epoll_ctl(ep_fd,EPOLL_CTL_ADD,cli_fd,&event);
				
				inet_ntop(AF_INET,&cli_addr.sin_addr,addr,sizeof(addr));
				cout<<addr<<" connected"<<endl;
			}else{
				while(read(events[i].data.fd,buf,10) > 0){
					cout<<buf<<endl;
					//clear buf[]
				}
				//when to close client and EPOLL_CTL_DEL
			}
		}
	}
	
	close(ep_fd);
	close(ser_fd);
	return(0);
}

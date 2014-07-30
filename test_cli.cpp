#include<iostream>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/select.h>
using namespace std;

int main(int argc,char **argv){
	int cli_fd;
	struct sockaddr_in cli_add;
	bzero(&cli_add,sizeof(cli_add));
	cli_add.sin_family = AF_INET;
	cli_add.sin_port = htons(12345);
	inet_pton(AF_INET,"121.10.118.102",&cli_add.sin_addr);

	cli_fd = socket(AF_INET,SOCK_STREAM,0);
	connect(cli_fd,(struct sockaddr *) &cli_add,sizeof(cli_add));
	string s;
	for(;;){
		cin>>s;
		write(cli_fd,(char *) s.c_str(),s.length());
	}
}

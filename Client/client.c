
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFF 1024
char key[]="111"; //chat all
char key1[]="100"; // chat Gr A
void *recvmg(void *sock)
{
	int their_sock = *((int *)sock);
	char msg[BUFF];
	int len;
	while ((len = recv(their_sock, msg, BUFF, 0)) > 0)
	{
		msg[len] = '\0';
		printf("%s \n", msg);
		//bzero(&msg,sizeof(msg));
		memset(msg, '\0', sizeof(msg));
		fflush(stdout);
	}
};
int main(int argc, char *argv[])
{
	struct sockaddr_in their_addr;
	int my_sock;
	//int their_sock;
	//int their_addr_size;
	pthread_t sendt, recvt;

	char ip[INET_ADDRSTRLEN];

	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(their_addr.sin_zero, '\0', sizeof(their_addr.sin_zero));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(5555);
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(my_sock, (struct sockaddr *)&their_addr, sizeof(their_addr)) < 0)
	{
		perror("connection not esatablished");
		exit(1);
	}
	char username[BUFF], var[BUFF];
	memset(username, '\0', BUFF);
	memset(var, '\0', sizeof(BUFF));
	printf("Enter your username: ");
	fgets(var,BUFF,stdin);
	var[strlen(var)-1]= '\0';
	write(my_sock, var, strlen(var));
	// Loi o day: Gui thua ki tu sau username
	
	strcpy(username, var);
	printf("%s",var);
	memset(var,'\0',sizeof(BUFF));

	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
	printf(" 1. Chat All \n");
	printf(" 2. Chat Group\n");
	printf(" 3. Chat peer to peer\n");
	printf(" Enter your choose: ");
	int choose;
	scanf("%d", &choose); // chon kieu chat
	char msg[BUFF];
	memset(msg, '\0', sizeof(BUFF));
	if (choose == 1)
	{

		int i=0;
		char res[BUFF];
		char offline[BUFF];
		pthread_create(&recvt, NULL,recvmg, &my_sock);
		fflush(stdin);
		while (fgets(msg, BUFF, stdin) > 0)
		{
			msg[strlen(msg) - 1] = '\0';
			strcpy(res, username);
			strcat(res, ": ");
			strcat(res, msg);
			strcpy(offline, username);
			strcat(offline, " is offline!");
			if (strcmp(msg, "@\n") == 0)
			{
				write(my_sock, offline, strlen(offline));
				exit(0);
			}
			else
			{
				int len;
				//send mess
				if(i>=1){
					len = write(my_sock, res, strlen(res));
					if (len < 0)
					{
						perror("message not sent");
						exit(1);
					}

				}
				i++;
				memset(msg, '\0', sizeof(msg));
				memset(res, '\0', sizeof(res));
			}
		}
		pthread_join(recvt, NULL);
	}
	if (choose == 2)
	{
		int x;// chon nhom
		int i = 0;
		char res[BUFF];
		char offline[BUFF];
		char mess[BUFF];
		printf("1.GroupA \n");
		printf("2.GroupB \n");
		scanf("%d", &x);
		//memset(key,'\0', sizeof(key));
		if (x == 1)
		{
			write(my_sock, key1, strlen(key1));
			pthread_create(&recvt, NULL, recvmg, &my_sock);
			while (1)
			{
				
				memset(msg, '\0', sizeof(msg));
				memset(mess, '\0', sizeof(mess));
				fflush(stdin);
				fgets(msg, BUFF, stdin);
				msg[strlen(msg)-1]= '\0';
				//printf("%s",mess);
				strcpy(mess, msg);
				strcpy(res, username);
				strcat(res, ": ");
				strcat(res, mess);
				strcat(res, "100");
				//printf("res: %s", res);
				if (i >= 1)
					write(my_sock, res, strlen(res));
				
				i++;
			}
			pthread_join(recvt, NULL);
		}
	}

	close(my_sock);
}
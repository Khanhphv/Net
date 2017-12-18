
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define BUFF 1024
char key[] = "111", //chat all
	key1[] = "100", // chat Gr A
	key2[] = "102", //chat Gr B
	key3[] = "103"; //Chat 1 vs 1
void sentFile(char *msg, int connfd)
{

	int j = 0;
	char name[BUFF];
	bzero(&name, BUFF);
	for (int i = 5; i < (strlen(msg)); i++) // bo di cai duoi 100
	{
		name[j] = msg[i];
		j++;
	}
	name[strlen(name)] = '\0';
	printf("%s\n", name);
	FILE *file;
	file = fopen(name, "r");
	if (file == NULL)
	{
		printf("File not found\n");	
	}
	else
	{
		while (1)
		{
			char secondbuff[256];
			int nread = fread(secondbuff, 1, 256, file);
			if (nread > 0)
			{
				write(connfd, secondbuff, nread);
			}

			if (nread < 256)
			{
				break;
			}
		}
	}
}
void recvfile(void *sock){
	int their_sock = *((int *)sock);
	int bytesReceived;
	char buff[256];
	FILE *file = fopen("down", "w+");

	while ((bytesReceived = recv(their_sock, buff, sizeof(buff), 0)) > 0)
	{
		//printf("Bytes received %d\n",bytesReceived);
		//printf("%s\n",buff );
		fwrite(buff, 1, bytesReceived, file);
		if (bytesReceived < 256)
		{
			printf("Download complete\n");
			break;
		}
	}
	fclose(file);
}

void *recvmg(void *sock)
{
	int their_sock = *((int *)sock);
	char msg[BUFF];
	int len;
	while ((len = recv(their_sock, msg, BUFF, 0)) > 0)
	{
		msg[len] = '\0';
		if((strstr(msg, "---f:"))!=NULL){
			recvfile(&their_sock);
		}
			printf("%s \n", msg);
			//bzero(&msg,sizeof(msg));
			memset(msg, '\0', sizeof(msg));
			fflush(stdout);
	
	}
}
int main(int argc, char *argv[])
{
	struct sockaddr_in their_addr;
	int my_sock;
	char ip[INET_ADDRSTRLEN];
	pthread_t recvt;
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

	//nhap ten dang nhap
	char username[BUFF], var[BUFF];
	memset(username, '\0', BUFF);
	memset(var, '\0', sizeof(BUFF));
	printf("Enter your username: ");
	fgets(var, BUFF, stdin);
	var[strlen(var) - 1] = '\0';
	strcpy(username, var);
	//gui username len server
	write(my_sock, var, strlen(var));
	// Loi o day: Gui thua ki tu sau username
	memset(var, '\0', sizeof(BUFF));

	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
	printf(" 1. Chat All \n");
	printf(" 2. Chat Group\n");
	printf(" 3. Chat peer to peer\n");
	printf(" Enter your choose: ");
	//pthread_join(recvt, NULL);
	//chon kieu chat
	int choose;
	scanf("%d", &choose);

	char msg[BUFF];
	memset(msg, '\0', sizeof(BUFF));

	//kieu chat ALL
	if (choose == 1)
	{
		write(my_sock, key, strlen(key));
		int i = 0;
		char res[BUFF];
		pthread_create(&recvt, NULL, recvmg, &my_sock);
		fflush(stdin);
		while (fgets(msg, BUFF, stdin) > 0)
		{
			msg[strlen(msg) - 1] = '\0';
			if (strcmp(msg, "@") == 0)
			{
				return 0;
				break;
			}
			if (strstr(msg, "FILE:") != NULL) //nhap file
			{
				write(my_sock, msg, strlen(msg));
				sentFile(msg, my_sock);
				memset(msg, '\0', BUFF);
				continue;
			}
			if(strstr(msg,"---f")!=NULL){
				write(my_sock, msg, strlen(msg));

				//recvfile(my_sock)x);
				memset(msg, '\0', BUFF);
				continue;
			}
			strcpy(res, username);
			strcat(res, ": ");
			strcat(res, msg);

			
			//send mess
			if (i >= 1)
			{
				write(my_sock, res, strlen(res));
				
			}
			i++;
			memset(msg, '\0', sizeof(msg));
			memset(res, '\0', sizeof(res));
		}
		pthread_join(recvt, NULL);
	}

	//kieu chat Group
	if (choose == 2)
	{
		int x; // chon nhom
		int i = 0;
		char res[BUFF];
		char mess[BUFF];
		printf(" 1.GroupA \n");
		printf(" 2.GroupB \n");
		printf(" Choose : ");
		scanf("%d", &x);
		//flush(stdin);
		//chon Group A
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
				msg[strlen(msg)-1] = '\0';
				if (strcmp(msg, "@") == 0)
				{
					return 0;
					break;
				};
				strcpy(mess, msg);
				strcpy(res, username);
				strcat(res, ": ");
				strcat(res, mess);
				strcat(res, key1);
				//printf("res: %s", res);
				if (i >= 1)
					write(my_sock, res, strlen(res));

				i++;
				memset(msg, '\0', sizeof(msg));
				memset(res, '\0', sizeof(res));
			}
			pthread_join(recvt, NULL);
		}
		else
		{
			//chon Group B
			if (x == 2)
			{
				write(my_sock, key2, strlen(key2));
				pthread_create(&recvt, NULL, recvmg, &my_sock);
				while (1)
				{
					memset(msg, '\0', sizeof(msg));
					memset(mess, '\0', sizeof(mess));
					fflush(stdin);
					fgets(msg, BUFF, stdin);
					msg[strlen(msg) - 1] = '\0';
					if (strcmp(msg, "@") == 0)
					{
						return 0;
						break;
					};
					strcpy(mess, msg);
					strcpy(res, username);
					strcat(res, ": ");
					strcat(res, mess);
					strcat(res, key2);
					//printf("res: %s", res);
					if (i >= 1)
						write(my_sock, res, strlen(res));

					i++;
					memset(msg, '\0', sizeof(msg));
					memset(res, '\0', sizeof(res));
				}
				pthread_join(recvt, NULL);
			}
		}
	}
	//chat 1 vs 1
	if (choose == 3)
	{
		int i = 0;
		write(my_sock, key3, strlen(key3));
		pthread_create(&recvt, NULL, recvmg, &my_sock);
		while (1)
		{
			char msg[BUFF], mess[BUFF], res[BUFF];
			memset(msg, '\0', sizeof(msg));
			memset(mess, '\0', sizeof(mess));
			fflush(stdin);
			fgets(msg, BUFF, stdin);
			msg[strlen(msg)-1] = '\0';
			if (strcmp(msg, "@") == 0)
			{
				return 0;
				break;
			}
			strcpy(mess, msg);
			strcpy(res, username);
			strcat(res, ": ");
			strcat(res, mess);
			strcat(res, key3);
			//printf("res: %s", res);
			if (i >= 1)
				write(my_sock, res, strlen(res));

			i++;
			memset(msg, '\0', sizeof(msg));
			memset(res, '\0', sizeof(res));
		}
		pthread_join(recvt, NULL);
	}

	close(my_sock);
}
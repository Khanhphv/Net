
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
int n = 0, var =0;			// so luong ng online
char key[] = "111", // loai chat all
	key1[] = "100", // loai tin nhan GroupA
	key2[] = "102", //loai tin nhan Group B
	key3[] = "103";
struct client_info
{
	int sockno;
	char name[BUFF];
	int group;
};
struct client_info all[100];
int groupA[100], countofA = 0; // danh sach cac socket cua client trong A

; // danh sach cac socket cua cac client

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



void removeUser(void *sock)
{
	
	struct client_info cl = *((struct client_info *)sock);
	//xoa ng vua disss
	int i, j;
	printf("%s disconnected\n", cl.name);
	for (i = 0; i <= n; i++)
	{
		if (all[i].sockno == cl.sockno)
		{
			j = i;
			while (j < n - 1)
			{
				all[j].sockno = all[j + 1].sockno;
				all[j].group = all[j + 1].group;
				strcpy(all[j + 1].name, all[j + 1].name);
				j++;
			}
		}
	}
	
}
//sent to all
void sendtoall(char *msg, int curr)
{
	int i;
	pthread_mutex_lock(&mutex);
	for (i = 0; i < n; i++)
	{
		if (all[i].sockno != curr)
		{
			if (write(all[i].sockno, msg, strlen(msg)) < 0)
			{
				perror("sending failure");
				continue;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
}
void sendtogroup(char *msg, int curr, int group)
{
	int i;
	pthread_mutex_lock(&mutex);
	for (i = 0; i < n; i++)
	{

		if ((all[i].sockno != curr) && (all[i].group == group))
		{
			printf(" Online:::%d \n", all[i].sockno);
			if (write(all[i].sockno, msg, strlen(msg)) < 0)
			{
				perror("sending failure");
				continue;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
}

void * recvmg(void *sock)
{
	struct client_info cl = *((struct client_info *)sock);
	char msg[BUFF];
	bzero(&msg, BUFF);
	int len;
	while ((len = read(cl.sockno, msg, sizeof(msg))) > 0)
	{
	
		//kiem tra kieu tin nhan la gi?

		//group A
		if ((strstr(msg, key1)) != NULL) //loai tin nhan cua Gr A
		{
			char mess[BUFF];
			bzero(&mess, BUFF);
			cl.group = 1;
			pthread_mutex_lock(&mutex);
			all[n - 1].group = cl.group;
			pthread_mutex_unlock(&mutex);
			if (strcmp(msg, key1) == 0) // giong kieu nhan dc yeu cau gia nhap nhom
				continue;

			for (int i = 0; i < (strlen(msg) - 3); i++) // bo di cai duoi 
			{
				mess[i] = msg[i];
			}
			mess[strlen(mess)] = '\0';
			//printf(":::%s\n", mess);
			sendtogroup(mess, cl.sockno, cl.group);
			memset(mess, '\0', sizeof(msg));
			memset(msg, '\0', sizeof(msg));
			fflush(stdout);
		}
		else {
			if ((strstr(msg, key2)) != NULL)
			{
				char mess[BUFF];
				bzero(&mess, BUFF);
				cl.group = 2;
				all[n - 1].group = cl.group;
				if (strcmp(msg, key2) == 0) // giong kieu nhan dc yeu cau gia nhap nhom
					continue;

				for (int i = 0; i < (strlen(msg) - 3); i++) // bo di cai duoi 
				{
					mess[i] = msg[i];
				}
				mess[strlen(mess)] = '\0';
				//printf(":::%s\n", mess);
				sendtogroup(mess, cl.sockno, cl.group);
				memset(mess, '\0', sizeof(msg));
				memset(msg, '\0', sizeof(msg));
				fflush(stdout);
			}
			else
			{
				//loai tin nhan chat  1 vs 1
				if((strstr(msg,key3)!=NULL)){
					if (strcmp(msg, key3) == 0){
						int var = 0;
						for(int i =0; i<n ; i++){
							if(all[i].group==3)
							var ++;
						}
						//printf("var = %d", var);
						if( var < 2){
							cl.group = 3;
							all[n - 1].group = cl.group;
							continue;
						}
						else{
							removeUser(&cl);
							char full[]= "room full";
							write(cl.sockno,full,strlen(full));
						}
						
						
					} // giong kieu nhan dc yeu cau gia nhap nhom
					char mess[BUFF];
					bzero(&mess, BUFF);
				
					for (int i = 0; i < (strlen(msg) - 3); i++) // bo di cai duoi 
					{
						mess[i] = msg[i];
					}
					mess[strlen(mess)] = '\0';
					//printf(":::%s\n", mess);
					sendtogroup(mess, cl.sockno, cl.group);
					memset(mess, '\0', sizeof(msg));
					memset(msg, '\0', sizeof(msg));
					fflush(stdout);
				}
				else{
					// loai tin nhan chat all
					if (strcmp(msg, key) == 0)
						continue;
					if (strstr(msg, "FILE:") != NULL)
					{	//up file tu client len server
						pthread_mutex_lock(&mutex);
						int bytesReceived;
						char buff[256];
						FILE *file = fopen("up", "w+");

						while ((bytesReceived = recv(cl.sockno, buff, sizeof(buff), 0)) > 0)
						{
							
							fwrite(buff, 1, bytesReceived, file);
							if (bytesReceived < 256)
							{
								printf("Download complete\n");
								break;
							}
						}
						fclose(file);
						pthread_mutex_unlock(&mutex);
					}
					else{
						if (strstr(msg, "---f:") != NULL)
						{
							write(cl.sockno, msg, strlen(msg) );
							int j = 0;
							char name[BUFF];
							bzero(&name, BUFF);
							for (int i = 5; i < (strlen(msg)); i++) // bo di cai duoi 100
							{
								name[j] = msg[i];
								j++;
							}
							name[strlen(name)] = '\0';
							printf("file name:%s\n", name);
							FILE *file;
							file = fopen(name, "r");
							if (file == NULL)
							{
								
								printf("File not found\n");
								char found[] = "File not found";
								write(cl.sockno, found, sizeof(found));
							}
							else
							{
								while (1)
								{
									char secondbuff[256];
									int nread = fread(secondbuff, 1, 256, file);
									if (nread > 0)
									{
										write(cl.sockno, secondbuff, nread);
									}

									if (nread < 256)
									{
										break;
									}
								}
							}
						}
					}
					sendtoall(msg, cl.sockno);
					memset(msg, '\0', sizeof(msg));
					fflush(stdout);
				}
					
		
			}

		}
	}
	pthread_mutex_lock(&mutex);
	removeUser(&cl);
	n--;
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
	struct sockaddr_in my_addr, their_addr;
	int my_sock;
	int their_sock;
	socklen_t their_addr_size;

	pthread_t recvt;
	char msg[BUFF];
	struct client_info cl;

	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(5555);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	their_addr_size = sizeof(their_addr);

	if (bind(my_sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0)
	{
		perror("binding unsuccessful");
		exit(1);
	}
	printf("Server Ready\n");
	if (listen(my_sock, 10) != 0)
	{
		perror("listening unsuccessful");
		exit(1);
	}

	while (1)
	{
		if ((their_sock = accept(my_sock, (struct sockaddr *)&their_addr, &their_addr_size)) < 0)
		{
			perror("accept unsuccessful");
			exit(1);
		}

		read(their_sock, msg, sizeof(msg));
		msg[strlen(msg)] = '\0';
		printf("%s:", msg);
		fflush(stdout);

		pthread_mutex_lock(&mutex);
		printf("connected\n");
		strcpy(cl.name, msg);
		memset(&msg, '\0', BUFF);
		cl.sockno = their_sock;
		all[n].sockno = cl.sockno;
		strcpy(all[n].name, cl.name);
		n++;
		pthread_create(&recvt, NULL, &recvmg, &cl);
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}
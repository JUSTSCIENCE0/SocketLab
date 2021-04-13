#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <list>
#include <cstring>

char messages[2048] = "";
int msize = 0;
pthread_mutex_t m;

class Client
{
private:
    int socket;
public:
    Client(){}

    Client(int sock)
    {
        printf("Client %d created\n", sock);
        socket = sock;
    }

    void Handle()
    {
        printf("Client %d handler started\n", socket);
        while(true)
        {
            char buffer[64] = "";
            int size = recv(socket, buffer, 64, 0);
            if (size<=0)
            {
                printf("Client %d dead\n", socket);
                close(socket);
                return;
            }
            if (!strcmp(buffer, "recv_mes"))
            {
                pthread_mutex_lock(&m);
                printf("Client %d want all messages\n", socket);
                send(socket, messages, msize, 0);
                pthread_mutex_unlock(&m);
                continue;
            }
            printf("Client %d send mes: %s", socket, buffer);
            pthread_mutex_lock(&m);
            if (msize+size >= 2048)
            {
               memset(messages, 0, 2048);
               msize=0;
            }
            memcpy(messages+msize, buffer, size);
            msize += size;
            pthread_mutex_unlock(&m);
        }
    }
};

int serv_sock = 0;
Client clients[20];
pthread_t client_thrd[20];
int client_num = 0;

void* client_handler(void* client_id)
{
    pthread_mutex_lock(&m);
    int id = *(int*)client_id;
    *(int*)client_id+=1;
    pthread_mutex_unlock(&m);
    clients[id].Handle();
    return NULL;
}

int main()
{
   struct sockaddr_in addr;
   serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (serv_sock == -1)
   {
      printf("Server socket couldn't created\n");
      return -1;
   }

   addr.sin_family = AF_INET;
   addr.sin_port = htons(50001);
   addr.sin_addr.s_addr = INADDR_ANY;

   int err = bind(serv_sock, (struct sockaddr*) &addr, sizeof(addr));
   if (err < 0)
   {
      printf("Server couldn't bind\n");
      return -1;
   }

   err = listen(serv_sock, 25);
   if (err < 0)
   {
      printf("Server couldn't listen\n");
      return -1;
   }

   pthread_mutex_init(&m, NULL);
   printf("Server %d started\n", serv_sock);
   while (true)
   {
      int client_socket = accept(serv_sock, NULL, NULL);
      if (client_socket <= 0)
      {
         printf("Accept error\n");
         return -1;
      }

      pthread_mutex_lock(&m);
      if (client_num >= 20)
      {
         printf("Max clients number\n");
         continue;
      }

      clients[client_num] = Client(client_socket);
      pthread_mutex_unlock(&m);

      int status = pthread_create(&client_thrd[client_num], NULL, client_handler, (void*)&client_num);
      if (status)
      {
         perror("pthread error\n");
         return -1;
      }

   }

   return 0;
}

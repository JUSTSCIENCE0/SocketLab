#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <list>
#include <cstring>
#include <arpa/inet.h>

char messages[2048] = "";
int msize = 0;
pthread_mutex_t m;
int serv_sock = 0;

int main()
{
   struct sockaddr_in addr, serv;
   serv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (serv_sock == -1)
   {
      printf("Server socket couldn't created\n");
      return -1;
   }

   memset(&serv, 0, sizeof(serv));
   memset(&addr, 0, sizeof(addr));

   serv.sin_family = AF_INET;
   serv.sin_addr.s_addr = INADDR_ANY;
   serv.sin_port = htons(50002);

   addr.sin_family = AF_INET;
   addr.sin_port = htons(50003);
   addr.sin_addr.s_addr = INADDR_ANY;
   uint32_t sock_size = sizeof(addr);

   if (bind(serv_sock, (const struct sockaddr*)&serv, sizeof(serv)) < 0)
   {
       printf("Bind error\n");
       return -1;
   }

   printf("Server %d started\n", serv_sock);
   while(true)
   {
       char msg[64] = "";
       int size = recvfrom(serv_sock, msg, 64, 0, (sockaddr*)&addr, &sock_size);

       if (size <= 0)
       {
          printf("Server socket error\n");
          return -1;
       }

       if (msg[0] != 0xa)
       {
          printf("Received message %s", msg);
       }
   }

   return 0;
}

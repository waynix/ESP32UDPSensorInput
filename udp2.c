#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <stdlib.h>
#include "keccak-tiny.h"
#include <sys/timeb.h>


#define HASHBITS 256
#define HASHLEN HASHBITS/8
#define PORT 1234

int testdata(ssize_t len,uint8_t* buff){
  if(len<0)
    return 1;
  uint8_t hash_test[HASHLEN];
  uint8_t hash_got[HASHLEN];
  memcpy(hash_got,buff,HASHLEN);
  memcpy(buff,"01234567890123456789012345678901",HASHLEN);
  sha3_256(hash_test,HASHLEN,buff,len+HASHLEN);
  return memcmp(hash_test,hash_got,HASHLEN);
}

void extract_date(ssize_t restlen,uint8_t* buffer){
  uint32_t date;
  if(restlen<4)
    printf("No date");
  memcpy(&date,buffer,4);
  printf("Date: %u\n",ntohl(date));
}

void printdata(ssize_t restlen,uint8_t* buffer)
{
  if(restlen<2){
    printf("End of Message");
    return;
  }
  //uint16_t tag=((uint16_t*)buffer)[0];
  
  //printf("datagram: %.*s\n", (int)restlen, buffer);
}

int main(void)
{
  int sock;
  struct sockaddr_in6 sa; 
  uint8_t buffer[16024];
  ssize_t recsize;
  socklen_t fromlen;

  memset(&sa, 0, sizeof sa);
  sa.sin6_family = AF_INET6;
  sa.sin6_addr = in6addr_any;
  sa.sin6_port = htons(PORT);
  fromlen = sizeof sa;

  sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
  if (bind(sock, (struct sockaddr *)&sa, sizeof sa) == -1) {
    perror("error bind failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  for (;;) {
    recsize = recvfrom(sock, (void*)buffer, sizeof(buffer), 0, (struct sockaddr*)&sa, &fromlen);
    if (recsize < 0) {
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    recsize-=HASHLEN;
    if(!testdata(recsize,buffer)){
      printf("recsize: %d\n ", (int)recsize);
      extract_date(recsize,buffer+HASHLEN);
      printdata(recsize,buffer+HASHLEN+4);
      //printf("%64x\n",(uint64_t)(buffer+HASHLEN));
    }else{
      printf("Wrong Checksum, %d bits\n",recsize*8);
    }
  }
}

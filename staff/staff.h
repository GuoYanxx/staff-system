#ifndef STAFF_H_
#define STAFF_H_
#define ERR_MSG(msg) do{\
	printf("__%d__\n",__LINE__);\
	perror(msg);\
}while(0)

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <signal.h>
#include <sys/wait.h>

#define N 512

typedef struct pack{
	char sign;
	char id[N];
	char name[N];
	char password[N];
	char phone[N];
	char address[N];
	int flag;
	int age;
	int pay;
	int vip;
}__attribute__((__packed__)) pack;

//cli
int do_staff_register(int sfd,struct sockaddr_in sin);
int do_manager_register(int sfd,struct sockaddr_in sin);
int do_staff_login(int sfd,struct sockaddr_in sin);
int do_manager_login(int sfd,struct sockaddr_in sin);
int do_manager_function(int sfd,struct sockaddr_in sin,struct pack snd);
int do_staff_function(int sfd,struct sockaddr_in sin,struct pack snd);
int do_staff_change(int sfd,struct sockaddr_in sin,struct pack snd);
int do_add(int sfd,struct sockaddr_in sin,struct pack snd);
int do_delete(int sfd,struct sockaddr_in sin,struct pack snd);
int do_change(int sfd,struct sockaddr_in sin,struct pack snd);
int do_change_name(int sfd,struct sockaddr_in sin,struct pack snd);
int do_change_password(int sfd,struct sockaddr_in sin,struct pack snd);
int do_change_phone(int sfd,struct sockaddr_in sin,struct pack snd);
int do_change_address(int sfd,struct sockaddr_in sin,struct pack snd);
int do_change_pay(int sfd,struct sockaddr_in sin,struct pack snd);
int do_change_age(int sfd,struct sockaddr_in sin,struct pack snd);
int do_select(int sfd,struct sockaddr_in sin,struct pack snd);
int do_staff_select(int sfd,struct sockaddr_in sin,struct pack snd);
int do_quit(int sfd,struct sockaddr_in sin,struct pack snd);



//ser
int do_recv(sqlite3* db,int sfd,struct sockaddr_in cin,int newfd);
int do_staff_register_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_manager_register_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_login(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_add_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_delete_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_change_name_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_change_password_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_change_phone_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_change_address_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_change_pay_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_change_age_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_select_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
int do_quit_1(int sfd,sqlite3* db,struct pack rcv,int newfd);
#endif

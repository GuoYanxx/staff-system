#include "staff.h"

#define PORT 8888
#define IP "192.168.1.128"

int main(int argc, const char *argv[])
{
	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd < 0)
	{
		ERR_MSG("socket");
		return -1;
	}

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);
	int reuse = 1;
	if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))<0)
	{
		ERR_MSG("setsockopt");
		return -1;
	}
	if(connect(sfd,(struct sockaddr*)&sin,sizeof(sin))<0)
	{
		ERR_MSG("connect");
		return -1;
	}

	char choose=0;
	while(1)
	{
		printf("******************\n");
		printf("****1.员工注册****\n");
		printf("***2.管理员注册***\n");
		printf("****3.员工登录****\n");
		printf("***4.管理员登录***\n");
		printf("******5.退出******\n");
		printf("******************\n");
		printf("请输入>>>");
		choose = getchar();
		while(getchar()!=10);
		switch(choose)
		{
		case '1':
			do_staff_register(sfd,sin);
			break;
		case '2':
			do_manager_register(sfd,sin);
			break;
		case '3':
			do_staff_login(sfd,sin);
			break;
		case '4':
			do_manager_login(sfd,sin);
			break;
		case '5':
			goto END;
		default:
			printf("输入有误\n");
			break;
		}	
	}
END:
	close(sfd);
	return 0;
}

int do_staff_register(int sfd,struct sockaddr_in sin)
{
	pack snd;
	pack rcv;
	snd.sign='R';
	printf("请输入账号>>>");
	fgets(snd.id,sizeof(snd.id),stdin);
	snd.id[strlen(snd.id)-1]=0;
	printf("请输入密码>>>");
	fgets(snd.password,sizeof(snd.password),stdin);
	snd.password[strlen(snd.password)-1]=0;
    snd.flag = 0;
	snd.vip=0;
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	printf("%c %s %s\n",snd.sign,snd.id,snd.password);
	if(res = recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign == 'R')
	{
		printf("注册成功\n");
	}
	else if(rcv.sign == 'F')
	{
		printf("注册失败，该用户名已经被注册\n");
	}
}	
int do_manager_register(int sfd,struct sockaddr_in sin)
{
	pack snd;
	pack rcv;
	snd.sign='M';
	printf("请输入账号>>>");
	fgets(snd.id,sizeof(snd.id),stdin);
	snd.id[strlen(snd.id)-1]=0;
	printf("请输入密码>>>");
	fgets(snd.password,sizeof(snd.password),stdin);
	snd.password[strlen(snd.password)-1]=0;
    snd.flag = 0;
	snd.vip=1;
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	printf("%c %s %s\n",snd.sign,snd.id,snd.password);
	if(res = recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign == 'R')
	{
		printf("注册成功\n");
	}
	else if(rcv.sign == 'F')
	{
		printf("注册失败，该用户名已经被注册\n");
	}
}
int do_staff_login(int sfd,struct sockaddr_in sin)
{
    int ret;
    pack snd;
    pack rcv;
    memset(&snd,0,sizeof(snd));
    memset(&rcv,0,sizeof(rcv));
    snd.sign='L';
//  输入账号密码登录发给服务器
    printf("请输入账号>>>");
    fgets(snd.id,sizeof(snd.id),stdin);
    snd.id[strlen(snd.id)-1]=0;
    printf("请输入密码>>>");
    fgets(snd.password,sizeof(snd.password),stdin);
    snd.password[strlen(snd.password)-1]=0;
    if(send(sfd,&snd,sizeof(snd),0)<0)
    {
        ERR_MSG("send");
        return -1;
    }
//接收服务器的判定
    ssize_t res=0;
    if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
    {
        ERR_MSG("recv");
        return -1;
    }
    if(rcv.sign=='R')
    {
        printf("登录成功\n");
        do_staff_function(sfd,sin,snd);
	}
	if(rcv.sign=='F'&&rcv.flag==0)
	{
		printf("登录失败:账号未注册或密码错误!\n");
	}
	if(rcv.sign=='F'&&rcv.flag==1)
	{
		printf("登录失败:账号已在别处登录\n");
	}
}
int do_manager_login(int sfd,struct sockaddr_in sin)
{
    int ret;
    pack snd;
    pack rcv;
    memset(&snd,0,sizeof(snd));
    memset(&rcv,0,sizeof(rcv));
    snd.sign='L';
//  输入账号密码登录发给服务器
    printf("请输入账号>>>");
    fgets(snd.id,sizeof(snd.id),stdin);
    snd.id[strlen(snd.id)-1]=0;
    printf("请输入密码>>>");
    fgets(snd.password,sizeof(snd.password),stdin);
    snd.password[strlen(snd.password)-1]=0;
    if(send(sfd,&snd,sizeof(snd),0)<0)
    {
        ERR_MSG("send");
        return -1;
    }
//接收服务器的判定
    ssize_t res=0;
    if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
    {
        ERR_MSG("recv");
        return -1;
    }
    if(rcv.sign=='R')
    {
        printf("登录成功\n");
        do_manager_function(sfd,sin,snd);
	}
	if(rcv.sign=='F'&&rcv.flag==0)
	{
		printf("登录失败:账号未注册或密码错误!\n");
	}
	if(rcv.sign=='F'&&rcv.flag==1)
	{
		printf("登录失败:账号已在别处登录\n");
	}
}
int do_manager_function(int sfd,struct sockaddr_in sin,struct pack snd)
{
	char choose=0;
	while(1)
	{
		printf("******************\n");
		printf("**1.增加员工信息**\n");
		printf("**2.删除员工信息**\n");
		printf("**3.修改员工信息**\n");
		printf("**4.查找员工信息**\n");
		printf("******5.下线******\n");
		printf("******************\n");
		printf("请输入选择>>>");
        choose=getchar();
        while(getchar()!=10);
        switch(choose)
        {
        case'1':
            do_add(sfd,sin,snd);
            break;
        case '2':
            do_delete(sfd,sin,snd);                                     
            break;
        case'3':
            do_change(sfd,sin,snd);
            break;    
        case'4':
            do_select(sfd,sin,snd);
            break;  
		case'5':
			do_quit(sfd,sin,snd);
			return 0;
        default:
            printf("输入错误\n");
        }
	}
}
int do_add(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	snd.sign='A';

	printf("请输入id>>>");
	fgets(snd.id,sizeof(snd.id),stdin);
	snd.id[strlen(snd.id)-1]=0;

	printf("请输入密码>>>");
	fgets(snd.password,sizeof(snd.password),stdin);
	snd.password[strlen(snd.password)-1]=0;

	printf("请输入姓名>>>");
	fgets(snd.name,sizeof(snd.name),stdin);
	snd.name[strlen(snd.name)-1]=0;

	printf("请输入手机号>>>");
	fgets(snd.phone,sizeof(snd.phone),stdin);
	snd.phone[strlen(snd.phone)-1]=0;

	printf("请输入地址>>>");
	fgets(snd.address,sizeof(snd.address),stdin);
	snd.address[strlen(snd.address)-1]=0;

	printf("请输入薪资>>>");
	scanf("%d",&snd.pay);

	printf("请输入年龄>>>");
	scanf("%d",&snd.age);

	snd.vip=0;
	snd.flag=0;
	ssize_t res=0;
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='T')
	{
		printf("员工信息增加成功\n");
	}
	else if(rcv.sign=='F')
	{
		printf("员工信息增加失败\n");
	}

	return 0;
}
int do_delete(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	snd.sign='D';

	printf("请输入要删除的员工id>>>");
	fgets(snd.id,sizeof(snd.id),stdin);
	snd.id[strlen(snd.id)-1]=0;

	ssize_t res=0;
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='T')
	{
		printf("员工信息删除成功\n");
	}
	else if(rcv.sign=='F')
	{
		printf("员工信息删除失败\n");
	}
	return 0;
}
int do_change(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	int choose;
	printf("请输入要修改的员工id>>>");
	fgets(snd.id,sizeof(snd.id),stdin);
	snd.id[strlen(snd.id)-1]=0;
	while(1)
	{
		printf("**********************\n");
		printf("***1.修改该员工姓名***\n");
		printf("***2.修改该员工密码***\n");
		printf("***3.修改该员工电话***\n");
		printf("***4.修改该员工地址***\n");
		printf("***5.修改该员工薪资***\n");
		printf("***6.修改该员工年龄***\n");
		printf("*****7.返回上一级*****\n");
		printf("**********************\n");
		printf("请输入选择>>>");
		choose=getchar();
		while(getchar()!=10);

		switch(choose)
		{
		case'1':
			do_change_name(sfd,sin,snd);
			break;
		case'2':
			do_change_password(sfd,sin,snd);
			break;
		case'3':
			do_change_phone(sfd,sin,snd);
			break;
		case'4':
			do_change_address(sfd,sin,snd);
			break;
		case'5':
			do_change_pay(sfd,sin,snd);
	     	break;
		case'6':
			do_change_age(sfd,sin,snd);
			break;
		case'7':
			do_manager_function(sfd,sin,snd);
			return 0;
		default:
			printf("输入错误\n");
		}
	}
	return 0;
}
int do_change_name(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	printf("请输入修改后的姓名>>>");
	fgets(snd.name,sizeof(snd.name),stdin);
	snd.name[strlen(snd.name)-1]=0;
	snd.sign='N';
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='R')
	{
		printf("修改名字成功\n");
	}
	if(rcv.sign=='F')
	{
		printf("修改名字失败\n");
	}
	return 0;
}
int do_change_password(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	printf("请输入修改后的密码>>>");
	fgets(snd.password,sizeof(snd.password),stdin);
	snd.password[strlen(snd.password)-1]=0;
	snd.sign='P';
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='R')
	{
		printf("修改密码成功\n");
	}
	if(rcv.sign=='F')
	{
		printf("修改密码失败\n");
	}
	return 0;
}
int do_change_phone(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	printf("请输入修改后的电话>>>");
	fgets(snd.phone,sizeof(snd.phone),stdin);
	snd.phone[strlen(snd.phone)-1]=0;
	snd.sign='H';
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='R')
	{
		printf("修改电话成功\n");
	}
	if(rcv.sign=='F')
	{
		printf("修改电话失败\n");
	}
	return 0;
}
int do_change_address(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	printf("请输入修改后的地址>>>");
	fgets(snd.address,sizeof(snd.address),stdin);
	snd.address[strlen(snd.address)-1]=0;
	snd.sign='Z';
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='R')
	{
		printf("修改地址成功\n");
	}
	if(rcv.sign=='F')
	{
		printf("修改地址失败\n");
	}
	return 0;
}
int do_change_pay(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	printf("请输入修改后的薪资>>>");
	scanf("%d",&snd.pay);
	snd.sign='X';
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='R')
	{
		printf("修改薪资成功\n");
	}
	if(rcv.sign=='F')
	{
		printf("修改薪资失败\n");
	}
	return 0;
}
int do_change_age(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	printf("请输入修改后的年龄>>>");
	scanf("%d",&snd.age);
	snd.sign='G';
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	ssize_t res=0;
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='R')
	{
		printf("修改年龄成功\n");
	}
	if(rcv.sign=='F')
	{
		printf("修改年龄失败\n");
	}
	return 0;
}
int do_select(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	snd.sign='S';

	printf("请输入要查找的员工id>>>");
	fgets(snd.id,sizeof(snd.id),stdin);
	snd.id[strlen(snd.id)-1]=0;

	ssize_t res=0;
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='T')
	{
		printf("员工信息查找成功\n");
	}
	else if(rcv.sign=='F')
	{
		printf("员工信息查找失败\n");
		return 0;
	}

	char buf[N];
	memset(buf,0,sizeof(buf));
	if(recv(sfd,buf,sizeof(buf),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	printf("%s\n",buf);
	return 0;
}
int do_quit(int sfd,struct sockaddr_in sin,struct pack snd)
{
    pack rcv;
    memset(&rcv,0,sizeof(rcv));
    snd.sign='Q';
    if(send(sfd,&snd,sizeof(snd),0)<0)
    {
        ERR_MSG("send");
        return -1;
    }
    if(recv(sfd,&rcv,sizeof(rcv),0)<0)
    {
        ERR_MSG("recv");
        return -1;
    }
    if(rcv.sign=='R')
    {
        printf("下线成功\n");
    }
    else if(rcv.sign=='F')
    {
        printf("下线失败\n");
        return -1;
    }
    return 0;
}
int do_staff_function(int sfd,struct sockaddr_in sin,struct pack snd)
{
	char choose=0;
	while(1)
	{
		printf("******************\n");
		printf("**1.查看个人信息**\n");
		printf("**2.修改个人信息**\n");
		printf("******3.下线******\n");
		printf("******************\n");
		printf("请输入选择>>>");
        choose=getchar();
        while(getchar()!=10);
        switch(choose)
        {
        case'1':
            do_staff_select(sfd,sin,snd);
            break;
        case'2':
            do_staff_change(sfd,sin,snd);                                     
            break;
  		case'3':
			do_quit(sfd,sin,snd);
			return 0;
        default:
            printf("输入错误\n");
        }
	}
}
int do_staff_select(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	snd.sign='S';

	ssize_t res=0;
	if(send(sfd,&snd,sizeof(snd),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	if(res=recv(sfd,&rcv,sizeof(rcv),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	if(rcv.sign=='R')
	{
		printf("员工信息查找成功\n");
	}
	else if(rcv.sign=='F')
	{
		printf("员工信息查找失败\n");
		return 0;
	}

	char buf[N];
	memset(buf,0,sizeof(buf));
	if(recv(sfd,buf,sizeof(buf),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
	printf("%s\n",buf);
	return 0;
}
int do_staff_change(int sfd,struct sockaddr_in sin, struct pack snd)
{
	pack rcv;
	int choose;
	while(1)
	{
		printf("**********************\n");
		printf("****1.修改个人姓名****\n");
		printf("****2.修改个人密码****\n");
		printf("****3.修改个人电话****\n");
		printf("****4.修改个人地址****\n");
		printf("*****5.返回上一级*****\n");
		printf("**********************\n");
		printf("请输入选择>>>");
		choose=getchar();
		while(getchar()!=10);

		switch(choose)
		{
		case'1':
			do_change_name(sfd,sin,snd);
			break;
		case'2':
			do_change_password(sfd,sin,snd);
			break;
		case'3':
			do_change_phone(sfd,sin,snd);
			break;
		case'4':
			do_change_address(sfd,sin,snd);
			break;
		case'5':
			do_staff_function(sfd,sin,snd);
			return 0;
		default:
			printf("输入错误\n");
		}
	}
	return 0;
}


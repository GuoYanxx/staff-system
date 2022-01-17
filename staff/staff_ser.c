#include "staff.h"

#define PORT 8888
#define IP "192.168.1.128"

void handler(int sig);

int main(int argc, const char *argv[])
{
	sqlite3* db = NULL;
	if(sqlite3_open("staff.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"sqlite3_open:%s\n", sqlite3_errmsg(db));
		return -1;
	}
	printf("%p\n",db);

	//创建一张表格
	char arr[N]= "create table if not exists staff (id int primary key,password char,vip int,flag int,name char,phone char,address char,pay int,age int)";
	char *errmsg = NULL;
	if(sqlite3_exec(db,arr,NULL,NULL,&errmsg)!=0)
	{
		printf("sqlite3_exec:%s  __%d__\n",errmsg,__LINE__);
		return -1;
	}
	//捕获17信号
	signal(SIGCHLD,handler);

	int sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
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
	if(bind(sfd,(struct sockaddr*)&sin,sizeof(sin))<0)
	{
		ERR_MSG("bind");
		return -1;
	}
	if(listen(sfd,10)<0)
	{
		ERR_MSG("listen");
		return -1;
	}
	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);
	int newfd = 0;

	while(1)
	{
		pid_t pid = fork();
		printf("pid %d\n",pid);
		if(pid>0)
		{
			newfd = accept(sfd,(struct sockaddr*)&cin, &addrlen);
			if(newfd < 0)
			{
				ERR_MSG("accept");
				return -1;
			}
			printf("%d连接成功\n",newfd);
		}
		else if(pid==0)
		{
			do_recv(db,sfd,cin,newfd);
			exit(0);
		}
		else
		{
			ERR_MSG("fork");
			return -1;
		}
	}
	close(newfd);
    if(sqlite3_close(db)!=0)
	{
		printf("关闭数据库失败\n");
		return -1;
	}
	printf("成功关闭数据库\n");
	return 0;
}
void handler(int sig)
{
	while(waitpid(-1,NULL,WNOHANG)>0);
}

int do_recv(sqlite3* db,int sfd,struct sockaddr_in cin,int newfd)
{
	ssize_t res=0;
	pack rcv;
	while(1)
	{
		memset(&rcv,0,sizeof(rcv));
		if((res=recv(newfd,&rcv,sizeof(rcv),0))<0)
		{
			if(res != -1)
				ERR_MSG("recv");
			return -1;
		}
		else if(0==res)
		{
			printf("newfd %d客户端已关闭\n",newfd);
			break;
		}
		printf("sign :%c\n",rcv.sign);
		switch(rcv.sign)
		{
		case'R':
			do_staff_register_1(sfd,db,rcv,newfd);
			break;
		case'L':
			do_login(sfd,db,rcv,newfd);
			break;
		case'M':
			do_manager_register_1(sfd,db,rcv,newfd);
			break;
		case'A':
			do_add_1(sfd,db,rcv,newfd);
			break;
		case'D':
			do_delete_1(sfd,db,rcv,newfd);
			break;
		case'N':
			do_change_name_1(sfd,db,rcv,newfd);
			break;
		case'P':
			do_change_password_1(sfd,db,rcv,newfd);
			break;
		case'H':
			do_change_phone_1(sfd,db,rcv,newfd);
			break;
		case'Z':
			do_change_address_1(sfd,db,rcv,newfd);
			break;
		case'X':
			do_change_pay_1(sfd,db,rcv,newfd);
			break;
		case'G':
			do_change_age_1(sfd,db,rcv,newfd);
			break;
		case'S':
			do_select_1(sfd,db,rcv,newfd);
			break;
		case'Q':
			do_quit_1(sfd,db,rcv,newfd);
			break;
		default:
			continue;
		}
	}
	return 0;
}
int do_staff_register_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
    int row,column = 0;
    char**result=NULL;
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    sprintf(sql,"select *from staff where id=\"%s\";",rcv.id);
    ret = sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
    printf("row:%d\n",row);
	if(ret==0&&row==0)
	{
		sprintf(sql,"insert into staff(id, password, vip, flag) values(\"%s\",\"%s\",%d,%d)",rcv.id,rcv.password,rcv.vip,rcv.flag);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		snd.sign = 'R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("注册成功\n");
	}
	else
	{
		printf("注册失败\n");
		snd.sign='F';
		snd.flag=0;
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
	sqlite3_free_table(result);
	return 0;
}
int do_manager_register_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
    int row,column = 0;
    char**result=NULL;
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    sprintf(sql,"select *from staff where id=\"%s\";",rcv.id);
    ret = sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
    printf("row:%d\n",row);
	printf("vip:%d\n",rcv.vip);
	if(ret==0&&row==0)
	{
		sprintf(sql,"insert into staff(id, password, vip, flag) values(\"%s\",\"%s\",\"%d\",%d)",rcv.id,rcv.password,rcv.vip,rcv.flag);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		snd.sign = 'R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("注册成功\n");
	}
	else
	{
		printf("注册失败\n");
		snd.sign='F';
		snd.flag=0;
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
	sqlite3_free_table(result);
	return 0;
}
int do_login(int sfd,sqlite3* db,pack rcv,int newfd)
{

    pack snd;
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;

    //查找登录人是否存在
    sprintf(sql,"select *from staff where id =\"%s\";",rcv.id);
    ret =sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
    if(ret==0)
    {
        if(row==1)//用户存在
        {
            //比对账号密码和是否已登录
            if((strcmp(result[9],rcv.id)==0)&&(strcmp(result[10],rcv.password)==0))
            {
                printf("result:%s\n",result[12]);
                if(atoi(result[12])==0)
                {
                    printf("result[12]:%s\n",result[12]);
                    sprintf(sql,"update staff set flag=1 where id =\"%s\";",rcv.id);
                    sqlite3_exec(db,sql,NULL,NULL,&errmsg);
                    snd.sign='R';                                                                            
                    snd.flag = 1;
                    printf("登录成功\n");
                }
                else
                {
                    snd.flag=1;
                    snd.sign ='F';
                }
           }
            else
            {                                                           
                snd.sign='F';
            }
        }
    }
    else
    {
        snd.sign='F';
    }
    if(send(newfd,&snd,sizeof(snd),0)<0)
    {
        ERR_MSG("send");
        return -1;
    }
    sqlite3_free_table(result);
    return 0;
}
int do_add_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
	pack snd;
	memset(&snd,0,sizeof(snd));
	char* errmsg=NULL;
	char** result = NULL;
	char sql[10*N]="";
	int row = 0,column = 0;
	int ret = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==0)
	{
		sprintf(sql,"insert into staff values(\"%s\",\"%s\",%d,%d,\"%s\",\"%s\",\"%s\",%d,%d)",
				rcv.id,rcv.password,rcv.vip,rcv.flag,rcv.name,rcv.phone,rcv.address,rcv.pay,rcv.age);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='T';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("添加成功\n");
	}
	else
	{
		printf("添加失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
	sqlite3_free_table(result);
    return 0;
}
int do_delete_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==1)
	{
		sprintf(sql,"delete from staff where id=\"%s\";",rcv.id);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='T';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("删除成功\n");
	}
	else
	{
		printf("删除失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
int do_change_name_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==1)
	{
		sprintf(sql,"update staff set name =\"%s\"  where id=\"%s\";",rcv.name,rcv.id);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("修改姓名成功\n");
	}
	else
	{
		printf("修改姓名失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
int do_change_password_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==1)
	{
		sprintf(sql,"update staff set password =\"%s\"  where id=\"%s\";",rcv.password,rcv.id);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("修改密码成功\n");
	}
	else
	{
		printf("修改密码失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
int do_change_phone_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==1)
	{
		sprintf(sql,"update staff set phone =\"%s\"  where id=\"%s\";",rcv.phone,rcv.id);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("修改电话成功\n");
	}
	else
	{
		printf("修改电话失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
int do_change_address_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==1)
	{
		sprintf(sql,"update staff set address =\"%s\"  where id=\"%s\";",rcv.address,rcv.id);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("修改地址成功\n");
	}
	else
	{
		printf("修改地址失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
int do_change_pay_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
	printf("xx\n");
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==1)
	{
		sprintf(sql,"update staff set pay =%d  where id=\"%s\";",rcv.pay,rcv.id);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("修改薪资成功\n");
	}
	else
	{
		printf("修改薪资失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
int do_change_age_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0&&row==1)
	{
		sprintf(sql,"update staff set age =%d  where id=\"%s\";",rcv.age,rcv.id);
		ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
		if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
		snd.sign='R';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
		printf("修改年龄成功\n");
	}
	else
	{
		printf("修改年龄失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
}
int do_select_1(int sfd,sqlite3* db,pack rcv,int newfd)
{
    pack snd;
	memset(&snd,0,sizeof(snd));
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
	char str[N];
    int row = 0,column = 0;
	sprintf(sql,"select * from staff where id=\"%s\";",rcv.id);
	ret=sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
	printf("row:%d\n",row);
	if(ret==0)
	{
		if(row==1)
		{
			sprintf(sql,"select *  from staff where id=\"%s\";",rcv.id);
			ret=sqlite3_exec(db,sql,NULL,NULL,&errmsg);
			if(ret!=SQLITE_OK)
			printf("%s\n",errmsg);
			snd.sign='R';
			if(send(newfd,&snd,sizeof(snd),0)<0)
			{
				ERR_MSG("send");
				return -1;
			}
			printf("查找成功\n");
			bzero(str,sizeof(str));
		    sprintf(str,"%s\t%s\t%s\t%s\t%s\t%s\t%s\n%s\t%s\t\t%s\t%s\t%s\t%s\t%s\t",
					result[0],result[1],result[4],result[5],result[6],result[7],result[8],result[9],result[10],result[13],result[14],result[15],result[16],result[17]);
			if(send(newfd,str,sizeof(str),0)<0)
			{
				ERR_MSG("send");
				return -1;
			}
		}
		else
		{
			printf("查找失败\n");
			snd.sign='F';
			if(send(newfd,&snd,sizeof(snd),0)<0)
			{
				ERR_MSG("send");
				return -1;
			}
		}
	}
	else
	{
		printf("查找失败\n");
		snd.sign='F';
		if(send(newfd,&snd,sizeof(snd),0)<0)
		{
			ERR_MSG("send");
			return -1;
		}
	}
	return 0;
}
int do_quit_1(int sfd,sqlite3*db,struct pack rcv,int newfd)
{
    pack snd;
    memset(&snd,0,sizeof(snd));
    ssize_t res=0;
    char* errmsg=NULL;
    char sql[3*N]="";
    int ret = 0;
    char** result=NULL;
    int row = 0,column = 0;
    sprintf(sql,"select * from staff where id =\"%s\";",rcv.id);
    ret =sqlite3_get_table(db,sql,&result,&row,&column,&errmsg);
    if(ret==0)
    {   
        printf("row:%d\n",row);
          if(row==1)//用户存在
        {
            sprintf(sql,"update staff set flag=0 where id =\"%s\";",rcv.id);
            sqlite3_exec(db,sql,NULL,NULL,&errmsg);
            snd.sign='R';
            snd.flag = 0;
        }
    }                                                                                      
    else
    {
        snd.sign='F';
    }   
    if(send(newfd,&snd,sizeof(snd),0)<0)
    {
        ERR_MSG("recv");
        return -1;
	}
	return 0;
}

 

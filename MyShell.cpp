using namespace std;
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include<vector>
#include <sys/stat.h>
#include <fcntl.h>
#include<algorithm>
string globalpath;
int gflag=0;
void signalHandler( int signum )
{
	cout<<"\n";
}
const char* implementbang(string cmdi)
{
	int numi;

	char cmd[1024];
   	//char *cmd1[10000];
   	char buff[1];
   	int j,count;
   	string substring="";
   	vector<string> v;

    string hpath(getenv("HOME"));
    hpath=hpath+"/History.txt";
  	int fd=open(hpath.c_str(),O_RDONLY);
  	count=0;
  	string str;
  	while(read(fd,buff,1))
  	{
  		j=0;
  		while(buff[0]!='\n')
  		{
  			cmd[j]=buff[0];
  			read(fd,buff,1);
  			j++;
  		}
  		cmd[j]='\0';
  		//cmd1[count]=cmd;
  		v.push_back(cmd);
  		count++;
  	}
	if(cmdi[1]!='\0' && cmdi[1]>='0' && cmdi[1]<='9')
	{
		string num=cmdi.substr(1,cmdi.length()-1);
		numi=atoi(num.c_str());
		str=v[numi-1];
	}
	else
	if(cmdi[1]!='\0' && cmdi[1]=='-')
	{
		string num=cmdi.substr(2,cmdi.length()-2);
		numi=atoi(num.c_str());
		reverse(v.begin(),v.end());
		str=v[numi-1];

	}
	else
	if(cmdi[1]!='\0' && cmdi[1]=='!' && cmdi.length()==2)
	{
		numi=1;
		reverse(v.begin(),v.end());
		v.resize(numi);
		str=v[numi-1];

	}
	else
	if(cmdi[1]!='\0' && cmdi.length()>1)
	{
		substring=cmdi.substr(1,cmdi.length()-1);
		reverse(v.begin(),v.end());
		for(vector<string>::iterator it=v.begin();it!=v.end();it++)
  		{
  			string match=*it;
  			if(match.length()>=(cmdi.length()-1))
  			if(match.substr(0,cmdi.length()-1)==substring)
  			{
  				str=match;
  				break;
  			}

  		}

	}
	cout <<str<<endl;
	const char *brr=str.c_str();
	return brr;
}

int parser(string cmd[])
{
	
	string command;
	int squoteflg,dquoteflg,k,start,pipe1;
	
	int i;
	char cwd[1024];

    getcwd(cwd, sizeof(cwd));

	cout << "MyShell :"<<cwd<<"$ ";

	getline(cin,command);

	ofstream outfile1;
	string hpath(getenv("HOME"));
    hpath=hpath+"/History.txt";
  
    outfile1.open(hpath.c_str(), std::ios_base::app);
    if(command[0]!='!')
			outfile1 <<command<<"\n"; //write command to history file

	const char* barr;
	if(command[0]=='!')
	{
		barr=implementbang(command);
		command=barr;
	}
    
		
	dquoteflg=0; squoteflg=0; 
	start=0;
	k=0;
	pipe1=0;
	int flg=0;
	for(i=0;i<(int)command.length();i++)
	{
		
		if(command[i]=='\'' && squoteflg==0)
			squoteflg=1;
		else
		if(command[i]=='\'' && squoteflg==1)
			squoteflg=0;
		else
		if(command[i]=='\"' && dquoteflg==0)
			dquoteflg=1;
		else
		if(command[i]=='\"' && dquoteflg==1)
			dquoteflg=0;

		if(command[i]=='|' && dquoteflg==0 && squoteflg==0 )
		{
			
			cmd[k]=command.substr(start,(i-start));

			start=i+1;
			k++;
			pipe1=i;
			flg=1;
		}

	}
	if(pipe1!=0)
	{
		if(pipe1!=command.length()-1)
			cmd[k]=command.substr(pipe1+1,(command.length()-(pipe1+1)));
		gflag=1;
			
	}
	if(flg==0)
	{
		cmd[0]=command;
		
	}
	return k;

}
void implementcd(char *cmdArr[],string cmd)
{
	if(gflag!=1)
	{
	char *path;
	string path1;
	int sp=0,i;
	for(i=0;i<(int)cmd.length();i++)
	{
		if(sp==1)
		{
			path1=cmd.substr(i,(int)cmd.length()-i);
			break;
		}
		if(cmd[i]==' ')
			sp++;

	}
	
  	if (path1.c_str() == NULL) 
  	{
    	fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  	} 
  	else 
  	if((strcmp(path1.c_str(),"~")==0) || (strcmp(path1.c_str(),"#")==0) || (strcmp(path1.c_str(),"--")==0) )
  	{
  		 path=getenv("HOME");
  		 string gpath(getenv("HOME"));
  		 char cwd[1024];
    	 getcwd(cwd, sizeof(cwd));
    	 string str(cwd);
  		 globalpath=gpath;
  		 if (chdir(path) != 0) 
    	 {
     		 perror("error");
    	 }

  	}
  	else
  	if(strcmp(path1.c_str(),"-")==0)
  	{
  		char cwd[1024];
    	 getcwd(cwd, sizeof(cwd));
    	 string str(cwd);

  		  cout<<globalpath<<"\n";
  		 if (chdir(globalpath.c_str()) != 0) 
    	 {
     		 perror("error");
    	 }
    	 
  		 globalpath=str;
  	}
  	else
  	{
  		 char cwd[1024];
    	 getcwd(cwd, sizeof(cwd));
    	 string str(cwd);
  		 globalpath=str;

  		if(path1!="..")
  		 globalpath=str;
    if (chdir(path1.c_str()) != 0) 
    {
      perror("error");
    }
	}
	}
}
void implementexport(char *cmdArr[],string cmd,int i)
{
		char *arr,*arr1[2];
		int index=0;
		arr=strtok(cmdArr[i+1],"=");
		while(arr!=NULL)
		{
			arr1[index]=arr;
			arr=strtok(NULL," ");
			index++;
		}
		setenv(arr1[0],arr1[1],1); 

}
void implementpwd(int in,int fd1)
{
	char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    if(fd1!=1)
    {
    	FILE* file=fdopen(fd1,"w");
    	fprintf(file,"%s\n",cwd);
    	fclose(file);
    }
    else
    cout <<cwd<<endl;

}
void implementecho(char *cmdArr[],int i,int in,int fd1,string cmd)
{
	int k=0,index=0;
	int dqflg=0,sqflg=0,sd=0,ed=0,ss=0,es=0,slashflg=0;
	char output[2048];
	bzero(output,2048);
	char *env1=NULL;
	int j=0;
	while(cmd[k]==' ')
		k++;
	while(cmd[k]!='\0' && cmd[k]==cmdArr[0][j])
	{
		k++;
		j++;
	}

	for(k=k;k<(int)cmd.length();k++)
	{
		if(cmd[k]=='\"' && sqflg!=1)
		{
			if(dqflg==0)
				dqflg=1;
			else
				dqflg=0;
		}
		else
		if(cmd[k]=='\'' && dqflg!=1)
		{
			if(sqflg==0)
				sqflg=1;
			else
				sqflg=0;
		}
		else
		if(cmd[k+1]!='\0' && cmd[k]=='\\')
		{
			output[index]=cmd[k+1];
			k=k+1;
			index++;
		}
		else
		if(cmd[k]==' ' && (dqflg==1 || sqflg==1) )
		{
			output[index]=cmd[k];
			index++;
		}
		else
		if((cmd[k]=='&' || cmd[k]=='|' || cmd[k]=='<' || cmd[k]=='>') )
		{
			if((dqflg==1 || sqflg==1))
			{
				output[index]=cmd[k];
				index++;
			}
		}
		else
		if(cmd[k]=='$' && sqflg==0 && dqflg==0)
		{
			int s=k+1;
			while(cmd[k]!='\0' && cmd[k]!=' ')
				k++;
			 string env=cmd.substr(s,k-s);
			 env1=getenv(env.c_str());

		}
		else
		{
			output[index]=cmd[k];
			index++;
		}
	}

	if(fd1!=1)
	{
		FILE* file=fdopen(fd1,"w");
		if(dqflg==0 && sqflg==0)
    		fprintf(file,"%s\n",output);
    	else
    		fprintf(file,"%s\n",env1);
    	fclose(file);
	}
	else
	{
	if(dqflg==0 && sqflg==0)
		cout << output;
	if(env1!=NULL)
		cout<<env1;
	cout<<endl;
	}
}

void implementhistory(char *cmdArr[],int i,int in,int fd1)
{
   	char cmd[1024];
   	char *cmd1[2000];
   	char buff[1];
   	int j,count;
   	vector<string> v;

    string hpath(getenv("HOME"));
    hpath=hpath+"/History.txt";
  	int fd=open(hpath.c_str(),O_RDONLY);
  	count=0;
  	while(read(fd,buff,1))
  	{
  		j=0;
  		while(buff[0]!='\n')
  		{
  			cmd[j]=buff[0];
  			read(fd,buff,1);
  			j++;
  		}
  		cmd[j]='\0';
  		cmd1[count]=cmd;
  		v.push_back(cmd);
  		count++;
  	}
  
  	int val=0,size=0;
  	if(cmdArr[i+1]!=NULL)
  	{
  		size=v.size();	
  		val=atoi(cmdArr[i+1]);
  	}
  	if(fd1!=1)
  	{
  		FILE* file=fdopen(fd1,"w");
  		for(j=size-val;j<(int)v.size();j++)
  		{
    		fprintf(file,"%s\n",cmd1[j]);
  		}
  		fclose(file);

  	}
  	else
  	{
  		j=size-val+1;
  		for(vector<string>::iterator it=v.begin()+size-val;it!=v.end();it++)
  		{
  		cout <<j<<" ";
  		cout <<*it<<endl;
  				j++;
  		}
  	}
}
int checkSellBuildins(char *cmdArr[],string cmd,int i,int in,int fd1)
{
	int flg=0;
	//char *c="cd";
	if(cmdArr[0]!='\0')
	{
	if(strcmp(cmdArr[0],"cd")==0) 
	{
		implementcd(cmdArr,cmd);
		flg=1;
	}
	if(strcmp(cmdArr[0],"pwd")==0) 
	{
		implementpwd(in,fd1);
		flg=1;
	}
	if(strcmp(cmdArr[0],"export")==0) 
	{
		implementexport(cmdArr,cmd,i);
		flg=1;
	}
	if(strcmp(cmdArr[0],"history")==0) 
	{
		implementhistory(cmdArr,i,in,fd1);
		flg=1;
	}
	if(strcmp(cmdArr[0],"echo")==0) 
	{
		implementecho(cmdArr,i,in,fd1,cmd);
		flg=1;
	}
	}
	return flg;

}
int executechild (int in, int out ,char * cmdArr[])
{
  	pid_t pid;
  	
  		if((pid=fork())==0)
    	{
      		if(in!=0)
       		{
          		dup2(in,0);
          		close(in);
        	}

      		if(out!=1)
        	{
          		dup2(out,1);
          		close(out);
        	}
      		return execvp (cmdArr[0], cmdArr);
         }
    
  return pid;
}
void execute(string cmd[], int k)
{
	
	char *cmdArr[500];
	int i,index=0;
	char temp[500];
	char *arr;
	int in, fd [2];
	in=0;
	vector<string> v;
	
	char fileo[1024];
	
	int sq=0,dq=0;


	for(i=0;i<=k;i++)  // for each command
	{
		index=0;	
		string str=cmd[i];
		
		string cmdtemp=cmd[i];
		char cmdtemparr[1024];
		int count=0,fpi=0,fpo=0,ncount=0;;
		char filei[1024];
		int sq=0,dq=0;

		bzero(filei,1024);
		bzero(fileo,1024);
		bzero(cmdtemparr,1024);


		while(cmdtemp[count]!='\0') // check if ioredirection is there or not
		{
			if(cmdtemp[count]=='\"')
			{
				if(dq==0)
					dq=1;
				else
					dq=0;
				cmdtemparr[ncount]=cmdtemp[count];
				count++;
				ncount++;
			}
			else
			if(cmdtemp[count]=='\'')
			{
				if(sq==0)
					sq=1;
				else
					sq=0;
				cmdtemparr[ncount]=cmdtemp[count];
				count++;
				ncount++;
			}
			else 
			if(cmdtemp[count]=='<' && dq==0 && sq==0)
			{
				count++;
				while(cmdtemp[count]!=' ' && count<cmdtemp.length())
				{
					filei[fpi]=cmdtemp[count];
					count++;
					fpi++;
				}
				count++;

			}
			else
			if(cmdtemp[count]=='>' && dq==0 && sq==0)
			{

				count++;
				while(cmdtemp[count]!=' ' && count<cmdtemp.length())
				{
					fileo[fpo]=cmdtemp[count];
					count++;
					fpo++;
				}

			}
			else
			{
				cmdtemparr[ncount]=cmdtemp[count];
				count++;
				ncount++;
			}
			

		}
		string stemp(cmdtemparr);
		cmd[i]=stemp;


		bzero(temp,500);
		strncpy(temp, cmd[i].c_str(), sizeof(temp));
		
		arr=strtok(temp," ");
		while(arr!=NULL)
		{
			cmdArr[index]=arr;
			arr=strtok(NULL," ");
			index++;
		}
		cmdArr[index]=NULL;

		
		//removing quotes
		string qstr="";
		int qcount=0;
		if(cmdArr[0]!='\0' &&  strcmp(cmdArr[0],"echo")!=0)
		{
			//cout <<"cmdArr="<<cmdArr[0];
			while(cmd[i][qcount]!='\0')
			{
				if(cmd[i][qcount]!='\'' && cmd[i][qcount]!='\"')
				{
					qstr=qstr+cmd[i][qcount];
				}
				qcount++;

			}
			index=0;
			cmd[i]=qstr;
			//cout <<qstr;
			bzero(temp,500);
			strncpy(temp, cmd[i].c_str(), sizeof(temp));
		
			arr=strtok(temp," ");
			while(arr!=NULL)
			{
				cmdArr[index]=arr;
				arr=strtok(NULL," ");
				index++;
			}
			cmdArr[index]=NULL;
		}
		

		//piping
		if(i<k)
		{

     		pipe (fd);

     		string sfpi(filei);    // io redirection of input
			if(sfpi!="")
				in=open(sfpi.c_str(),O_RDONLY);

      		
			string sfpo(fileo);  // io redirection of output
			if(sfpo!="")
				fd[1]=open(sfpo.c_str(),O_WRONLY | O_CREAT,S_IRWXU);

      		int flg;
			flg=checkSellBuildins(cmdArr,cmd[i],i,in,fd[1]);  //check if command is shell build in
			
			if(flg==0)
			{
      		 executechild(in, fd[1],cmdArr);
      		}
      		close (fd[1]);

      		in = fd [0]; //next child will read from here
      		
      	}
      	
	}

	int flg;

	string sfpo(fileo);
	int fd1=1;
	if(sfpo!="")
		fd1=open(sfpo.c_str(),O_WRONLY | O_CREAT,S_IRWXU);
 	
	flg=checkSellBuildins(cmdArr,cmd[k],k,in,fd1);
	
	if(flg==0)
	{
		int status;
		int pid1=fork();
		if(pid1==0)
		{

			if (in != 0)
 	   		dup2 (in, 0);

 	   		string sfpo(fileo);
			if(sfpo!="")
			{
				fd1=open(sfpo.c_str(),O_WRONLY | O_CREAT,S_IRWXU);
				dup2(fd1,1);
 	   		}
			cmdArr[index]=NULL;
  			int err=execvp (cmdArr[0], cmdArr);
  			if(err==-1)
  			{
  				cout<<"command not found"<<endl;
  			}
  			
  		}
  		else
  		{
  			waitpid(pid1,&status,0); 
  		}
  	}
  	

}
int main()
{
			while(1)
			{

				string cmd[50];
				int k ,i; //total number of commands	

				//signal(SIGSTOP, signalHandler);
				signal(SIGINT, signalHandler);
				
				gflag=0;
				k=parser(cmd); //take input & parse
		
				if(cmd[0]=="exit")
				{
					cout <<"Bye....."<<endl;
					exit(0);
				}
				execute(cmd,k); //execute commands
		
			}
		
	return 0;
}
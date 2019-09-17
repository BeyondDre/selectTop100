#include<iostream>
#include<stdio.h>
#include<fstream>
#include<string>
#include<ctime>
#include<map>
#include<memory>
#include<windows.h>
#include<cstdlib>
#include<algorithm>
using namespace std;

struct comp{
	bool operator()(const int &a,const int &b){
		return (a>b);
	}
};

//产生随机的url.txt文件，这里只产生1.72GB的url.txt文件 
void ProductBigURLFile(){
	string s[400];
	int index=0;
	string ss;
	ifstream fin("0.txt");
	
	while(getline(fin,ss)){
		s[index]=ss;
		index++;
		if(index>=400)break;
	}
	fin.close();
	
	ofstream fout("url.txt");
	int num;
	
	string ch[]={".com",".org",".cn",".gov",".bss",".html",".asp","php"};
	for(int i=0;i<500000;i++){
		srand(time(0));
		for(int j=0;j<256;j++){
			num=rand()%400;
			ss=s[num];
			//ss+=ch[num%8];
			fout<<ss<<endl;
		}
	}
	fout.close();
}

//读取url.txt文件，并分割成n个小文件 
void BigFilePartition(int n,int len=500000){
	ifstream fin;
	ofstream fo[300];
	string s;
	//string prefilename="//nfile//";
	string fout_name=".txt";
	long long step_tatal=0;//记录总行数
	
	fin.open("url.txt");
	
	char ch[10];
	for(int i=1;i<=n;i++){//建立小文件的输出连接流 
		sprintf(ch,"%d",i);
		string temp=ch+fout_name;
		//cout<<temp.c_str()<<endl;
		fo[i].open(temp.c_str());
	}
	
	
	int index=1;
	int flag=0;
	while(1){//把源文件拆分成多个小文件 
		int step1=1;
		while(step1<=len){//每次写入500000行 
			if(getline(fin,s))fo[index]<<s<<endl;
			else{
				flag=1;
				break;
			}
			step1++;
		}
		index++;
		if(flag>0 || index>n)break;
	}
	fin.close();

	
	for(int i=1;i<=n;i++){//关闭对小文件的写入连接 
		fo[i].clear();
		fo[i].close();
	}
}

//对每一个小文件进行排序操作，最后以<URL,count>的形式写入当前小文件里
void nFileSort(int n){
	ifstream fi[300];
	ofstream fo[300];
	string prefilename="//nfile//";
	string sortprefilename="//sorted_nfile//";
	string filename=".txt";
	string filename1="_1.txt";
	char ch[10];
	
	for(int i=1;i<=n;i++){//用map底层的红黑树原理根据URL值进行快速排序 
		sprintf(ch,"%d",i);
		string temp=ch+filename;
		string temp1=ch+filename1;
		fi[i].open(temp.c_str());
		
		
		string ss;
		map<string,int> t;
		while(getline(fi[i],ss)){
			if(t.find(ss)!=t.end()){
				t.find(ss)->second++;//找到重复URL值，则对应的出现次数count++ 
			}
			else t.insert(pair<string,int>(ss,1));
		}
		fi[i].close();
		
		string stemp="***";
		int tt=-1;
		//把内存中排好序的数据以<URL,count>的形式写入当前文件里 
		fo[i].open(temp1.c_str());
		map<string,int>::iterator iter;
		for(iter=t.begin();iter!=t.end();iter++){
			//cout<<iter->first<<" "<<iter->second<<endl;
			fo[i]<<iter->first<<" "<<iter->second<<endl;
		}
		fo[i]<<stemp<<" "<<tt;
		fo[i].close();
	}
}

//n个有序文件按URL值进行多路归并排序，结果以<count,url>保存在result.txt文件中 
void nFileMerge(int n){
	string filename="_1.txt";
	string sortprefilename="//sorted_nfile//";
	ofstream fout("result.txt");
	
	ifstream fi[300];
	char ch1[10];
	
	for(int i=1;i<=n;i++){//建立小文件的输出连接流 
		sprintf(ch1,"%d",i);
		string temp=ch1+filename;
		fi[i].open(temp.c_str());
		//if(fi[i])cout<<"第"<<i<<"个文件开始读取 "<<fi[i]<<endl;
	}
	
	char min_s[30];
	char ch[300][30];//记录从每一个小文件读取的url值 
	int num[300]={0};//记录从每一个小文件读取的url对应的count值 
	int index;
	int tl=0;
	int totalfile=0;
	while(1){
		memset(min_s,'z',30);
		string s1,s2;
		int total=0;
		int flag=0;
		tl++;
		for(int i=1;i<=n;i++){
			if(ch[i][0]=='\0' || num[i]<=0){
				if(strcmp(ch[i],"***")!=0){
					
					fi[i]>>s1>>s2;
					strcpy(ch[i],s1.c_str());
					num[i]=atoi(s2.c_str());
					
					if(num[i]==-1 || strcmp(ch[i],"***")==0){
						totalfile++;
						//cout<<"关闭文件数： "<<totalfile<<endl;
						fi[i].clear();
						fi[i].close();
						//cout<<"文件 "<<i<<" 关闭 "<<fi[i]<<endl;
						//Sleep(1000);
					}
				}
			}
		}
		
		for(int i=1;i<=n;i++){//找到最小URL值 
			//if(ch[i][0]!='\0' && num[i]>0){
			if(num[i]>0 && strcmp(ch[i],"***")!=0){
				if(strcmp(min_s,ch[i])>0){
					strcpy(min_s,ch[i]);
					index=i;
				}
				flag=1;
			}
		}
		
		//cout<<"totalfile : "<<totalfile<<" flag: "<<flag<<" tl: "<<tl<<endl;
		
		if(totalfile>=n || flag==0)break;
		
		if(flag){
			total=num[index];
			num[index]=0; 
			memset(ch[index],'\0',30);
			
			for(int i=1;i<=n;i++){//找到剩下的和最小URL值一样的URL值 
				//if(num[i]>0 && ch[i][0]!='\0'){
				if(num[i]>0 && strcmp(ch[i],"***")!=0){
					if(strcmp(min_s,ch[i])==0){
						total+=num[i];
						memset(ch[i],'\0',30);
						num[i]=0;
					}
				}
			}
		}
		//cout<<"结果为："<<min_s<<" "<<total<<endl;
		fout<<total<<" "<<min_s<<endl;
	}
	
	//cout<<"合并结束！"<<endl;
	for(int i=1;i<=n;i++){//关闭小文件的输出连接流 
		fi[i].close(); 
	}
	fout.close();
}

//对result.txt文件中的<count,url>根据count大小选择top100
void selectTop100(){
	ifstream fin;
	fin.open("result.txt");
	ofstream fout;
	fout.open("top100.txt");
	int count=0;
	string s1,s2;
	char ch[30];
	int num;
	multimap<int,string,comp> ve;
	
	while(count<100){
		fin>>s1>>s2;
		//strcpy(ch,s2.c_str());
		num=atoi(s1.c_str());
		if(num>0){
			//cout<<num<<" "<<s2<<endl;
			ve.insert(make_pair(num,s2));
			count++;
		}
	}
	
	multimap<int,string,comp>::reverse_iterator iter1;
	while(!fin.eof()){
		fin>>s1>>s2;
		num=atoi(s1.c_str());
		iter1=ve.rbegin();
		//cout<<"last: "<<iter1->first<<endl;
		if(num>(iter1->first)){
			ve.erase(ve.find(iter1->first));
			ve.insert(make_pair(num,s2));
		}
	}
	fin.close();
	
	int ct=0;
	//cout<<"结果："<<endl; 
	multimap<int,string,comp>::iterator iter;
	for(iter=ve.begin();iter!=ve.end();iter++){
		ct++;
		cout<<iter->first<<" "<<iter->second<<endl;
		fout<<iter->first<<" "<<iter->second<<endl;
	}
	cout<<ct<<endl;
	fout.close();
}
int main()
{
	ProductBigURLFile();  //生成随机url.txt文件

	int n = 256;  //分割的小文件数量 
	//int step;     //每个小文件中的条目数，由  源文件总条目数/小文件数量  大概算出 
	
	//cout<<"由于内存只有1GB，建议每个小文件大小为500MB或以下, ";
	//cout<<"因此，100GB的文件建议分成300个小文件"<<endl;
	//cout<<"输入要分割成小文件的数量: "<<endl;
	//cin>>n;
	//cin>>step;
	
	cout<<"第一步： 把url文件分割成n个小文件"<<endl;
	BigFilePartition(n);//读取url.txt文件，并分割成n个小文件 
	
	cout<<"第二步： 对每一个小文件进行排序操作，最后以<URL,count>的形式写入当前小文件"<<endl;
	nFileSort(n);//对每一个小文件进行排序操作，最后以<URL,count>的形式写入当前小文件
	
	cout<<"第三步： n个有序文件按URL值进行多路归并排序,同时去重！"<<endl;
	nFileMerge(n);//n个有序文件按URL值进行多路归并排序，结果保存在result.txt文件中 
	
	cout<<"第四步： 从result文件中选择top100"<<endl;
	selectTop100();
	
	cout<<"结束"<<endl;
	
	return 0;
}

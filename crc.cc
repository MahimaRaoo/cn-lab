#include<iostream>
#include<string.h>
using namespace std;
char t[50],data[28],div[28],cs[28],rec[28];
int ld,lt,ldiv,ldata,max=28;

void xorr()
{
for(int k=1;k<ldiv;k++)
cs[k]=((cs[k]==div[k])?'0':'1');

}

void crc(char *t)
{
cout<<"in"<<t<<"in";
int i,j;
for(i=0;i<ldiv;i++)
{
cs[i]=t[i];
}
cout<<" *"<<cs<<"hi";
do
{
if(cs[0]=='1')
xorr();

for(j=0;j<ldiv-1;j++)
cs[j]=cs[j+1];

cs[j]=t[i++];

cout<<"##"<<cs<<"##";
}while(i<=ldata+ldiv-1);

}

int main()
{
//cout<<"Enter the length of the codeword";

cout<<"Enter the data word :"<<endl;
cin>>data;
ldata=strlen(data);
cout<<ldata;
cout<<endl;

cout<<"Enter the divisor :"<<endl;
cin>>div;
ldiv=strlen(div);
cout<<ldiv<<endl;

cout<<"The total word is :";


for(int i=0;i<ldata;i++)
t[i]=data[i];

for(int i=ldata;i<ldiv+ldata-1;i++)
{
t[i]='0';
}

cout<<t<<endl;


crc(t);

cout<<"The checksum is :"<<cs;


for(int i=ldata;i<ldiv+ldata-1;i++)
{
t[i]=cs[i-ldata];
}

cout<<"The total code word is :"<<t;

cout<<"Enter the received codeword :";
//cin.getline(rec,max,'\n');
cin>>rec;

crc(rec);

cout<<"The remainder is :"<<cs;

int flag=0;

for(int i=0;i<ldiv-1;i++)
{

if(rec[i]=='1')
{flag=1;
break;
}
}

if(flag==0)
cout<<"No error detected ";

else
cout<<"Error detected ";





}

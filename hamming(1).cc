#include<iostream>
using namespace std;

int main()
{
int data[7],datarec[7],c1,c2,c3,c;

cout<<"ENTER THE 4 BITS :"<<endl;
cin>>data[0];
cin>>data[1];
cin>>data[2];
cin>>data[4];

data[3]=data[0]^data[1]^data[2];
data[5]=data[0]^data[1]^data[4];
data[6]=data[0]^data[2]^data[4];

cout<<"The encoded data is :"<<endl;
for(int i=0;i<7;i++)
{
cout<<data[i]<<"  ";

}

cout<<"Enter the received data :"<<endl;
for(int i=0;i<7;i++)
{
cin>>datarec[i];
}
c3=datarec[3]^datarec[2]^datarec[1]^datarec[0];
c2=datarec[5]^datarec[4]^datarec[1]^datarec[0];
c1=datarec[6]^datarec[4]^datarec[2]^datarec[0];

c=c3*4+c2*2+c1;

if(c==0)
{
cout<<"NO ERROR"<<endl;

}


else
{
cout<<"ERROR AT : "<<7-c-1;

cout<<"THE WRONG OUTPUT IS ";
for(int i=0;i<7;i++)
{
cout<<datarec[i]<<"  ";

}

if(datarec[7-c]==0)
datarec[7-c]=1;
else
datarec[7-c]=0;


cout<<"THE CORRECT OUTPUT IS ";

for(int i=0;i<7;i++)
cout<<datarec[i]<<"  ";


}

}

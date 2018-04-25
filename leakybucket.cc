#include<iostream>
using namespace std;

int main()
{
int n=4,store=0,size=250,out_rate=50;
int in_rate[4]={100,200,100,150};

cout<<n<<store<<size<<out_rate;

for(int i=0;i<4;i++)
{
cout<<in_rate[i];
}

while(n!=0)
{
for(int i=0;i<4;i++)
{
if(in_rate[i]<(size-store))
{cout<<"packets not lost";
store+=in_rate[i];
cout<<store<<" "<<size;}
else
{
cout<<"packets lost";
store=size;
cout<<store<<" "<<size;
}

if(store>0)
{
store-=out_rate;
cout<<store<<" "<<size<<"After outgoing";
}
else
cout<<"EMPTY ";

n--;
}
}

while(store>0)
{

store-=out_rate;

cout<<store<<"after all the inputs are taken";
}


}



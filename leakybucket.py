import time

n=3
i=200
o=50
bs=300
s=0

while(n!=0):
	print("incoming packets :",i)
	if(i<=(bs-s)):
		print("no packets lost ")
		s=s+i
		print("Bucketsize :"+str(bs)+"Store wihtout packet loss:"+str(s))
	else:
		print("packets lost"+str(i-(bs-s)))
		s=bs
		print("Bucketsize :"+str(bs)+"Store with packet loss:"+str(s))
	s=s-o
	print("Bucketsize :"+str(bs)+"Store after outgoing:"+str(s))

	n=n-1
	time.sleep(5)



		 
		



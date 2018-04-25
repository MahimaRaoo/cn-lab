import socket

def client_program():
	host=socket.gethostname()
	port=5155
	cli_sock=socket.socket()
	cli_sock.connect((host,port))
	
	#msg1=input("Enter file name :")
	msg="hiii"
	cli_sock.send(msg.encode())

	content=cli_sock.recv(1024).decode()	
	print("CONTENT :"+ content)
	
	cli_sock.close()

client_program()


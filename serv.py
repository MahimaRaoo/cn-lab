import socket

def server_program():
	host=socket.gethostname()
	port=5155
	serv_sock=socket.socket()
	serv_sock.bind((host,port))
	serv_sock.listen(2)
	conn,addr=serv_sock.accept()
	print(addr)
	data=conn.recv(1024).decode()
	f=open(data)
	l=f.read()
	conn.send(l.encode())
	conn.close()

server_program()

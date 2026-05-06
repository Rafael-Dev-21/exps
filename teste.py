import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("127.0.0.1", 9999))

sock.sendto(b"hello me!", ("127.0.0.1", 9999))

data, addr = sock.recvfrom(1024)
print("recebi:",data,"de",addr)

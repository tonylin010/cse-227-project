import socket

HOST = '127.0.0.1'  # Same as your C code
PORT = 8080         # Must match your C server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    response = s.recv(1024)
    print("Received from server:")
    print(response.decode())

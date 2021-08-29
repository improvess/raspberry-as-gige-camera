#!/usr/bin/env python3

import socket

HOST = '192.168.2.2'  
PORT = 4001     

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b'PING0\0\0\0\0')
    data = s.recv(1024)

print('Received', repr(data))
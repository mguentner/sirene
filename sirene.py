#!/usr/bin/env 

import socket

port = 2355
ip = "192.168.23.177"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
data = bytearray("alarm23c", 'ascii')
data.append('s')

sock.sendto(data , (ip, port))

sock.close();

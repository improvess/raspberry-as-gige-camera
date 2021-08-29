
import socket

class Device(object):
  
  STATUS_SIZE = 4
  HEADER_SIZE = STATUS_SIZE + 5

  def __init__(self, address, port, response_buffer_size):
    self.address = address
    self.port = port
    self.response_buffer_size = max(Device.HEADER_SIZE, response_buffer_size)

  def ping(self):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      s.sendall(b'PING0\0\0\0\0')
      reply = s.recv(self.response_buffer_size)
      result = reply[0:min(len(reply), Device.STATUS_SIZE)].decode("utf-8") 
    return result

  def isOpened(self):
    result = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      s.sendall(b'ISOP0\0\0\0\0')
      reply = s.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
    return result

  def open(self):
    result = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      s.sendall(b'OPEN0\0\0\0\0')
      reply = s.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
    return result

  def release(self):
    result = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      s.sendall(b'CLOS0\0\0\0\0')
      reply = s.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
    return result

  def set(self, propId, ):
    result = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      s.sendall(b'OPEN0\0\0\0\0')
      reply = s.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
    return result
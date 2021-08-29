
import socket
import struct
import cv2 as cv
import numpy as np

class Device(object):
  
  STATUS_SIZE = 4
  DATA_SIZE_ADDRESS = STATUS_SIZE + 1
  HEADER_SIZE = STATUS_SIZE + 5
  IMAGE_META_DATA_SIZE = 12

  def __init__(self, address, port):
    self.address = address
    self.port = port
    self.response_buffer_size = Device.HEADER_SIZE + Device.IMAGE_META_DATA_SIZE + 1920 * 1080 * 3

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

  def set(self, propId, value):
    result = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      data_size = 12
      double_value = float(value)
      double_bytes = struct.pack('<d', double_value)
      request = b'SET00' + data_size.to_bytes(4, 'little') + propId.to_bytes(4, 'little') + double_bytes
      s.sendall(request)
      reply = s.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
    return result

  def get(self, propId):
    result = -1
    ret = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      data_size = 4
      request = b'GET00' + data_size.to_bytes(4, 'little') + propId.to_bytes(4, 'little')
      s.sendall(request)
      reply = s.recv(self.response_buffer_size)
      ret = len(reply) >= (Device.HEADER_SIZE + 4) and reply.startswith(b'0200')
      if ret:
        double_in_bytes = reply[Device.HEADER_SIZE : Device.HEADER_SIZE + 8]
        result, = struct.unpack('<d', double_in_bytes)

    return ret, result

  def read(self):
    result = None
    ret = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.connect((self.address, self.port))
      request = b'GRAB0\0\0\0\0'
      s.sendall(request)
      header_part = s.recv(Device.HEADER_SIZE)
      ret = len(header_part) >= (Device.HEADER_SIZE) and header_part.startswith(b'0200')
      if ret:
        data_size = int.from_bytes(header_part[Device.DATA_SIZE_ADDRESS : Device.DATA_SIZE_ADDRESS + 4], "little") 
        ret = False

        if data_size > Device.IMAGE_META_DATA_SIZE:
          reply = self.__load_frame_data(s, data_size)
          if reply:
            offset = 0
            rows  = int.from_bytes(reply[offset : offset + 4], "little") 
            offset += 4
            cols  = int.from_bytes(reply[offset : offset + 4], "little") 
            offset += 4
            type  = int.from_bytes(reply[offset : offset + 4], "little") 
            # FIXME use type variable
            result = np.zeros((rows, cols, 3), np.uint8) 
            offset += 4
            result.data = reply[offset : offset + data_size - Device.IMAGE_META_DATA_SIZE]
            ret = True
      
    return ret, result

  def __load_frame_data(self, sock, data_size):
    data = bytearray()
    while len(data) < data_size:
        packet = sock.recv(data_size - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data



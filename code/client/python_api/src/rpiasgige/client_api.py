
import socket
import struct
import numpy as np
import time

class Device(object):
  
  STATUS_SIZE = 4
  DATA_SIZE_ADDRESS = STATUS_SIZE + 1
  HEADER_SIZE = STATUS_SIZE + 5
  IMAGE_META_DATA_SIZE = 12

  def __init__(self, address, port):
    self.address = address
    self.port = port
    self.response_buffer_size = Device.HEADER_SIZE + Device.IMAGE_META_DATA_SIZE + 1920 * 1080 * 3
    self.server_socket = None

  def ping(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'PING' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      self.server_socket.sendall(request)
      reply = self.server_socket.recv(self.response_buffer_size)
      result = reply.startswith(b'PONG')
      if not keep_alive:
        self.__disconnect()
    return result

  def isOpened(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'ISOP' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      self.server_socket.sendall(request)
      reply = self.server_socket.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
      if not keep_alive:
        self.__disconnect()
    return result

  def open(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'OPEN' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      self.server_socket.sendall(request)
      reply = self.server_socket.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
      if not keep_alive:
        self.__disconnect()
    return result

  def release(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'CLOS' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      self.server_socket.sendall(request)
      reply = self.server_socket.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
      if not keep_alive:
        self.__disconnect()
    return result

  def set(self, propId, value, keep_alive = False):
    result = False
    if self.__checkConnection():
      data_size = 12
      double_value = float(value)
      double_bytes = struct.pack('<d', double_value)
      request = b'SET0' + (b'1' if keep_alive else b'0') + data_size.to_bytes(4, 'little') + propId.to_bytes(4, 'little') + double_bytes
      self.server_socket.sendall(request)
      reply = self.server_socket.recv(self.response_buffer_size)
      result = reply.startswith(b'0200')
      if not keep_alive:
        self.__disconnect()
    return result

  def get(self, propId, keep_alive = False):
    result = -1
    ret = False
    if self.__checkConnection():
      data_size = 4
      request = b'GET0' + (b'1' if keep_alive else b'0') + data_size.to_bytes(4, 'little') + propId.to_bytes(4, 'little')
      self.server_socket.sendall(request)
      reply = self.server_socket.recv(self.response_buffer_size)
      ret = len(reply) >= (Device.HEADER_SIZE + 4) and reply.startswith(b'0200')
      if ret:
        double_in_bytes = reply[Device.HEADER_SIZE : Device.HEADER_SIZE + 8]
        result, = struct.unpack('<d', double_in_bytes)
      if not keep_alive:
        self.__disconnect()

    return ret, result

  def read(self, keep_alive = False):
    result = None
    ret = False
    if self.__checkConnection():
      request = b'GRAB' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      self.server_socket.sendall(request)
      self.server_socket.sendall(request)
      header_part = self.server_socket.recv(Device.HEADER_SIZE)
      ret = len(header_part) >= (Device.HEADER_SIZE) and header_part.startswith(b'0200')
      if ret:
        data_size = int.from_bytes(header_part[Device.DATA_SIZE_ADDRESS : Device.DATA_SIZE_ADDRESS + 4], "little") 
        ret = False

        if data_size > Device.IMAGE_META_DATA_SIZE:
          reply = self.__load_frame_data(data_size)
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
      if not keep_alive:
        self.__disconnect()
      
    return ret, result

  def __load_frame_data(self, data_size):
    data = bytearray()
    while len(data) < data_size:
        packet = self.server_socket.recv(data_size - len(data))
        if not packet:
            return None
        data.extend(packet)
    return data

  def __checkConnection(self):
    if self.server_socket is None:
      self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      try:
        self.server_socket.connect((self.address, self.port))
      except:
        self.server_socket = None
    return not self.server_socket is None

  def __disconnect(self):
    if not self.server_socket is None:
      self.server_socket.close()
      self.server_socket = None

class Performance_Counter(object):

  def __init__(self, cycle_count):
    self.CYCLE_COUNT = cycle_count
    self.count = 0
    self.total_read = 0.0
    self.fps = -1.0
    self.mean_data_size = -1.0
    self.begin_time_ref = time.monotonic() 

  def loop(self, data_size):
    result = False

    self.count += 1
    self.total_read += data_size

    if self.count == 1:
      self.begin_time_ref = time.monotonic() 

    if self.count >= self.CYCLE_COUNT:
      end_time_ref = time.monotonic() 
      time_spent = (end_time_ref - self.begin_time_ref) * 1000.0
      self.mean_data_size = self.total_read / self.count
      if time_spent >= 1.0:
          self.fps = self.count * 1000.0 / time_spent
          self.mean_data_size = self.total_read / self.count
          result = True
      else:
        self.fps = -1.0
      self.count = 0
      self.total_read = 0
    return result

  def get_fps(self):
    return self.fps

  def get_mean_data_size(self):
    return self.mean_data_size

  def reset(self):
    self.count = 0
    self.total_read = 0.0
    self.fps = -1.0
    self.mean_data_size = -1.0
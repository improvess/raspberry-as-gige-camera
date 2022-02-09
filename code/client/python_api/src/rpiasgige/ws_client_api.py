
import websocket
import struct
import numpy as np
import time
import sys

class Device(object):
  
  STATUS_SIZE = 4
  DATA_SIZE_ADDRESS = STATUS_SIZE + 1
  HEADER_SIZE = STATUS_SIZE + 5
  IMAGE_META_DATA_SIZE = 12
  RESPONSE_BUFFER_SIZE = 1024

  def __init__(self, address, port):
    self.address = address
    self.port = port
    self.server_socket = None
    self.successive_connection_issues = 0
    self.response_buffer = bytearray(Device.IMAGE_META_DATA_SIZE + 1920 * 1080 * 3)
    self.read_timeout = 1

  def ping(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'PING' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      reply = self.__talk(request)
      result = reply.startswith(b'PONG')
      if not keep_alive:
        self.__disconnect()
    return result

  def isOpened(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'ISOP' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      reply = self.__talk(request)
      result = reply.startswith(b'0200')
      if not keep_alive:
        self.__disconnect()
    return result

  def open(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'OPEN' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      reply = self.__talk(request)
      result = reply.startswith(b'0200')
      if not keep_alive:
        self.__disconnect()
    return result

  def release(self, keep_alive = False):
    result = False
    if self.__checkConnection():
      request = b'CLOS' + (b'1' if keep_alive else b'0') + b'\0\0\0\0'
      reply = self.__talk(request)
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
      reply = self.__talk(request)
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
      reply = self.__talk(request)
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
      package = self.__talk(request)
      ret = len(package) >= (Device.HEADER_SIZE) and package.startswith(b'0200')
      if ret:
        data_size = int.from_bytes(package[Device.DATA_SIZE_ADDRESS : Device.DATA_SIZE_ADDRESS + 4], "little") 
        ret = False
        if data_size > Device.IMAGE_META_DATA_SIZE:
          offset = Device.DATA_SIZE_ADDRESS + 4
          rows  = int.from_bytes(package[offset : offset + 4], "little") 
          offset += 4
          cols  = int.from_bytes(package[offset : offset + 4], "little") 
          offset += 4
          type  = int.from_bytes(package[offset : offset + 4], "little") 
          # FIXME use type variable
          result = np.zeros((rows, cols, 3), np.uint8) 
          offset += 4
          result.data = package[offset : offset + data_size - Device.IMAGE_META_DATA_SIZE]
          ret = True
      if not keep_alive:
        self.__disconnect()
      
    return ret, result

  def __checkConnection(self):
    if self.server_socket is None:
      try:
        self.server_socket = websocket.create_connection('ws://' + self.address + ':' + str(self.port))
      except:
        self.__disconnect()
    return not self.server_socket is None

  def setReadTimeout(self, newValue):
    pass

  def __disconnect(self):
    if not self.server_socket is None:
      try:
        self.server_socket.close()
      except:
        # swallow the exception on closing
        pass
      finally:
        self.server_socket = None

  def __talk(self, request):
    self.server_socket.send_binary(request)
    result = self.server_socket.recv()
    return result

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

def printf(format, *args):
    sys.stdout.write(format % args)
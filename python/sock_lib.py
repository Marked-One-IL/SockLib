from __future__ import annotations
import socket
import struct

class Serializer:
    def __init__(self):
        self._bytes: bytes = bytes()

    def serialize_int8(self, i: int):
        self._bytes += struct.pack("<b", i)
    def serialize_uint8(self, i: int):
        self._bytes += struct.pack("<B", i)
    def serialize_int16(self, i: int):
        self._bytes += struct.pack("<h", i)
    def serialize_uint16(self, i: int):
        self._bytes += struct.pack("<H", i)   
    def serialize_int32(self, i: int):
        self._bytes += struct.pack("<i", i)
    def serialize_uint32(self, i: int):
        self._bytes += struct.pack("<I", i)
    def serialize_int64(self, i: int):
        self._bytes += struct.pack("<q", i)
    def serialize_uint64(self, i: int):
        self._bytes += struct.pack("<Q", i)
    def serialize_float32(self, f: float):
        self._bytes += struct.pack("<f", f)
    def serialize_float64(self, f: float):
        self._bytes += struct.pack("<d", f)
    def serialize_bytes(self, b: bytes):
        self._bytes += b

    def serialize_bool(self, b: bool):
        self.serialize_uint8(int(b))
    def serialize_char(self, c: str):
        self.serialize_int8(ord(c[0]))
    def serialize_int(self, i: int):
        self.serialize_int32(i)
    def serialize_float(self, f: float):
        self.serialize_float32(f)
    def serialize_str(self, s: str):
        b: bytes = s.encode()
        self.serialize_uint32(len(b))
        self._bytes += b

class Deserializer:
    def __init__(self, b: bytes):
        self._bytes: bytes = b
        self._current: int = 0

    def deserialize_int8(self) -> int:
        i = struct.unpack("<b", self.deserialize_bytes(1))[0]
        return i 
    def deserialize_uint8(self) -> int:
        i = struct.unpack("<B", self.deserialize_bytes(1))[0]
        return i
    def deserialize_int16(self) -> int:
        i = struct.unpack("<h", self.deserialize_bytes(2))[0]
        return i
    def deserialize_uint16(self) -> int:
        i = struct.unpack("<H", self.deserialize_bytes(2))[0]
        return i
    def deserialize_int32(self) -> int:
        i = struct.unpack("<i", self.deserialize_bytes(4))[0]
        return i
    def deserialize_uint32(self) -> int:
        i = struct.unpack("<I", self.deserialize_bytes(4))[0]
        return i
    def deserialize_int64(self) -> int:
        i = struct.unpack("<q", self.deserialize_bytes(8))[0]
        return i
    def deserialize_uint64(self) -> int:
        i = struct.unpack("<Q", self.deserialize_bytes(8))[0]
        return i
    def deserialize_float32(self) -> float:
        f = struct.unpack("<f", self.deserialize_bytes(4))[0]
        return f
    def deserialize_float64(self) -> float:
        f = struct.unpack("<d", self.deserialize_bytes(8))[0]
        return f
    def deserialize_bytes(self, size: int) -> bytes:
        b = self._bytes[self._current: self._current + size]
        self._current += size
        return b

    def deserialize_bool(self) -> bool:
        return bool(self.deserialize_uint8())
    def deserialize_char(self) -> str:
        return chr(self.deserialize_int8())[0]
    def deserialize_int(self) -> int:
        return self.deserialize_int32()
    def deserialize_float(self) -> float:
        return self.deserialize_float32()
    def deserialize_str(self) -> str:
        size: int = self.deserialize_uint32()
        return self.deserialize_bytes(size).decode()

class Sock:
    SIZE_LIMIT: int = 2147483647
    LOCAL_HOST: str = "127.0.0.1"

    def __init__(self, sock: socket.socket):
        self._sock: socket.socket = sock

    def set_timeout(self, ms: int):
        self._sock.settimeout(float(ms) / 1000.0)
    def close(self):
        self._sock.close() # 'shutdown()' could cause issues. Python doesn't have strict move semantics.

    def send_serialized(self, s: Serializer):
        self.send_uint32(len(s._bytes))
        self.send_all_bytes(s._bytes)
    def recv_deserialized(self) -> Deserializer:
        size: int = self.recv_uint32()
        return Deserializer(self.recv_all_bytes(size))
    def recv_deserialized_limit(self, limit: int) -> Deserializer | None:
        size: int = self.recv_uint32()
        if size > limit:
            self.recv_discard(size)
            return None
        return Deserializer(self.recv_all_bytes(size))

    def send_int8(self, i: int):
        self.send_all_bytes(struct.pack("<b", i)) 
    def send_uint8(self, i: int):
        self.send_all_bytes(struct.pack("<B", i))
    def send_int16(self, i: int):
        self.send_all_bytes(struct.pack("<h", i))
    def send_uint16(self, i: int):
        self.send_all_bytes(struct.pack("<H", i))
    def send_int32(self, i: int):
        self.send_all_bytes(struct.pack("<i", i))
    def send_uint32(self, i: int):
        self.send_all_bytes(struct.pack("<I", i))
    def send_int64(self, i: int):
        self.send_all_bytes(struct.pack("<q", i))
    def send_uint64(self, i: int):
        self.send_all_bytes(struct.pack("<Q", i))
    def send_float32(self, f: float):
        self.send_all_bytes(struct.pack("<f", f))
    def send_float64(self, f: float):
        self.send_all_bytes(struct.pack("<d", f))
    def send_all_bytes(self, b: bytes):
        if len(b) > Sock.SIZE_LIMIT:
            raise Exception("len(b) > Sock.SIZE_LIMIT")
        self._sock.sendall(b)
    def send_some_bytes(self, b: bytes):
        if len(b) > Sock.SIZE_LIMIT:
            raise Exception("len(b) > Sock.SIZE_LIMIT")
        self._sock.send(b)

    def send_bool(self, b: bool):
        self.send_uint8(int(b))
    def send_char(self, b: str):
        self.send_int8(ord(b[0]))
    def send_int(self, i: int):
        self.send_int32(i)
    def send_float(self, f: float):
        self.send_float32(f)
    def send_str(self, s: str):
        b: bytes = s.encode()
        self.send_uint32(len(b))
        self.send_all_bytes(b)

    def recv_int8(self) -> int:
        return struct.unpack("<b", self.recv_all_bytes(1))[0]
    def recv_uint8(self) -> int:
        return struct.unpack("<B", self.recv_all_bytes(1))[0]
    def recv_int16(self) -> int:
        return struct.unpack("<h", self.recv_all_bytes(2))[0]
    def recv_uint16(self) -> int:
        return struct.unpack("<H", self.recv_all_bytes(2))[0]
    def recv_int32(self) -> int:
        return struct.unpack("<i", self.recv_all_bytes(4))[0]
    def recv_uint32(self) -> int:
        return struct.unpack("<I", self.recv_all_bytes(4))[0]
    def recv_int64(self) -> int:
        return struct.unpack("<q", self.recv_all_bytes(8))[0]
    def recv_uint64(self) -> int:
        return struct.unpack("<Q", self.recv_all_bytes(8))[0]
    def recv_float32(self) -> float:
        return struct.unpack("<f", self.recv_all_bytes(4))[0]
    def recv_float64(self) -> float:
        return struct.unpack("<d", self.recv_all_bytes(8))[0]
    def recv_all_bytes(self, size: int) -> bytes:
        if size > Sock.SIZE_LIMIT:
            raise Exception("size > Sock.SIZE_LIMIT")
        sent: int = 0
        b: bytes = bytes()
        while sent < size:
            t: bytes = self._sock.recv(size - sent)
            received: int = len(t)
            if received == 0:
                raise Exception("Failed to receive data because the session ended")
            sent += received
            b += t
        return b
    def recv_some_bytes(self, size: int) -> bytes:
        if size > Sock.SIZE_LIMIT:
            raise Exception("size > Sock.SIZE_LIMIT")
        b: bytes = self._sock.recv(size)
        if len(b) == 0:
            raise Exception("Failed to receive data because the session ended")
        return b
    def recv_discard(self, size: int):
        sent: int = 0
        while sent < size:
            sent += len(self.recv_some_bytes(size - sent))

    def recv_bool(self) -> bool:
        return bool(self.recv_uint8())
    def recv_char(self) -> str:
        return chr(self.recv_int8())[0]
    def recv_int(self) -> int:
        return self.recv_int32()
    def recv_float(self) -> float:
        return self.recv_float32()
    def recv_str(self) -> str:
        size: int = self.recv_uint32()
        return self.recv_all_bytes(size).decode()
    def recv_str_limit(self, limit: int) -> str | None:
        size: int = self.recv_uint32()
        if size > limit:
            self.recv_discard(size)
            return None
        return self.recv_all_bytes(size).decode()

    @classmethod
    def connect(cls, address: str, port: int) -> Sock:
        sock: Sock = Sock(socket.socket(socket.AF_INET, socket.SOCK_STREAM))
        sock._sock.connect((address, port))
        return sock

class Server:
    def __init__(self, port: int, localhost: bool):
        self._mainSock: Sock = Sock(socket.socket(socket.AF_INET, socket.SOCK_STREAM))

        if localhost:
            self._mainSock._sock.bind(("127.0.0.1", port))
        else:
            self._mainSock._sock.bind(("0.0.0.0", port))
        self._mainSock._sock.listen()

    def accept(self) -> Sock:
        return Sock(self._mainSock._sock.accept()[0])
import serial
import struct
import time

PORT = "/dev/ttyACM0"
BAUD = 115200

def vn_crc16(data: bytes) -> int:
    crc = 0
    for b in data:
        crc = ((crc >> 8) | (crc << 8)) & 0xFFFF
        crc ^= b
        crc ^= (crc & 0xFF) >> 4
        crc ^= (crc << 12) & 0xFFFF
        crc ^= ((crc & 0xFF) << 5) & 0xFFFF
    return crc & 0xFFFF

def build_packet(yaw, pitch, roll, ax, ay, az, pressure):
    pkt = bytearray(36)

    pkt[0] = 0xFA
    pkt[1] = 0x01
    pkt[2] = 0x05
    pkt[3] = 0x00
    pkt[4] = 0x00

    struct.pack_into("<f", pkt,  5, yaw)
    struct.pack_into("<f", pkt,  9, pitch)
    struct.pack_into("<f", pkt, 13, roll)
    struct.pack_into("<f", pkt, 17, ax)
    struct.pack_into("<f", pkt, 21, ay)
    struct.pack_into("<f", pkt, 25, az)
    struct.pack_into("<f", pkt, 29, pressure)

    crc = vn_crc16(pkt[1:33])
    pkt[33] = (crc >> 8) & 0xFF
    pkt[34] = crc & 0xFF
    pkt[35] = 0x00

    return pkt

ser = serial.Serial(PORT, BAUD)
time.sleep(1)

print("Sending VN packets at 200 Hz")

while True:
    pkt = build_packet(
        yaw=1.0,
        pitch=2.0,
        roll=3.0,
        ax=0.1,
        ay=0.2,
        az=9.81,
        pressure=1013.25
    )
    ser.write(pkt)
    time.sleep(0.005)


import socket
import struct
import threading

HOST = "127.0.0.1"
TCP_PORT = 4444
UDP_PORT = 4445


def receive_all(conn, size):
    data = b""

    while len(data) < size:
        chunk = conn.recv(size - len(data))

        if not chunk:
            return None

        data += chunk

    return data


def tcp_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, TCP_PORT))
    server.listen(1)

    print(f"TCP server listening on {HOST}:{TCP_PORT}")

    conn, addr = server.accept()

    print(f"TCP client connected: {addr}")

    try:
        raw_size = receive_all(conn, 4)

        if raw_size is None:
            return

        packet_size = struct.unpack("<I", raw_size)[0]

        packet = receive_all(conn, packet_size)

        if packet is None:
            return

        offset = 0

        player_id = struct.unpack_from("<I", packet, offset)[0]
        offset += 4

        name_length = struct.unpack_from("<I", packet, offset)[0]
        offset += 4

        username = packet[offset:offset + name_length].decode("utf-8")
        offset += name_length

        x, y = struct.unpack_from("<ff", packet, offset)
        offset += 8

        level = struct.unpack_from("<i", packet, offset)[0]
        offset += 4

        health = struct.unpack_from("<i", packet, offset)[0]
        offset += 4

        max_health = struct.unpack_from("<i", packet, offset)[0]
        offset += 4

        experience = struct.unpack_from("<i", packet, offset)[0]
        offset += 4

        animation = struct.unpack_from("<i", packet, offset)[0]
        offset += 4

        connected = struct.unpack_from("<B", packet, offset)[0]

        print("\n--- TCP Player received ---")
        print(f"ID:         {player_id}")
        print(f"Username:   {username}")
        print(f"Position:   ({x}, {y})")
        print(f"Level:      {level}")
        print(f"Health:     {health}/{max_health}")
        print(f"Experience: {experience}")
        print(f"Animation:  {animation}")
        print(f"Connected:  {bool(connected)}")

    finally:
        conn.close()
        server.close()


def udp_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind((HOST, UDP_PORT))

    print(f"UDP server listening on {HOST}:{UDP_PORT}")

    while True:
        data, addr = server.recvfrom(12)

        if len(data) != 12:
            continue

        player_id, x, y = struct.unpack("<Iff", data)

        print(f"[UDP] ID: {player_id} | Position: ({x}, {y})")


if __name__ == "__main__":
    tcp_thread = threading.Thread(target=tcp_server)
    udp_thread = threading.Thread(target=udp_server)

    tcp_thread.start()
    udp_thread.start()

    tcp_thread.join()
    udp_thread.join()
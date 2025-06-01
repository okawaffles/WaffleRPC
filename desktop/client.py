import socket
import time
import pypresence

SERVER_IP = "192.168.1.202"
PORT = 0xCAFE
BUFFER_SIZE = 2048  # Can be any reasonable size

def log_packet(data: bytes):
    print(f"\n📦 Received {len(data)} bytes:")
    print("🧱 Raw Bytes:", data.hex())
    
    # Optional: Try to interpret as known structure (magic + title_id + name)
    if len(data) >= 628:
        magic = int.from_bytes(data[0:8], 'little')
        title_id = int.from_bytes(data[8:16], 'little')
        name = data[16:628].split(b'\x00', 1)[0].decode(errors='replace')
        print("🔍 Interpreted Content:")
        print(f"  Magic    : 0x{magic:016X}")
        print(f"  Title ID : 0x{title_id:016X}")
        print(f"  Name     : {name}")
    else:
        print("⚠️ Packet too small to parse structured data.")

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.settimeout(10)
        try:
            sock.connect((SERVER_IP, PORT))
        except socket.timeout:
            print("❌ Connection timed out.")
            return
        except socket.error as e:
            print(f"❌ Connection failed: {e}")
            return

        print(f"✅ Connected to {SERVER_IP}:{PORT}")

        while True:
            try:
                data = sock.recv(BUFFER_SIZE)
                if not data:
                    print("❌ Connection closed by peer.")
                    break
                log_packet(data)
            except socket.timeout:
                print("⌛ No data received (timeout).")
            except Exception as e:
                print(f"❌ Error receiving data: {e}")
                break

if __name__ == "__main__":
    main()
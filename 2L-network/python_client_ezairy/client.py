import socket
import time

SERVER_HOST = 'py_server_container'
SERVER_PORT = 5001

def request_data():
    while True:
        try:
            # Membuka sambungan soket ke server pasangan spesifiknya
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((SERVER_HOST, SERVER_PORT))
            
            # Hantar arahan permintaan kepada server
            client_socket.sendall(b"GET_LATEST")
            
            # Menerima jawapan respons daripada server
            response = client_socket.recv(1024).decode('utf-8')
            print(f"[Client] Response received:\n -> {response}\n")
            
            client_socket.close()
        except Exception as e:
            print(f"[Client Error] Cannot connect to server: {e}")
        
        # Request data setiap 10 saat untuk melihat perubahan point dari pelayan
        time.sleep(10)

if __name__ == "__main__":
    print("[Client] Starting Python Socket Client...")
    request_data()
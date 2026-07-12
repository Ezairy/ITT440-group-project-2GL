import socket
import mysql.connector
import time
import threading

DB_CONFIG = {
    'host': 'db_container',
    'user': 'root',
    'password': '2GL@345',
    'database': 'project_db'
}

USER_NAME = 'python_server_user1'
PORT = 5001

def update_points_loop():
    """Thread latar belakang untuk menambah point setiap 30 saat."""
    while True:
        try:
            conn = mysql.connector.connect(**DB_CONFIG)
            cursor = conn.cursor()
            
            # Menaikkan point dan mengemas kini timestamp automatik
            query = "UPDATE user_points SET points = points + 1 WHERE user = %s"
            cursor.execute(query, (USER_NAME,))
            conn.commit()
            
            print(f"[Cron] Points updated for {USER_NAME}")
            cursor.close()
            conn.close()
        except Exception as e:
            print(f"[Cron Error] Database update failed: {e}")
        
        time.sleep(30)

def handle_client_requests():
    """Fungsi utama pelayan soket untuk melayan permintaan klien."""
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Re-use port untuk mengelakkan ralat 'Address already in use'
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('0.0.0.0', PORT))
    server_socket.listen(5)
    print(f"[Server] Python Socket Server listening on port {PORT}...")

    while True:
        client_conn, addr = server_socket.accept()
        print(f"[Server] Connected by {addr}")
        
        try:
            # Menerima mesej arahan (request) daripada klien
            data = client_conn.recv(1024).decode('utf-8')
            if data == "GET_LATEST":
                # Akses pangkalan data sebelum menghantar data kepada klien
                conn = mysql.connector.connect(**DB_CONFIG)
                cursor = conn.cursor(dictionary=True)
                
                query = "SELECT user, points, datetime_stamp FROM user_points WHERE user = %s"
                cursor.execute(query, (USER_NAME,))
                result = cursor.fetchone()
                
                cursor.close()
                conn.close()

                if result:
                    response = f"User: {result['user']} | Points: {result['points']} | Last Update: {result['datetime_stamp']}"
                else:
                    response = "User data not found."
                
                client_conn.sendall(response.encode('utf-8'))
        except Exception as e:
            print(f"[Server Error] {e}")
            client_conn.sendall(b"Error fetching data from database.")
        finally:
            client_conn.close()

if __name__ == "__main__":
    # Jalankan loop kemas kini data 30 saat secara asynchronous
    cron_thread = threading.Thread(target=update_points_loop, daemon=True)
    cron_thread.start()

    # Jalankan pelayan soket utama
    handle_client_requests()
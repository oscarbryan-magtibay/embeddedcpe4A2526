import socket
import requests

def check_port(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(2)
    result = sock.connect_ex((host, port))
    sock.close()
    return result == 0

def main():
    host = "192.168.254.110"
    port = 5000
    
    print("=== Network Diagnostic ===")
    print(f"Target: {host}:{port}")
    
    # Check port
    if check_port(host, port):
        print("✓ Port 5000 is OPEN and listening")
    else:
        print("✗ Port 5000 is CLOSED or blocked")
        print("  Run as Administrator: netsh advfirewall firewall add rule name='Port 5000' dir=in action=allow protocol=TCP localport=5000")
    
    # Test endpoints
    endpoints = ["/", "/api/current", "/api/sensor-data"]
    for endpoint in endpoints:
        url = f"http://{host}:{port}{endpoint}"
        try:
            if endpoint == "/api/sensor-data":
                response = requests.post(url, json={"test": True}, timeout=5)
            else:
                response = requests.get(url, timeout=5)
            print(f"✓ {endpoint} - Status: {response.status_code}")
        except Exception as e:
            print(f"✗ {endpoint} - Error: {e}")

if __name__ == "__main__":
    main()
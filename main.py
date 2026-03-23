import serial
import time
from datetime import datetime

SERIAL_PORT = 'COM4'
BAUD_RATE = 115200
TIMEOUT = 1

def log(message):
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] {message}")

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
        log(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud")
        print("Type message (format: ID:message) or 'exit'")

        while True:
            user_input = input("> ")

            if user_input.lower() == 'exit':
                break

            # Expect input like: 101:Hello
            if ":" not in user_input:
                print("Invalid format. Use ID:message")
                continue

            msg = "MSG:" + user_input
            ser.write((msg + '\n').encode())

            log(f"Sent → {msg}")

            time.sleep(0.1)

            while ser.in_waiting > 0:
                response = ser.readline().decode().strip()

                if response.startswith("RPLY"):
                    parts = response.split(":")
                    if len(parts) == 3:
                        _, rid, ans = parts
                        log(f"📩 Reply from Device {rid} → {ans}")
                elif response:
                    log(f"Device: {response}")

    except serial.SerialException as e:
        print(f"Error: {e}")

    except KeyboardInterrupt:
        print("\nExiting...")

    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serial port closed")

if __name__ == "__main__":
    main()
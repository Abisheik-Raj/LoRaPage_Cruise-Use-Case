import serial
import time

SERIAL_PORT = 'COM4'  
BAUD_RATE = 115200
TIMEOUT = 1  

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud")
        print("Enter messages to send (type 'exit' to quit):")
        
        while True:
            user_input = input("> ")
            if user_input.lower() == 'exit':
                break
                
            ser.write((user_input + '\n').encode('utf-8'))
            print(f"Sent: {user_input}")
            
            time.sleep(0.1)
            while ser.in_waiting > 0: 
                response = ser.readline().decode('utf-8').strip()
                if response:
                    print(f"Response: {response}")
                    
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
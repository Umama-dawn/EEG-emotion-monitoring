# Reads CSV-style serial lines from ESP32 and saves to CSV file
import serial, csv, time

SERIAL_PORT = "COM5"   # change to your port, or "/dev/ttyUSB0"
BAUD = 115200
OUT_CSV = "eeg_data.csv"

def main():
    ser = serial.Serial(SERIAL_PORT, BAUD, timeout=1)
    time.sleep(2)
    with open(OUT_CSV, "w", newline="") as f:
        writer = csv.writer(f)
        header = ["timestamp"] + [f"CH{i}" for i in range(1,9)]
        writer.writerow(header)
        while True:
            line = ser.readline().decode(errors='ignore').strip()
            if not line:
                continue
            # expected: ts,ch1,ch2,...ch8
            parts = line.split(",")
            if len(parts) >= 9:
                try:
                    row = [float(x) for x in parts[:9]]
                    writer.writerow(row)
                    print(row)
                except:
                    pass

if __name__ == "__main__":
    main()

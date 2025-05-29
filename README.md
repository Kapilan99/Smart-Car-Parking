
# Arduino I2C Sensor Network

This project implements a sensor network using multiple Arduino boards. A **master Arduino** communicates with **two slave Arduinos** over the **I2C protocol** to collect and display sensor data.

## 📦 Project Structure

* `master_code.ino` – Controls the overall system and requests data from slave devices.
* `slave_code1.ino` – Reads data from sensors (e.g., DHT11 for temperature and humidity) and responds to the master.
* `Slave_code2.ino` – Reads data from additional sensors and responds to the master.

## 🔌 Hardware Requirements

* 3 × Arduino boards (1 Master + 2 Slaves)
* I2C pull-up resistors (4.7kΩ or 10kΩ recommended)
* Sensors:

  * DHT11 or DHT22 (on Slave 1)
  * LDR, or any analog sensor (on Slave 2)
* Jumper wires
* Breadboards (optional)

## 🔁 Communication Protocol

* **I2C Bus**

  * Master initiates communication and requests data
  * Slave 1 I2C Address: `0x08`
  * Slave 2 I2C Address: `0x09`

## 🧠 Functionality

### Master Arduino

* Periodically requests data from Slave 1 and Slave 2
* Displays the received data over Serial Monitor

### Slave 1

* Reads temperature and humidity from DHT sensor
* Sends formatted string data to master when requested

### Slave 2

* Reads data from analog sensor (e.g., LDR)
* Sends data to master when requested

## 🚀 Getting Started

### 1. Upload Code

* Flash `slave_code1.ino` to the first slave (connected to DHT sensor)
* Flash `Slave_code2.ino` to the second slave (connected to analog sensor)
* Flash `master_code.ino` to the master board

### 2. Connect I2C

* Connect `SDA`, `SCL`, `GND` between all Arduinos
* Ensure each slave has a unique I2C address
* Use pull-up resistors on `SDA` and `SCL` lines

### 3. Monitor Output

* Open Serial Monitor on the master Arduino
* Observe real-time sensor values collected from both slaves

## 🛠️ Libraries Used

* `Wire.h` – I2C communication
* `DHT.h` – DHT sensor library (for Slave 1)

Install DHT library via:

```bash
Arduino IDE → Library Manager → Search “DHT sensor library” by Adafruit → Install
```

## 📷 Sample Output

```
Reading from Slave 1: Temp = 26.0°C, Humidity = 58.0%
Reading from Slave 2: Light Intensity = 678
```

## ⚠️ Notes

* Ensure the slaves respond within the timeout period to avoid communication errors.
* Use external power if current drawn by sensors exceeds USB supply.

## 📄 License

MIT License



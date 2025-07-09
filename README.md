# motor-rpm-measurement
This project measures and displays the RPM of a DC motor using an ESP32, an optical encoder, and a magnetic Hall effect sensor (A3144). RPM data is shown on a local web server for real-time monitoring.

---

## 📷 Schematic

![Schematic](Images/kicad_schematic.png)

---

## ⚙️ Components Used
- ESP32 DevKit V1
- Optical encoder (3806-OPTI-360-AB-OC)
- A3144 Hall Effect Sensor
- L298N motor driver
- DC Motor 12V
- 12V power supply
- Buck converter (LM2596S)

---

## 🚀 Features
- Real-time RPM measurement using both sensors
- Local web server-based data viewing
- Simultaneous output from both sensors

---

## 🧠 How it Works
The ESP32 counts pulses from both sensors and calculates RPM over time. If no pulse is received in a time window, RPM is considered zero, and RPM is shown via the local web server.

---

## 📁 Folder Structure

- [`/code`](./code) → Arduino sketch (`rpm_measurement.ino`)
- [`/Images`](./Images) → KiCad schematic image
- [`README.md`](./README.md) → This documentation file


---

## 🛠️ How to Use
1. Clone this repository to your local machine.
2. Open the `.ino` file in Arduino IDE or PlatformIO.
3. Connect the ESP32 to your system and upload the code.
4. Power the motor setup and sensors as per the schematic.
5. Fill in the mobile hotspot name and password in the SSID and Password section of the code.
6. Open a browser and visit the IP address printed on the Serial Monitor to view RPM.

---

## 📎 License
This project is licensed under the MIT License.


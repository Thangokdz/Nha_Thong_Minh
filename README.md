# Nha_Thong_Minh
Các tính năng tích hợp để làm cho ngôi nhà sống tiện ích hơn

## Description
This project is an IoT-based smart home system that monitors environmental conditions and automatically controls household devices such as lights, fans, and air conditioning based on sensor data.

## Features
- Monitor temperature and humidity using DHT11 sensor
- Detect gas levels using MQ2 sensor with Kalman filter smoothing
- Detect human presence using PIR motion sensor
- Automatic control of:
  - Lights
  - Fan
  - Air conditioning
- Gas leak detection with buzzer alarm (siren pattern)
- LCD 20x4 display for real-time system status
- Weather condition classification based on temperature & humidity
- UART communication for real-time data monitoring

## Hardware Components
- Arduino Uno
- DHT11 Temperature & Humidity Sensor
- MQ2 Gas Sensor
- PIR Motion Sensor
- LCD 20x4 (I2C)
- Buzzer
- Relay modules (for fan, AC, light)
- Breadboard & jumper wires

## Working Principle
- The system continuously reads environmental data:
  - Temperature & humidity (DHT11)
  - Gas concentration (MQ2 with Kalman filtering)
  - Motion detection (PIR)

- When motion is detected:
  - Lights, fan, and AC are turned ON
  - Devices turn OFF after 10 seconds of no movement

- If gas concentration exceeds threshold:
  - Buzzer alarm is activated with a siren pattern
  - Warning message is displayed on LCD

- LCD displays:
  - Temperature
  - Humidity
  - Weather status
  - Device status

- Data is sent via Serial (UART) every 2 seconds

## Demo
(Add your video link here)

## Images
<img width="775" height="425" alt="Screenshot 2026-04-03 171826" src="https://github.com/user-attachments/assets/aac27413-ce53-4fbe-99a9-d5d2ee592ee5" />
![z7687872064297_d485fe1f74a0fbeb3cc424617e08fd9a](https://github.com/user-attachments/assets/ba5cff4a-3170-4c61-9dd9-9553b349fb48)



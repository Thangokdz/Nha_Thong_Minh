#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <SimpleKalmanFilter.h>

// ===== CẤU HÌNH CHÂN =====
const int DHTPin = 2;       
const int PIRPin = 3;     
const int mq2Pin = A0;   
const int lightPin = 6;   
const int fanPin = 7;      
const int acPin = 8;        
const int buzzerPin = 9;   

// ===== LCD I2C =====
LiquidCrystal_I2C lcd(0x27, 20, 4);

// ===== DHT11 =====
#define DHTTYPE DHT11
DHT dht(DHTPin, DHTTYPE);

// ===== Kalman filter cho MQ2 =====
SimpleKalmanFilter bo_loc(2, 2, 0.001);

// ===== Quản lý thời gian PIR =====
unsigned long lastMotionTime = 0;
const unsigned long offDelay = 10000; // 10 giây tắt thiết bị khi không có người
const int gasThreshold = 300;         // Ngưỡng cảnh báo gas

// ===== Quản lý thời gian còi =====
unsigned long buzzerStartTime = 0;
bool buzzerActive = false;

// ===== Quản lý UART =====
unsigned long lastSerialTime = 0;
const unsigned long serialInterval = 2000; // gửi UART mỗi 2 giây

void setup() {
    Serial.begin(9600);
    dht.begin();
    lcd.init();
    lcd.backlight();

    pinMode(PIRPin, INPUT);
    pinMode(lightPin, OUTPUT);
    pinMode(fanPin, OUTPUT);
    pinMode(acPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
}

void loop() {
    unsigned long currentMillis = millis();

    // ===== ĐỌC CẢM BIẾN =====
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int gasRaw = analogRead(mq2Pin);
    float gasFiltered = bo_loc.updateEstimate(gasRaw);
    int pirState = digitalRead(PIRPin);

    // ===== BẬT/TẮT THIẾT BỊ DỰA TRÊN PIR =====
    if (pirState == HIGH) {
        lastMotionTime = currentMillis;
        digitalWrite(lightPin, HIGH);
        digitalWrite(fanPin, HIGH);
        digitalWrite(acPin, HIGH);
    } else if (currentMillis - lastMotionTime > offDelay) {
        digitalWrite(lightPin, LOW);
        digitalWrite(fanPin, LOW);
        digitalWrite(acPin, LOW);
    }

    // ===== CẢNH BÁO GAS =====
    bool gasAlert = (gasFiltered > gasThreshold);

    if (gasAlert && !buzzerActive) {
        // Bắt đầu kêu còi
        buzzerActive = true;
        buzzerStartTime = currentMillis;
    }

    if (buzzerActive) {
        unsigned long t = currentMillis - buzzerStartTime;
        if (t < 4000) {
            // Giai điệu kiểu còi cứu hỏa
            int phase = (t / 200) % 4;
            switch (phase) {
                case 0: tone(buzzerPin, 1000); break;
                case 1: tone(buzzerPin, 1200); break;
                case 2: tone(buzzerPin, 1400); break;
                case 3: tone(buzzerPin, 1600); break;
            }
        } else {
            noTone(buzzerPin);
            buzzerActive = false; // dừng còi sau 4 giây
        }
    } else {
        noTone(buzzerPin); // đảm bảo tắt khi không cảnh báo
    }

    // ===== XÁC ĐỊNH THỜI TIẾT =====
    String weatherStatus;
    if (temperature > 30 && humidity > 70) {
        weatherStatus = "NONG AM";
    } else if (temperature > 20 && humidity > 70) {
        weatherStatus = "NOM AM";
    } else if (temperature < 20 && humidity < 50) {
        weatherStatus = "LANH";
    } else if (temperature < 15) {
        weatherStatus = "RET";
    } else {
        weatherStatus = "BINH THUONG";
    }

    // ===== HIỂN THỊ LCD 4 DÒNG =====
    if (gasAlert) {
        lcd.setCursor(0,0); lcd.print("!!! CANH BAO GAS !!!");
        lcd.setCursor(0,1); lcd.print(" Nguong gas: "); lcd.print(gasFiltered,1);
        lcd.setCursor(0,2); lcd.print(" Hay kiem tra nha");
    } else {
        // Hoán đổi dòng 0,2 và 0,3
        lcd.setCursor(0,0); lcd.print("NHIET DO: "); lcd.setCursor(10,0); lcd.print(temperature,1); lcd.print((char)223); lcd.print("C");
        lcd.setCursor(0,1); lcd.print("DO AM   : "); lcd.setCursor(10,1); lcd.print(humidity,0); lcd.print("%");
        lcd.setCursor(0,2); lcd.print("THOI TIET: "); lcd.print(weatherStatus);           // dòng 0,3 cũ -> 0,2
        lcd.setCursor(0,3); lcd.print(pirState ? "QUAT,DEN,DIEUHOA:Bat" : "QUAT,DEN,DIEUHOA:Tat"); // dòng 0,2 cũ -> 0,3
    }

    // ===== GỬI UART =====
    if (currentMillis - lastSerialTime >= serialInterval) {
        Serial.print("DoAm: "); Serial.print(humidity,0); 
        Serial.print("% | NhietDo: "); Serial.print(temperature,1); Serial.print("C");
        Serial.print(" | Gas(raw): "); Serial.print(gasRaw);
        Serial.print(" | Gas(filt): "); Serial.print(gasFiltered,1);
        Serial.print(" | Thoi tiet: "); Serial.println(weatherStatus);
        lastSerialTime = currentMillis;
    }
}

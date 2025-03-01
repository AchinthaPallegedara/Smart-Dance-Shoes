# 🔥 Smart Dance Shoes – Machine Learning Powered Motion & Light Sync 🔥

This project brings **technology, dance, and innovation** together! Our **Smart Dance Shoes** use motion sensors, an ESP32-S3 microcontroller, and a machine learning model to sync RGB lighting with movement, **creating a mesmerizing dance experience**.

## 🚀 Features

✅ **Motion Recognition** – MPU-6050 (Accelerometer & Gyroscope)\
✅ **ML-Powered Light Control** – Trained with **Edge Impulse**\
✅ **Embedded System Development** – Written in **C++ for ESP32-S3**\
✅ **Dynamic Light Synchronization** – Real-time response to dance moves\
✅ **Power-Efficient** – Runs on **LiFePO₄ batteries**

## 🎓 Developed for

This project was built as part of our studies at **University of Sri Jayewardenepura**.

## 👨‍💻 Team Members

- **Achintha Pallegedara**
- **Dasith Dilina Malinda**
- **Shasin Theekshana**

## 🙌 Special Thanks

Big shoutout to **Pathum Lakshan, Avinsa Sathsarani, Samindya Janani, and Sewmini Ranasinghe** for their immense support in **training the motion model for the shoe**. Your contributions were key to making this project a reality!

---

## 🛠️ Installation & Setup

### 1️⃣ **Hardware Requirements**

- **ESP32-S3** (Microcontroller)
- **MPU-6050** (Accelerometer & Gyroscope)
- **WS2812B RGB LEDs**
- **LiFePO₄ Battery**
- **Resistors & Wires**

### 2️⃣ **Software & Tools**

- **Arduino IDE / PlatformIO** (for ESP32 programming)
- **Edge Impulse** (for ML model training)
- **Python** (for preprocessing data)
- **C++ / ESP-IDF** (for embedded development)

### 3️⃣ **Setup Instructions**

#### 📌 **1. Clone the Repository**

```bash
git clone https://github.com/AchinthaPallegedara/Smart-Dance-Shoes.git
cd Smart-Dance-Shoes
```

#### 📌 **2. Install Dependencies**

```bash
pip install edge-impulse-cli
```

#### 📌 **3. Upload Code to ESP32**

- Open the `.ino` or `.cpp` file in **Arduino IDE** or **PlatformIO**.
- Select **ESP32-S3** as the board and upload.

#### 📌 **4. Train ML Model on Edge Impulse**

- Collect motion data using MPU-6050.
- Train a classifier using **Edge Impulse**.
- Deploy the trained model to ESP32.

#### 📌 **5. Test & Enjoy!**

Put on the **Smart Dance Shoes**, start moving, and watch the lights sync in real-time! 🎶💃🕺

---

## 🏗️ Project Structure

```
📂 smart-dance-shoes
│── 📁 dancingShoe        # ESP32-S3 Code (C++)
│── 📁 shoe_inferencing    # Edge Impulse ML model files
│── 📁 newDancingshoe-export #Dataset exported for Edge Impulse
│── 📁 hardware        # Circuit diagrams & schematics
│── 📁 docs            # Project documentation
│── README.md          # Project Overview
│── LICENSE            # License file
```

---

## 🤝 Contributing

We welcome contributions! Feel free to fork this repo, create pull requests, or suggest improvements.

---

## 📜 License

This project is licensed under the **MIT License**.

---

## 📢 Let’s Connect!

We’d love to hear your thoughts! How do you see this technology evolving in dance and entertainment? Drop a ⭐ on GitHub if you like the project! 🚀✨

\#MachineLearning #SmartShoes #Innovation #DanceTech #IoT #EmbeddedSystems #EdgeAI #ESP32

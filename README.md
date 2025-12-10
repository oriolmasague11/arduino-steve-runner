# 🕹️ Steve Runner — Arduino OLED Game
A side-scrolling runner game inspired by the Chrome Dino, built for Arduino and displayed on a 128×64 SSD1306 OLED screen.  
Featuring animated characters, smooth physics, score tracking, collision detection, and custom pixel-art sprites.

---

## 🎥 Overview
Steve Runner is a lightweight Arduino game where the player controls **Steve**, who must jump or crouch to avoid obstacles. It includes:

- Start menu with **Play** and **Character Selection**
- Multiple **skins** for Steve
- Parabolic **jump physics**
- Smooth sprite animations (run, jump, crouch, death)
- Randomly generated **obstacles** and **clouds**
- Collision detection and **Game Over screen**
- **Score** and **High Score** system
- Intro logos: Arduino, Adafruit, Elegoo, Chrome

---

## 🛠️ Hardware Requirements

| Component | Notes |
|----------|-------|
| **Arduino UNO / Nano** | or any ATmega328-based board |
| **SSD1306 OLED Display (128×64, I2C)** | Address 0x3C |
| **2 Push Buttons** | Jump and Crouch |
| **10kΩ resistors** | Optional |
| **Jumper wires** | — |

---

## 📡 Wiring Diagram

### OLED → Arduino
| OLED Pin | Arduino Pin |
|----------|-------------|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

(picture of the schema has to be added)

### Buttons
| Button | Arduino Pin | Action |
|--------|-------------|--------|
| Button A | **3** | Jump / Confirm |
| Button B | **2** | Crouch / Menu navigation |

LED on **pin 10** is optional.

---

## ▶️ How to Upload

1. Install required Arduino libraries:  
   - **Adafruit GFX**  
   - **Adafruit SSD1306**

2. Open the `.ino` file in Arduino IDE.
3. Select board and COM port.
4. Upload.
5. Play!

---

## 🎮 Controls

| Action | Button |
|--------|--------|
| Jump | **A** |
| Crouch | **B** |
| Menu select | **B** |
| Menu confirm | **A** |

Jump uses a realistic **parabolic motion**, crouch adjusts hitbox and sprite.

---

## 🏁 Gameplay Features

### 🌥️ Background
- Animated clouds continuously scroll in the background.

### 🦖 Player Animations
- Running cycle with alternating legs  
- Jumping  
- Crouching  
- Death pose  

### 🎯 Obstacles
- Random generation with configurable spacing
- Animated sprites for certain objects
- Object pooling for performance

### 💥 Collision Detection
Custom bounding-box collision system optimized for the OLED.

---

## 🧩 Menus

### 📌 Title Screen
Options:
- **PLAY**
- **MY DINO**

### 🧍 Character Selector
- Shows the selected character's name  
- Press **B** to switch skins  
- Press **A** to choose  

---

## 🖼️ Screenshots
*(Screenshots have to be added)*

---

## 🤝 Credits

Developed by **Oriol Masagué Sans**.  
Pixel art, design, and programming included in this repository.

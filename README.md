# Cable Cam Project

Welcome to the Cable Cam Project! This project was born out of a need for a simple, efficient, and safe cable cam that can carry small cameras, perfect for capturing smooth shots. Dive in to learn how we made it from scratch!

![Cable Cam Intro](docs/intro.jpg)

---

## Electrical Design

The cable cam runs on an Arduino UNO with an ESP8266 WiFi module (since we didn't have access to built-in WiFi solutions at the time). 

**High Power Motors:** With motors drawing more than 6A, we needed a robust motor driver to handle the load. So, we designed our own H-Bridge to keep costs low and overcome shop limitations during the pandemic. 

Check out our custom PCB design that brings it all to life:
![H-Bridge Design](docs/h_bridge.jpg)

---

## Software

We developed an Android interface to control the cable cam over WiFi—keeping it simple with an "ON" push button and a velocity slider for easy operation.

**Safety First:** Dual ultrasonic sensors on both sides of the cable cam detect obstacles like poles to protect the system and your valuable camera equipment.

The Arduino handles:
- **Serial Communication** with the WiFi module.
- **Sensor Interruptions** for collision detection.
- **H-Bridge Control** for motor operations.

Given the high loads on the motors during acceleration and deceleration, speed control is gradual to protect our 3D-printed parts. The code is optimized to avoid any blocking functions, making everything time-efficient and safe.

Check out the wiring diagram:
[Wiring Diagram](docs/wiring_diagram.pdf)

---

### Demo

Watch the cable cam in action! Hear the powerful motor driven by our H-Bridge and Android app, with safety sensors ensuring smooth stops.
![Electrical Demo Video](docs/electrical.mp4)

---

## Final Assembly

Our prototype is built with MDF and 3D-printed parts but is designed for aluminum sheet construction for greater durability. 

Want to build your own? Grab the blueprints and assmeble it:
[Blueprints](docs/blueprints.pdf)
[Instructions](docs/Assembly_Instructions.pdf)


---

## Final Demo

Now, see the cable cam in full motion!
![Cable Cam Demo](docs/cable_cam.mp4)

---

This README gives you the complete journey from concept to creation, with a focus on both functionality and safety. Enjoy exploring the project!

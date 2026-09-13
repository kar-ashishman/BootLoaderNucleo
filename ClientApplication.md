# 📘 Bootloader ClientApp

This project implements a **Python-based serial communication client** for interacting with a microcontroller bootloader.  
It supports **authorization**, **target testing**, **bootloader version fetching**, and **flashing the target**.

---

## ⚙️ Features

- **Authorization** using seed-key handshake  
- **Test Target** by blinking onboard LED  
- **Fetch Bootloader Version** from the device  
- **Menu-driven interface** for user interaction  
- **Serial communication** with configurable COM port and baud rate  

---

## 🔑 Authorization Flow

The client authenticates with the target using a **seed-key mechanism**:

```mermaid
flowchart TD
    A[Start Authorization] --> B[Send 0x03 Request Seed Command]
    B --> C{Target Response}
    C -->|0: Ready| D[Receive 4-byte Seed]
    C -->|Else| E[Fail Authorization]
    D --> F[Compute Key = Seed XOR Mask]
    F --> G[Send Key to Target]
    G --> H{Target Response}
    H -->|0: Success| I[Authorization Successful]
    H -->|Else| J[Authorization Failed]
```

```mermaid
sequenceDiagram
    Client->>Target: Send 0x01 (Test Target)
    Target-->>Client: Response 0 (Test Started)
    Note over Target: LED blinks for 5 seconds
    Target-->>Client: Response 0 (Test Completed)
```

```mermaid
sequenceDiagram
    Client->>Target: Send 0x02 (Fetch Version)
    Target-->>Client: Response 0 (Positive Ack)
    Target-->>Client: Send Version Byte
    Client->>Client: Display Bootloader Version
```


# Anomaly-based IDS for RPL-based IoT Networks

Project focused on the design and implementation of a distributed  
anomaly-based Intrusion Detection System (IDS) for IoT networks using RPL,  
developed with Contiki-NG and the Cooja simulator.

---

## Technologies

- Contiki-NG
- Cooja Simulator
- RPL / 6LoWPAN

---

## Project Structure

```text
iot-ids-rpl/
├── contiki-ng/        # Contiki-NG framework (not versioned)
├── firmware/          # IDS module, RPL modifications, attack firmware
│   ├── ids/
│   ├── dis-attack/
│   ├── Makefile
│   └── rpl-icmp6.c
├── simulations/       # Cooja simulation scenarios (.csc)
├── scripts/           # Setup and deployment scripts
├── docs/              # Report and figures
├── requirements.txt
└── README.md
```

---

## Overview

This repository contains:

- An IDS module for detecting DIS-based attacks in RPL networks  
- Integration of the IDS into the RPL control plane  
- Attack firmware to simulate malicious nodes  
- Simulation scenarios for evaluation (baseline and attack cases)  
- A deployment script to integrate all modifications into Contiki-NG  

The IDS performs real-time detection and mitigation by filtering malicious  
DIS messages based on threshold and temporal analysis.

---

## 1. Requirements

- Windows 11
- WSL2 enabled
- Ubuntu 24.04 LTS (WSL)
- Internet access

All development and simulations are performed **inside WSL (Linux)**.

---

## 2. Clone the Repository

```bash
git clone https://github.com/<user>/iot-ids-rpl.git
cd iot-ids-rpl
```
---

## 3. Install System Dependencies

Run the provided setup script:

```bash
./scripts/setup_ubuntu.sh
```

This installs:

- Java 17
- JavaFX (GUI support)
- Build and compilation tools required by Cooja

---

## 4. Clone Contiki-NG

Contiki-NG is not included in this repository and must be cloned locally:

```bash
git clone https://github.com/contiki-ng/contiki-ng.git contiki-ng
```

⚠️ The folder must be named contiki-ng and located in the root of this project.

---

## 5. Install IDS and Firmware Modifications

Run the deployment script:

```bash
cd scripts
chmod +x install_contiki_mods.sh
./install_contiki_mods.sh
```

This script will:

- Copy the IDS module into Contiki-NG
- Replace the RPL implementation file (rpl-icmp6.c)
- Install attack firmware
- Update firmware build configuration

---

## 6. Run Cooja
```bash
cd ../contiki-ng/tools/cooja
./gradlew run
```

---

## 7. Load Simulations

Simulation files are available in:

```bash
simulations/
```

Examples include:

- dis-baseline.csc → normal network behavior
- dis-attack.csc → attack scenario
- S1_dense.csc, S2_medium.csc, etc. → extended scenarios

---

## IDS Functionality

The IDS detects DIS flooding attacks by:

- Monitoring incoming DIS messages per node
- Applying a configurable threshold
- Temporarily blocking suspicious nodes
- Permanently blocking repeated offenders

The detection mechanism is integrated directly into the RPL control plane,
enabling real-time mitigation of malicious traffic.

---

## Reproducibility

This project ensures reproducibility by:

- Keeping Contiki-NG outside version control
- Storing only custom and modified components
- Providing an automated deployment script

---

## Notes
- The deployment script overwrites specific Contiki-NG files
- It is recommended to use a clean Contiki-NG clone
- Simulations are ready to use and do not require installation

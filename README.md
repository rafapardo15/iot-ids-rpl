# Anomaly-based IDS for RPL-based IoT networks

Proyecto académico para el diseño e implementación de un IDS distribuido
basado en anomalías para redes IoT con RPL, utilizando Contiki-NG y Cooja.

## Tecnologías
- Contiki-NG
- Cooja Simulator
- RPL / 6LoWPAN
- Python (análisis de logs)

## Estructura
- firmware/     Código Contiki de los nodos
- simulations/  Escenarios de Cooja
- scripts/      Scripts de análisis y automatización
- docs/         Memoria y figuras

# IoT RPL IDS – Environment Setup and Baseline Simulation

This repository provides the **environment setup and baseline simulation**
required to work with **RPL-based IoT networks** using **Contiki-NG** and the
**Cooja simulator**.  
The IDS design and analysis are intentionally excluded from this document and
are covered in the technical report.

---

## 1. Requirements

- Windows 11
- WSL2 enabled
- Ubuntu 24.04 LTS (WSL)
- Internet access

All development and simulation are performed **inside WSL (Linux)**.

---

## 2. Clone the repository

```bash
git clone https://github.com/<user>/iot-ids-rpl.git
cd iot-ids-rpl
```

## 3. Install system dependencies

Run the provided setup script:

```bash
./scripts/setup_ubuntu.sh
```
This installs:

- Java 17
- JavaFX (GUI support)
- Build and compilation tools required by Cooja

## 4. Clone Contiki-NG

Contiki-NG is not versioned inside this repository and must be cloned locally:

```bash
git clone https://github.com/contiki-ng/contiki-ng.git contiki-ng
```
## 5. Launch Cooja

```bash
cd contiki-ng/tools/cooja
./gradlew run
```

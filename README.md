# Load Cell Weight Measurement with HX711 on STM32 NUCLEO F446RE

**Note:** This project is created in STM32CubeIDE. It utilizes the HAL layer for GPIO initialization, the **HX711 communication protocol is implemented via manual bit-banging** to ensure precise timing of the 24-bit data transfer and gain selection.

---

## Overview

This project interfaces a **Strain Gauge Load Cell** with an STM32 Nucleo F446RE via the **HX711 24-bit ADC amplifier**. It provides force measurements by calculating the average of multiple samples, applying a calibration scale, and managing a zero-point tare.

**Key features:**

- **Manual Bit-Banging Protocol:** Implementation of the HX711 synchronous serial interface for data clocking.
- **Configurable Gain:** Software support for Channel A with 128x or 64x gain selection via extra clock pulses.
- **Zero-Point Calibration (Tare):** Functionality to reset the "zero" weight offset at startup.
- **Signed 24-bit Handling:** Logic to convert the 24-bit output of the HX711 into a 32-bit signed integer.

**Core Files:**

- `main.c` – Entry point that initializes the weight sensor and runs the measurement loop.
- `loadcell.c` – Driver logic for GPIO bit-banging, byte shifting, and sensor data retrieval.
- `loadcell.h` – Defines the `loadcell_type` structure and function prototypes.

---

## Hardware Configuration & Connections

### 1. The HX711 Interface
The HX711 requires two-wires: a serial clock (**SCK**) and a data line (**DT**).


* **DT (Data Out):** This pin goes **LOW** when data is ready for retrieval.
* **SCK (Serial Clock):** Used to shift out bits and set the gain/channel for the next conversion.

### 2. Physical Connections

| **HX711 Pin** | **STM32 Pin** | **Function** |
|---------------|---------------|---------------------------|
| VCC           | 3.3V / 5V     | Power Supply              |
| GND           | GND           | Ground                    |
| DT (Data)     | PA2           | Input (Data from HX711)   |
| SCK (Clock)   | PA3           | Output (Clock from STM32) |

> **Note:** The DT pin is configured with an internal **Pull-Up** resistor because the HX711 data line floats high until the sensor is ready to transmit.

---

## System Logic & Timing

The communication follows a specific synchronous serial timing protocol:

1. **Wait for Ready:** The system polls the DT pin until it goes **LOW**, signaling that a new 24-bit conversion is complete.
2. **Bit-Banging:** The SCK pin is toggled 24 times to shift out the data bits (MSB first). 
3. **Gain Selection:** 1 to 3 additional pulses are sent after the 24th bit to configure the gain for the next measurement:
   - **25 pulses:** Channel A, Gain 128
   - **26 pulses:** Channel B, Gain 32 (Not supported in this project, just channel A)
   - **27 pulses:** Channel A, Gain 64
4. **Sign Extension:** Since the HX711 output is 24-bit signed, the bits are shifted into a 32-bit variable and the sign is manually obtained.

---

## Measurement Equations

### 1. Weight Calculation
The final weight is derived by removing the offset and applying the calibration scale factor.

$$Weight = \frac{Reading_{average} - Offset}{Scale}$$

Where:
- $Reading_{average}$ is the raw average of $N$ samples.
- $Offset$ is the value captured during the `tare()` process.
- $Scale$ is the conversion factor determined during calibration.

### 2. Calibration Procedure
To find the `A_scale` factor:
1. Call `tare()` with an empty scale.
2. Place a known weight (e.g., 500g) on the scale.
3. Observe the output of `get_average_value()`.
4. Calculate: $Scale = \frac{Value}{KnownWeight}$.

---

## Reference Materials

- **HX711 Datasheet:** 24-Bit Analog-to-Digital Converter for Weigh Scales.

---

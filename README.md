# Automotive Anti-Lock Braking System (ABS) RTOS Simulation

A bare-metal, preemptive Anti-Lock Braking System (ABS) Electronic Control Unit (ECU) simulation built on the ARM Cortex-M3 architecture using FreeRTOS.

This project demonstrates deterministic vehicle physics, ISO 26262 functional safety mechanisms, and true hardware preemption over a standard sequential software loop. 

## ⚙️ Core Architecture
* **Hardware Target:** ARM Cortex-M3 (Emulated via QEMU `lm3s6965evb`)
* **Kernel:** FreeRTOS (Custom configured)
* **Build System:** GNU Make & `arm-none-eabi-gcc`
* **Drivers:** Custom bare-metal memory-mapped UART and NVIC controllers.

## 🚀 Key Engineering Features

### 1. Deterministic Hardware Interrupts (IRQs)
Bypassed standard blocking functions (like `getchar()`) by wiring the simulated brake pedal directly to the ARM processor's Nested Vector Interrupt Controller (NVIC) on IRQ 5. This allows the system to respond to a braking event in under a microsecond, regardless of the CPU's current workload.

### 2. Proportional Analog Braking (Leaky Integrator)
Implemented a stateful physics engine using a software "Leaky Integrator."
* **Short Press:** Simulates light pedal pressure; triggers smooth, manual deceleration without ABS intervention.
* **Long Press:** Saturates the pressure variable, crossing the ABS threshold and violently preempting the system to trigger the hydraulic release/apply control loop.

### 3. Stateful Preemption & Multithreading
Engineered true concurrency by isolating tasks:
* **Cruise Task (Priority 1):** Maintains current vehicle momentum and telemetry.
* **Emergency Brake Task (Priority 3):** Sleeps infinitely until a hardware interrupt drops a Binary Semaphore. It preempts all low-priority tasks, executes the 100ms ABS physics loop, calculates slip-ratio ($\lambda$), and commands the simulated hydraulic valves.

### 4. Hardware-Level Memory Protection
Configured FreeRTOS Method-2 Stack Boundary mapping. If a rogue calculation in a low-priority thread overflows its allocated 256-word RAM stack, the OS detects the destroyed `0xA5` boundary paint and immediately triggers a hardware lock (`vApplicationStackOverflowHook`) to prevent the vehicle from driving with corrupted memory.

## 🛠️ Execution Instructions
To compile and run the simulation on a Linux/WSL environment with QEMU installed:

`make clean`
`make`
`qemu-system-arm -machine lm3s6965evb -nographic -kernel build/abs_ecu.elf`

* **Hold 'B':** Apply manual brakes.
* **Hold 'H':** Panic stomp (Trigger ABS control loop).
* **Press 'C':** Reset throttle to 100 km/h.
* **Ctrl + A, then X:** Exit QEMU.

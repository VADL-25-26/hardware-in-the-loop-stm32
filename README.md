# hardware-in-the-loop-stm32
STM32H753 firmware for hardware in the loop testing.

## System Overview
Implements FreeRTOS-based system that takes host-generated data as input and transmits data identical to the VectorNav VN-100 IMU in terms of packet protocol and frequency. Guarantees deterministic, hard real-time 200 Hz output by decoupling non-deterministic sim input, using latest state mutex-protected critical section, and tightly bounded latency sources. Includes PWM monitoring with pulse width change detection and signaling.

## Interfacing
Leverages full-duplex USART communication for host software interactions. Plant connection also achieved through USART, with PWM hardware pins being monitored by the STM32. STM32 pinout is as follows:
 - HOST -> STM32: USART3 Rx, PD8 (Bridged via ST-LINK Micro USB port)
 - STM32 -> HOST: USART3 Tx, PD9 (Bridged via ST-LINK Micro USB port)
 - STM32 -> PLANT: USART2 Tx, PA2
 - PLANT -> STM32: TIM2, PA0

## Task Structure
The system employs a three task structure as follows:
1. Output task: Unblocks upon firing of hardware timer interrupt operating at 200 Hz. Transmits IMU data to plant. 
2. Feedback task: Upon a monitored pulse width change greater than a parameterized threshold, signals host to update simulation accordingly.
3. Receive task: Continuously accepts, buffers, and parses bytes of data from host. Upon successful parsing of a full packet, updates latest state object.

## Determinism and Hard Real-Time Guarantees
FreeRTOS allows for manual priority selection, creating a means of preventing preemption of the output task. Additionally, timer-centric firmware architecture implicitly defines output task as top authority over CPU behavior. All tasks explicitly avoid unbounded sources of latency through constant-time loops, avoidance of dynamic memory allocation, and careful isolation of non-deterministic host behavior. Latest state is mutex-protected, with non-blocking access. 

## Testing, Validation, and Results
Task execution time and CPU load were analyzed via GPIO toggling (which introduced a trivial instrumentation overhead of under 1 us) and oscilloscope. Worst-case load analysis was performed to define system characteristics:

<div align="center">
  
| Task | Worst-Case Execution Time (WCET) |
| :--- | ---: |
| Output | < 3.5 ms |
| Feedback | < 1 ms |
| Receive | < 100 us |

</div>

Based on these measurements, the current system hardware and framework can handle a significantly higher data input frequency than necessary to support the 200 Hz output packet. Reframing in terms of a rocket HIL implementation, the system provides a deterministic, closed-loop platform that enforces real-time timing while exercising flight software against realistic I/O firmware and hardware behavior.

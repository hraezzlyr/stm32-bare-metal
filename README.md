# STM32 Bare-Metal

The project will include my own implementations of STM32 peripheral drivers.
The target microcontroller is the STM32F439ZI, based on the ARM Cortex-M4 core.
All features will be implemented and tested using this chip. It will not be
based on vendor-provided libraries such as HAL or LL. Peripheral drivers are
implemented from scratch.

## 🎯 Goals
Improving my skills in the field of embedded systems is one of my goals. I want
to know what happens at the lowest level of a microcontroller. This repository
is intended to show everyone can learn (even me) how microcontrollers work under
the hood. Additionally, there are already many STM32 projects based on HAL or LL
from STMicroelectronics. These reasons are why I decided against using
the vendor's libraries.

## 📟 Target hardware
- MCU: `STM32F439ZI`
- Core: `ARM Cortex-M4`
- Architecture: `ARMv7-M`
- Devkit: `STM32 NUCLEO-F439ZI`

## 🛠️ Toolchain
- Language: C11
- Compiler: GNU Arm Embedded GCC (`arm-none-eabi-gcc`)
- Debugger: GDB
- Debug server: OpenOCD
- Debug interface: SWD
- Debug probe: TBD
- Build system: TBD (most likely `Makefile`)

## ✇ Current status of drivers
| Driver | Status | Description |
|:--------|:-------:|:-------------:|
| RCC    | In progress | Reset and clock control |
| GPIO   | Planned | General-purpose I/O |
| EXTI   | Planned | External interrupts and events |
| UART/USART | Planned | Serial communication |
| SPI    | Planned | Serial peripheral interface |
| I2C    | Planned | I2C communication |
| TIM    | Planned | General-purpose timers |
| PWM    | Planned | PWM generation using timers |
| DMA    | Planned | Direct memory access |
| ADC    | Planned | Analog-to-digital conversion |
| DAC    | Planned | Digital-to-analog conversion |
| RTC    | Planned | Real-time clock |
| WWDG/IWDG | Planned | Watchdogs |
| FLASH  | Planned | Internal Flash interface |
| LCD    | Planned | LCD interface |

## 🔗 Dependencies
As I wrote, these drivers are written without STM32 HAL or LL. I don't use:
- STM32 HAL
- STM32 LL
- STM32CubeMX generated peripheral code

Yet I have decided to avoid the drudgery and use header files of ARM CMSIS core
and STM32F439ZI peripheral definitions. At this stage, the startup code and
linker script are vendor-provided and will be studied in more detail later.
So these are not mine:
- ARM CMSIS Core
- STM32 CMSIS Device headers
- STM32 startup code
- STM32 system source
- linker script

## 📋 Project Structure
```text
stm32-bare-metal/
├── drivers
│   └── rcc
│       ├── inc
│       │   └── rcc.h
│       └── src
│           └── rcc.c
├── vendor/
│   ├── cmsis/
│   │   └── inc/
│   │       ├── core_cm4.h
│   │       ├── cmsis_compiler.h
│   │       ├── cmsis_gcc.h
│   │       ├── cmsis_version.h
│   │       └── mpu_armv7.h
│   │
│   └── stm32/
│       ├── inc/
│       │   ├── stm32f4xx.h
│       │   ├── stm32f439xx.h
│       │   └── system_stm32f4xx.h
│       ├── src/
│       │   └── system_stm32f4xx.c
│       ├── startup/
│       │   └── startup_stm32f439xx.s
│       └── linker/
│           └── stm32f439zitx_FLASH.ld
│
├── .gitignore
├── LICENSE
├── main.c
└── README.md
```
- `vendor/cmsis/` — ARM CMSIS Core headers.
- `vendor/stm32/` — STM32F4 device headers, system initialization,
  startup code and linker script provided by STMicroelectronics.

## 🏗️ Building
TODO - Build system is not implemented yet.

## 🪲 Flashing / Debugging
TODO - Flashing / Debugging systems are not implemented yet.

## 📝 License
Take a look at the License file.
> [!NOTE]
> Vendor files retain the appropriate licenses of their authors.
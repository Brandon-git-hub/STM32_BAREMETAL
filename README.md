
[Github Repo](https://github.com/Brandon-git-hub/STM32_BAREMETAL)

## Devices

**燒錄工具:** NUCLEO-F767ZI

**Target MCU:** STM32F072VBT6

## 安裝環境

### 🧩 一、安裝 Arm GNU Toolchain（最通用、官方版本）

官方來源：
🔗 **[Arm Developer – GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)**

選擇：

> **Windows x86_64 hosted cross compiler for Arm Cortex-M and Cortex-R processors (bare-metal target)**

檔名大約是：

```
arm-gnu-toolchain-14.3.rel1-mingw-w64-i686-arm-none-eabi.exe
```

測試:
```powershell
arm-none-eabi-gcc --version
arm-none-eabi-objcopy --version
arm-none-eabi-size --version
```

若看到：

```
arm-none-eabi-gcc (Arm GNU Toolchain 13.2.Rel1) ...
```

代表環境就緒 ✅


#### 🧱 編譯指令

```bash
arm-none-eabi-gcc main.c -o main.elf -mcpu=cortex-m0 -mthumb
arm-none-eabi-size main.elf
arm-none-eabi-objcopy -O binary main.elf main.bin
```

你應該會得到：

```
text    data     bss     dec     hex filename
  16       0       0      16       10 main.elf
```

以及 `main.bin`。


### ✅ 檢查最重要一點

要確定 **這支編譯器能生成 Cortex-M0 Thumb 指令集**。
可用：

```bash
arm-none-eabi-objdump -d main.elf | more
```

會看到像：

```
00000000 <main>:
   0:   b480        push    {r7}
   2:   af00        add     r7, sp, #0
   4:   e7fe        b.n     4 <main+0x4>
```

這就是 Thumb 指令（非 x86），表示完全正確。


### 燒錄
STM32_Programmer_CLI -c port=SWD freq=18000  sn=066EFF485570854967101553 -w main.bin 0x08000000 -rst
```
      -------------------------------------------------------------------
                       STM32CubeProgrammer v2.20.0
      -------------------------------------------------------------------

ST-LINK SN  : 066EFF485570854967101553
ST-LINK FW  : V2J39M27
ST-LINK FW  : V2J39M27
Board       : NUCLEO-F767ZI
Voltage     : 3.25V
SWD freq    : 950 KHz
Connect mode: Normal
Reset mode  : Software reset
Device ID   : 0x448
Revision ID : Rev Z
Device name : STM32F07x
Flash size  : 128 KBytes
Device type : MCU
Device CPU  : Cortex-M0
BL Version  : 0xA1
```


## 專案

### Cmake
```bat
@REM 砍掉舊 build（把錯誤的 Cache 清乾淨）
rm build -Recurse -Force
@REM 告訴 CMake 如何讀取您的專案並生成建構腳本
cmake -S . -B build -G "Ninja"
@REM 啟動編譯和連結的過程
cmake --build build -j
@REM 執行燒錄
cmake --build build --target flash
```
| 指令 | 階段 | 參數 | 作用 | 裸機開發中的意義 |
| :--- | :--- | :--- | :--- | :--- |
| `cmake -S . -B build -G "Ninja"` | **配置與生成** | `-S .` | 指定原始碼目錄 (Source Directory) 為當前目錄 (`.`)。 | 告訴 CMake `CMakeLists.txt` 在哪裡。 |
| | | `-B build` | 指定建構目錄 (Build Directory) 為 `build` 子目錄。 | 將所有生成的中間檔案和最終輸出 (.elf, .bin, .hex) 集中存放。 |
| | | `-G "Ninja"` | 指定建構生成器 (Generator) 為 **Ninja**。 | 讓 CMake 輸出高效的 `build.ninja` 腳本，為快速編譯打基礎。 |
| `cmake --build build -j` | **建構與編譯** | `cmake --build build` | 啟動標準 CMake 建構流程，並指向 `build` 目錄。 | 這是啟動編譯的標準化、跨平台方式 (它會自動調用 Ninja)。 |
| | | `-j` | 傳遞給底層建構工具 (Ninja) 的選項。 | 告訴 Ninja **使用電腦所有可用的 CPU 核心**進行平行編譯，極大加速編譯時間。 |




## 1. 安裝環境


### 🧩 一、安裝 Arm GNU Toolchain（最通用、官方版本）

官方來源：
🔗 **[Arm Developer – GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)**

---

#### ✅ 建議下載版本

選擇：

> **Windows x86_64 hosted cross compiler for Arm Cortex-M and Cortex-R processors (bare-metal target)**

檔名大約是：

```
arm-gnu-toolchain-14.3.rel1-mingw-w64-i686-arm-none-eabi.exe
```

---

### 📦 安裝建議

* 路徑建議：

  ```
  C:\Toolchains\arm-gnu\
  ```

  （**不要放在含空白的目錄**如 `Program Files`）
* 勾選：

  * [x] Add to PATH（若沒選，可手動加）

---

### 🔧 驗證環境變數

開 PowerShell：

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

---

### 🧠 二、理解它與普通 GCC 的差異

| 指令                      | 用途               | 說明                   |
| ----------------------- | ---------------- | -------------------- |
| `arm-none-eabi-gcc`     | C/C++ 編譯器        | 專為 ARM Cortex-M (裸機) |
| `arm-none-eabi-as`      | 組譯器              | 處理 `.s` 檔            |
| `arm-none-eabi-ld`      | 連結器              | 處理 linker script     |
| `arm-none-eabi-objcopy` | 轉換 ELF → BIN/HEX | 產生可燒錄檔               |
| `arm-none-eabi-size`    | 顯示段大小            | text/data/bss 大小     |
| `arm-none-eabi-gdb`     | 除錯器              | 可連 ST-LINK/OpenOCD   |

它是完整「裸機 ARM 開發工具鏈」。

---

### ⚙️ 三、建立最小測試專案（確認工具鏈可用）

#### 🗂️ 建立結構

在某個資料夾（例如 `D:\toolchain_test`）：

```bash
main.c
```

#### ✍️ 內容

```c
int main(void) {
    while (1) {}
}
```

---

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

---

#### 📘 解釋：

* `-mcpu=cortex-m0` → 告訴編譯器生成對應指令集。
* `-mthumb` → 使用 Thumb 指令集（Cortex-M 必備）。
* `-O0` / `-O2` → 優化選項（裸機常用 `-O2` 或 `-Os`）。
* `-nostdlib` / `-ffreestanding` → 之後在連結時用來避免鏈接 libc。

目前你只是讓編譯器確認能輸出 ARM ELF 檔。
下一步我們就能把這工具鏈導入到你的 **p003_baremetal/Makefile**。

---

### ✅ 四、檢查最重要一點

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


### 五、 燒錄
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
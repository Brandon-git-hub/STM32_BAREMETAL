@echo off
REM =========================================================
REM 完整的裸機建構與燒錄流程 (Windows Batch Script)
REM 確保您的環境變數已包含 cmake, ninja, arm-none-eabi-gcc 和 STM32_Programmer_CLI。
REM =========================================================

REM 1. 設定編碼 (修正 STM32CubeProgrammer CLI 亂碼問題)
chcp 65001 > nul
echo.
echo [1/4] 設定編碼為 UTF-8 (chcp 65001)

REM 2. 砍掉舊 build（把錯誤的 Cache 清乾淨）
echo.
echo [2/4] 清理舊 build 目錄...
RMDIR /S /Q build

REM 3. 配置 (告訴 CMake 如何讀取專案並生成建構腳本)
echo.
echo [3/4] 配置 CMake 與生成 Ninja 腳本...
REM 重要：這裡必須傳入 Toolchain File 的路徑，-DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake
cmake -S . -B build -G "Ninja" 

IF ERRORLEVEL 1 (
echo.
echo 錯誤:CMake 配置失敗，請檢查 CMakeLists.txt 或 Toolchain File 路徑。
GOTO END
)

REM 4. 建構 (啟動編譯、連結)
echo.
echo [4/4] 啟動編譯與連結...
cmake --build build -j

IF ERRORLEVEL 1 (
echo.
echo 錯誤：編譯或連結失敗，請檢查原始碼。
GOTO END
)

REM 5. 執行燒錄 (Flash)
echo.
echo [5/5] 執行燒錄目標...
cmake --build build --target flash

IF ERRORLEVEL 1 (
echo.
echo 錯誤：燒錄失敗，請檢查線材連線或 STM32CubeProgrammer CLI 輸出。
GOTO END
)

echo.
echo =========================================================
echo 流程完成：建構與燒錄皆成功！
echo 程式碼已運行於微控制器上。
echo =========================================================

:END
echo.
pause
@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ================================
echo    XMake VS工程生成工具
echo ================================
echo.

:: 检查xmake是否安装
where xmake >nul 2>nul
if %errorlevel% equ 0 (
    echo [信息] 检测到xmake已安装
    goto :generate_project
) else (
    echo [警告] 未检测到xmake安装
)

:: 询问是否安装xmake
:ask_install
echo.
set /p install_choice="是否安装xmake？(y/n): "
if /i "%install_choice%"=="y" (
    goto :install_xmake
) else if /i "%install_choice%"=="n" (
    echo [错误] 用户选择不安装xmake，无法生成VS工程
    echo 按任意键退出...
    pause >nul
    exit /b 1
) else (
    echo 请输入 y 或 n
    goto :ask_install
)

:: 安装xmake
:install_xmake
echo.
echo [信息] 开始安装xmake...
echo 注意：安装过程可能需要管理员权限
echo.

:: 检查PowerShell是否可用
where powershell >nul 2>nul
if %errorlevel% neq 0 (
    echo [错误] 未找到PowerShell，无法安装xmake
    echo 请手动安装PowerShell后再运行此脚本
    pause >nul
    exit /b 1
)

:: 使用PowerShell执行安装命令
echo 正在执行安装命令...
powershell -Command "irm https://xmake.io/psget.text | iex"

:: 检查安装是否成功
where xmake >nul 2>nul
if %errorlevel% equ 0 (
    echo [成功] xmake安装完成
) else (
    echo [错误] xmake安装失败，请手动安装
    echo 可以访问 https://xmake.io 获取安装指南
    pause >nul
    exit /b 1
)

:: 生成VS工程
:generate_project
echo.
echo [信息] 开始生成VS工程...
echo 执行命令: 
echo xmake f --with_test=y 
echo xmake project -k vs
echo.

:: 检查当前目录是否有xmake.lua文件
if not exist "xmake.lua" (
    echo [警告] 当前目录未找到xmake.lua文件
    echo 请确保在包含xmake.lua的目录中运行此脚本
    set /p continue="是否继续？(y/n): "
    if /i not "!continue!"=="y" (
        exit /b 1
    )
)

:: 执行xmake命令生成VS工程
xmake f --with_test=y 
xmake project -k vs

:: 检查生成结果
if %errorlevel% equ 0 (
    echo.
    echo [成功] VS工程生成完成
    echo 生成的工程文件位于当前目录的vs20xx文件夹中
) else (
    echo.
    echo [错误] VS工程生成失败
    echo 请检查xmake配置和错误信息
)

echo.
echo 按任意键退出...
pause >nul
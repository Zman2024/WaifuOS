@echo off
echo ========== Building Kernel ==========
@wsl make
echo ========== Building Image ===========
@wsl make buildimg
pause



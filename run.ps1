# Compile
& "C:\Src\mingw64\bin\g++.exe" -fdiagnostics-color=always -g -std=c++17 .\main.cpp -o .\NexusPrime.exe -I "C:\Src\SFML-3.1.0\include" -L "C:\Src\SFML-3.1.0\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lwinmm

# Run
if ($LASTEXITCODE -eq 0) {
    .\NexusPrime.exe
}
# NEXUS PRIME

NEXUS PRIME is a multi-utility desktop application built in C++ with SFML and Win32 console support. It combines productivity utilities, mini-games, and a retro-style action game experience into a single suite.

## 🚀 Project Overview

This project bundles several utilities and games into one menu-driven application:

- User registration and login system
- Dashboard with multiple utilities
- Quiz game with three difficulty levels
- Resume generator
- Strong password generator
- Encryption and decryption tools
- Focus watch utility
- Game launcher with:
  - Flappy Bird
  - Dark Knight action game

## 🗂️ Folder Structure

- `main.cpp` — application entry point
- `run.ps1` — helper script to compile and run the project
- `includes/` — implementation files for each feature
  - `registeration.cpp`, `login.cpp`, `logout.cpp`, `dashboard.cpp`
  - `logs.cpp`, `encrypt.cpp`, `decrypt.cpp`, `password_generator.cpp`
  - `enter.cpp`, `resume_generator.cpp`, `quiz.cpp`, `focus_watch.cpp`
  - `flappy_bird.cpp`, `game_launcher.cpp`, `dark_knight.cpp`
- `data/` — runtime data files and assets
  - `accounts.txt`, `highscores.txt`, quiz question files, logs
  - `audio/` — sound assets like `background.wav` and `slash.wav`
  - `fonts/` — application font files
  - `resume/` — generated resume text files

## ✅ Key Features

### Authentication
- Register new users & password protection through encryption
- Login existing users
- Logout and session persistence

### Utilities
- Quiz game with easy, medium, and hard modes
- Resume generator that writes text documents
- Strong password generator
- Simple encryption and decryption utility
- Focus watch timer utility

### Games
- Flappy Bird clone
- Dark Knight action RPG built in `includes/dark_knight.cpp`
  - Background music support
  - Slash attack sound effects
  - Enemy and boss combat
  - Level progression and win/lose states

## 💻 Requirements

- Windows OS
- MinGW `g++` compiler
- SFML 3.1.0 installed
- `winmm` multimedia library for sound support

## 🛠️ Build Instructions

Open PowerShell in the project root folder and run:

```powershell
& "C:\Src\mingw64\bin\g++.exe" -fdiagnostics-color=always -g -std=c++17 .\main.cpp -o .\NexusPrime.exe -I "C:\Src\SFML-3.1.0\include" -L "C:\Src\SFML-3.1.0\lib" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lwinmm
```

If compilation succeeds, start the application with:

```powershell
.\NexusPrime.exe
```

Alternatively, run the helper script:

```powershell
.\run.ps1
```

## 🎮 How to Use

1. Launch the app
2. Register or log in
3. Use the dashboard menu to choose utilities or games
4. In the game launcher, select the Dark Knight game or Flappy Bird

## 📌 Notes

- Make sure `data/fonts/font.TTF` exists for in-game text rendering
- Make sure `data/audio/background.wav` and `data/audio/slash.wav` exist for Dark Knight audio
- Close `NexusPrime.exe` before recompiling to avoid file lock errors

## 🧠 Technical Details

### File handling and persistence
- `data/accounts.txt` stores registered users as `username encrypted_password` pairs.
- Registration checks for duplicate usernames by scanning `data/accounts.txt` with `ifstream`.
- Login validates the entered credentials by encrypting the password and comparing against stored records.
- `ofstream` is opened with `ios::app` when appending new data, ensuring existing content is preserved.
- Error checks verify successful file open operations before any read/write occurs.

### Logging system
- `log()` writes events to three separate log files:
  - `data/logs/login_logs.txt` for login/logout events
  - `data/logs/registeration_logs.txt` for registrations
  - `data/logs/event_logs.txt` for dashboard and game launcher activities
- Each log entry includes the current `USERNAME` and a timestamp created with `std::time` and `std::put_time`.

### Quiz and highscore storage
- Quiz questions are loaded from plain text files in `data/`: `easy.txt`, `medium.txt`, and `hard.txt`.
- The quiz parser uses `getline()` to read question and answer lines sequentially.
- Scores are appended to `data/highscores.txt` after each quiz session.

### Resume generation
- User input is captured with `getline()` to preserve spaces in names, addresses, skills, and experience.
- Generated resumes are written to `data/resume/` as plain `.txt` files.
- `printBullets()` formats resume sections with bullet-style lines.

### Sound and assets
- SFML audio assets are loaded from `data/audio/` at runtime.
- The Dark Knight game loads `background.wav` and `slash.wav` using SFML audio APIs.
- `gFont` is loaded from `data/fonts/font.TTF` for in-game text rendering.

### Architecture and code structure
- `main.cpp` is the entry point and includes all feature modules directly via `#include`.
- Global variables like `USERNAME` and application state are used for cross-module access.
- The dashboard menu is a central hub that invokes utilities and game launchers.
- `includes/dark_knight.cpp` contains a full SFML-powered game loop with state management for splash, menu, story, gameplay, pause, victory, and defeat screens.

## ❤️ Thanks

Enjoy building and expanding NEXUS PRIME! If you want, I can also help you add a polished `About` screen or a new mini-game.

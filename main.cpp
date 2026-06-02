#include <iostream>
#include <fstream>
#include <string>
#include <windows.h> // we are gonna use this to change color of text
#include <iomanip>
#include <ctime> 
#include <cstdlib>
#include <limits>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <optional>


//.wav sound effects for focus watch
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // Links the Windows multimedia library automatically

using namespace std;

string USERNAME = "SYSTEM"; // Track which user is using the system
string temporaryUsername; // Track the most recently registered user for logging
const int KEY = 142071; // Key for encryption and decryption


void registeration();
void login();
void logout(); 
void dashboard();
void passwordGenerator();
void waitForKey();
void resumeLauncher();
void focusWatch();
void log(string action);
void quiz(int difficulty);
void gameLauncher();
void flappyBirdLauncher();
void darkKnight();

string encrypt(string message, int key);
string decrypt(string encryptedMsg, int key);

//only used for cv generation by asma
void printBullets(ofstream &file, string data);



#include "includes/setColor.cpp"
#include "includes/registeration.cpp"
#include "includes/login.cpp"
#include "includes/logout.cpp"
#include "includes/dashboard.cpp"
#include "includes/logs.cpp"
#include "includes/encrypt.cpp"
#include "includes/decrypt.cpp"
#include "includes/password_generator.cpp"
#include "includes/enter.cpp"
#include "includes/resume_generator.cpp"
#include "includes/quiz.cpp"
#include "includes/focus_watch.cpp"
#include "includes/flappy_bird.cpp"

#include "includes/game_launcher.cpp"
#include "includes/dark_knight.cpp"

int main(){

    int choice;
    do
    {
        setColor(1); // one is for blue
        cout << "Welcome to NEXUS PRIME! The Ultimate Utility Suite" << endl;
        setColor(7); // 7 for default white
        cout << "Please select an option:" << endl;
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cin >> choice;
    } while (choice != 1 && choice != 2);

    if (choice == 1) 
    {
        registeration();
        cout << "\nPlease login to access NEXUS PRIME." << endl;
        login();
    }
    else if (choice == 2) 
    {
        login();
    }

    dashboard(); // go to dashboard after successful login
   
}



void dashboard() {
    int utilityChoice; 

    // Use a while loop to keep running until they explicitly choose to exit/logout
    while (true) {
        cout << "\nWelcome, " << USERNAME << endl;
        setColor(10);  // 10 for green
        cout << "You have successfully accessed NEXUS PRIME!" << endl;
        setColor(6); // 6 for yellow
        cout << "\nNow Let's get started..." << endl;
        cout << "Loading..." << endl;
        cout << "NEXUS PRIME is now ready to use!" << endl;
        setColor(7);

        do {
            setColor(1);
            cout << "\nPlease select a utility to use:" << endl;
            setColor(7);
            cout << "1. Quiz" << endl;
            cout << "2. Game" << endl;
            cout << "3. Resume Generator" << endl;
            cout << "4. Strong Password Generator" << endl;
            cout << "5. Encryption" << endl;
            cout << "6. Decryption" << endl;
            cout << "7. Focus Watch" << endl;
            setColor(12);
            cout << "8. Logout" << endl;
            cout << "9. Exit NEXUS PRIME" << endl;
            setColor(7);

            setColor(1);
            cout << "\nEnter your choice: ";
            setColor(7);
            cin >> utilityChoice;

            if (utilityChoice < 1 || utilityChoice > 9) {
                setColor(12);
                cout << "Invalid choice! Please select a valid option." << endl;
                setColor(7);
            }
        } while (utilityChoice < 1 || utilityChoice > 9);

        // options functionalities
        if (utilityChoice == 8) {
            logout();
            login();
            continue;
        }
        else if (utilityChoice == 9) {
            logout();
            return; 
        }
        else if (utilityChoice == 1) {
            cout << "Quiz utility is launching..." << endl;
            log("quiz launcher");

            int difficulty;
            do{
                cout << endl << "Select difficulty level:" << endl;
                cout << "1. Easy" << endl;
                cout << "2. Medium" << endl;
                cout << "3. Hard" << endl;
                cin >> difficulty;
                if (difficulty < 1 || difficulty > 3) {
                    setColor(12);
                    cout << "Invalid choice! Please select a valid option (1-3)." << endl;
                    setColor(7);
                }
            } while(difficulty < 1 || difficulty > 3);
            setColor(10);
            if (difficulty == 1) {
                cout << "You selected Easy difficulty." << endl;
            } else if (difficulty == 2) {
                cout << "You selected Medium difficulty." << endl;
            } else if (difficulty == 3) {
                cout << "You selected Hard difficulty." << endl;
            }
            setColor(7);

            quiz(difficulty); // Call the function to start the quiz

            waitForKey();
        }
        else if (utilityChoice == 2) {
            gameLauncher();
            log("game launcher");
            waitForKey();
        }
        else if (utilityChoice == 3) {
            resumeLauncher();
            log("resume generator");
            waitForKey();
           
        }
        else if (utilityChoice == 4) {
            passwordGenerator();
        }
        else if (utilityChoice == 5) {
            string message;
            int key;
            cout << "\n--- ENCRYPTION ---\n";
            cout << "Enter the message to encrypt: ";
            cin.ignore();
            getline(cin, message);

            cout << "Enter the key (integer): ";
            while (!(cin >> key)) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "[!] Invalid key. Please enter an integer: ";
            }

            string result = encrypt(message, key);
            cout << "\n Data encrypted successfully!\n";
            cout << "Encrypted message: " << result << "\n";
            waitForKey();
        }
        else if (utilityChoice == 6) {
            string encryptedMsg;
            int key;
            cout << "\n--- DECRYPTION ---\n";
            cout << "Enter the encrypted message: ";
            cin.ignore();
            getline(cin, encryptedMsg);

            cout << "Enter the key (integer): ";
            while (!(cin >> key)) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "[!] Invalid key. Please enter an integer: ";
            }
            string result = decrypt(encryptedMsg, key);
            cout << "\n Data decrypted successfully!\n";
            cout << "Decrypted message: " << result << "\n";
            waitForKey();
        }
        else if (utilityChoice == 7) {
            focusWatch();
            waitForKey();
          
        }
    }
}

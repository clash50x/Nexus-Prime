void registeration(){
     
    // Registration logic
    string username, password;
    bool usernameExists;

    do {
        usernameExists = false;
        cout << "Enter username: ";
        cin >> username;

    //checks for existing username
    ifstream checkAccount("data/accounts.txt");
    string existingUsername;

    while (checkAccount >> existingUsername) {
        if (existingUsername == username) {
            usernameExists = true;
            break;
        }
    }
    checkAccount.close();

    if (usernameExists) {
        setColor(12);  // Red color = twelve
        cout << "Username already exists! It must be unique." << endl;
        setColor(7);
    }
    } while (usernameExists); // Loop continues until a unique username is entered.


    cout << "Enter password: ";
    cin >> password;

    ofstream registerAccount("data/accounts.txt", ios::app);
    if (!registerAccount) {
        setColor(12);
        cout << "Error opening file!" << endl;
        setColor(7);
        return;
    }
    // encrypts password
    password = encrypt(password, KEY);
    // stores data in file
    registerAccount << username << " " << password << endl;
    registerAccount.close();
    setColor(10);
    cout << "Registration successful!" << endl;
    temporaryUsername = username; // Store the registered username for logging
    log("registeration");
    setColor(7); // resets clr to default
    return;
}
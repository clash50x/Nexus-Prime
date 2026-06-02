void login()
{
    string user, pass;
    int failedAttempts = 0;

    while (true) {
        cout << "Enter username: ";
        cin >> user;
        cout << "Enter password: ";
        cin >> pass;

        // encrypt the pass to match saved password
        pass = encrypt(pass, KEY);

        ifstream loginAccount("data/accounts.txt");
        if (!loginAccount) {
            setColor(12);
            cout << "Error opening file!" << endl;
            setColor(7);
            return;
        }

        string username, password;
        bool loginSuccess = false;
        while (loginAccount >> username >> password) {
            if (username == user && password == pass) {
                loginSuccess = true;
                break;
            }
        }
        loginAccount.close();

        if (loginSuccess) {
            setColor(10);
            cout << "Login Successful!" << endl;
            setColor(7);
            USERNAME = user; // Global variable for tracking user
            log("login");
            return;
        }

        failedAttempts++;
        setColor(12);
        cout << "Login Failed! " << (3 - failedAttempts) << " attempt(s) remaining." << endl;
        setColor(7);

        if (failedAttempts >= 3) {
            int retryChoice;
            do {
                cout << "\nToo many failed attempts. Please choose an option:" << endl;
                cout << "1. Register a new account" << endl;
                cout << "2. Try login again" << endl;
                cout << "Enter your choice: ";
                cin >> retryChoice;
                if (retryChoice < 1 || retryChoice > 2) {
                    setColor(12);
                    cout << "Invalid choice. Please enter 1 or 2." << endl;
                    setColor(7);
                }
            } while (retryChoice < 1 || retryChoice > 2);

            if (retryChoice == 1) {
                registeration();
            }
            failedAttempts = 0;
        }
    }
}

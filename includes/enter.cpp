void waitForKey() {
    cout << "\nPress Enter to continue...";
    cin.clear();            // Reset any internal error
    
    cin.ignore(10000, '\n'); 
    
    cin.get();              // wait for user to press enter
}


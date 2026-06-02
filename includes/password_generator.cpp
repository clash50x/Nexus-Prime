void passwordGenerator() 
{
    srand(time(0));  // Seed the random number generator with the current time
    const char alphabets[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";  // Character pools separated
    const char digits[] = "0123456789";
    const char specials[] = "!@#$%&*";

    int alphaCount = 52; 
    int digitCount = 10;
    int specialCount = 7;
    int totalChars = alphaCount + digitCount + specialCount;

    char allChars[100];     // Combined pool of all characters
    int idx = 0;
    for (int i = 0; i < alphaCount;   i++) allChars[idx++] = alphabets[i];
    for (int i = 0; i < digitCount;   i++) allChars[idx++] = digits[i];
    for (int i = 0; i < specialCount; i++) allChars[idx++] = specials[i];

    int length;
    cout<< "Enter desired password length (minimum 8): ";
    cin >> length;
   while (length<8) 
    {
        cout<< "Error! Password length must be at least 8 \n";
        cout<< "Enter a valid password: ";
        cin>>length;
    }

    char password[length];   //Declare the password array (max 100 characters + null terminator)
    password[0] = alphabets[rand() % alphaCount];
    password[1] = digits[rand() % digitCount];
    password[2] = specials[rand() % specialCount];

    for (int i=3; i< length; i++)  //remaining slots to be filled with any character in the pool
    {
        password[i] = allChars[rand() % totalChars];
    }

    password[length] = '\0';  // Add null terminator to mark the end of the character array
    
    for (int i=length - 1; i>0; i--)   //Shuffle the password array
     {
        int j= rand()%(i + 1); // Pick a random index from 0 to i
        char temp = password[i];   // Swap password[i] and password[j]
        password[i] = password[j];
        password[j] = temp;
    }

    cout<< "Generated Password for you is: " << password << endl;
    log("password generation");
    waitForKey();
}

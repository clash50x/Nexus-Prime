void gameLauncher()
{
    log("game launcher");
    cout << "==========  GAME LAUNCHER ==========" << endl;
    cout << "1. Flappy Bird" << endl;
    cout << "2. Dark Knight" << endl;
    cout << "Select a game to play (enter the number): ";
    
    int choice;
    cin >> choice;

    switch (choice)
    {
        case 1:
            flappyBirdLauncher();
            break;
        case 2:
            darkKnight();
            break;
        default:
            setColor(12);
            cout << "Invalid choice. Please select a valid game." << endl;
            setColor(7);
            break;
    }
}
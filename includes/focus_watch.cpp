void focusWatch()
{
    log("focus watch launcher");
    float targetHours;
    cout << "==========  FOCUS WATCH ==========" << endl;
    cout << "Enter target focus time (in hours): ";
    cin >> targetHours;
    int targetSeconds = static_cast<int>(targetHours * 3600);    // Convert target hours into total seconds
    int elapsedSeconds = 0;

    setColor(11); // Cyan for focus mode
    cout << "\nFocus session started! Stay locked in.\n";
    cout << "----------------------------------------\n";
    setColor(7); // Reset to default color

    // Loop until elapsed time reaches target time
    while (elapsedSeconds <= targetSeconds)
    {
        // Calculate Hours, Minutes, Seconds
        int hours = elapsedSeconds / 3600;
        int minutes = (elapsedSeconds % 3600) / 60;
        int seconds = elapsedSeconds % 60;

        setColor(10); // Green text for active countdown

        // \r resets the cursor to the start of the line. (carriage return)
        cout << "\r Focus Time: " 
             << setfill('0') << setw(2) << hours << ":"
             << setfill('0') << setw(2) << minutes << ":"
             << setfill('0') << setw(2) << seconds;
        
        cout.flush(); // Force terminal update (essential for display)

        setColor(7); // Reset color

        if (elapsedSeconds == targetSeconds) {
            break;
        }

        this_thread::sleep_for(chrono::seconds(1));
        elapsedSeconds++;
    }

    // Session Finished Alert Screen
    setColor(11); // Cyan celebration text
    cout << "\n\nSession Complete! Awesome job staying focused.\n";
    cout << "Take a break, you've earned it!\n";
    setColor(7);

    // Plays fahhhhh
    PlaySound(TEXT("data/audio/fahhh.wav"), NULL, SND_FILENAME | SND_SYNC);
}
void quiz(int difficulty) 
{
    ifstream quizFile;

    if(difficulty == 1) {
        cout << "Loading Easy Quiz..." << endl;
        quizFile.open("data/easy.txt");
    } else if (difficulty == 2) {
        cout << "Loading Medium Quiz..." << endl;
        quizFile.open("data/medium.txt");
    } else if (difficulty == 3) {
        cout << "Loading Hard Quiz..." << endl;
        quizFile.open("data/hard.txt");
    }

    if(!quizFile) {
        setColor(12);
        cout << "Error opening quiz file!" << endl;
        setColor(7);
        return;
    }

    // Count total questions in file
    int totalQuestions = 0;
    string line;
    while(getline(quizFile, line)) {
        totalQuestions++;
    }
    totalQuestions /= 3; // Each question has 3 lines

    // Reset file pointer
    quizFile.clear();
    quizFile.seekg(0);

    int questions[10];
    int correctAnswers = 0;
    int count = 0;

    // Generate 10 unique random question indices
    while (count < 10 && quizFile)
    {
        int randomIndex = rand() % totalQuestions; 
        
        // Check that random index isn't already selected
        bool isDuplicate = false;
        for (int i = 0; i < count; i++) {
            if (questions[i] == randomIndex) { 
                isDuplicate = true;
                break;
            }
        }
        
        if (!isDuplicate) {
            questions[count] = randomIndex;
            count++;
        }
    }

    cout << "========================================" << endl;
    cout << "       WELCOME TO THE C++ QUIZ          " << endl;
    cout << "========================================" << endl << endl;

    for(int i = 0; i < 10; i++) {
        quizFile.clear();  // Clear any file flags (like EOF)
        quizFile.seekg(0); // Move to start of file

        string questionText = "";
        string optionsText = "";
        char correctAnswer = ' ';

        // Calculate the target line for this question
        int targetLine = questions[i] * 3;  // Question N starts at line N*3 
        int currentLine = 0;

        string line;
        while (getline(quizFile, line)) {
            if (currentLine == targetLine) {
                questionText = line; // Line 1: Question
                
                if(getline(quizFile, line)) {
                    optionsText = line; // Line 2: Options
                }
                if(getline(quizFile, line)) {
                    correctAnswer = line[0]; // Line 3: Correct answer
                }
                break;
            }
            currentLine++; 
        }

        // Print the question to the user
        cout << "Q" << (i + 1) << ": " << questionText << endl;
        cout << optionsText << endl;
        
        cout << "Your Answer: ";
        char userAnswer;
        cin >> userAnswer;
        
        if (toupper(userAnswer) == toupper(correctAnswer)) {
            setColor(10); // Green
            cout << "Correct!\n\n";
            setColor(7);  // Reset
            correctAnswers++;
        } else {
            setColor(12); // Red
            cout << "Wrong! The correct answer was " << correctAnswer << "\n\n";
            setColor(7);  // Reset
        }
    }

    cout << "========================================" << endl;
    cout << "Quiz Finished! Your Score: " << correctAnswers << "/10" << endl;
    cout << "========================================" << endl;
    quizFile.close();


    ofstream scoreFile("data/highscores.txt", ios::app);
    if (!scoreFile) {
        setColor(12);
        cout << "Error opening score log file!" << endl;
        setColor(7);
        return;
    }
    scoreFile << USERNAME << " scored " << correctAnswers << "/10 on the " 
              << ((difficulty == 1) ? "Easy" : (difficulty == 2) ? "Medium" : "Hard") 
              << " quiz." << endl;
    scoreFile.close();
}
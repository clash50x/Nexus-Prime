void log(string action)
{
    if(action == "login" || action == "logout") {
        ofstream fout("data/logs/login_logs.txt", ios::app);
        if (!fout) {
            setColor(12);
            cout << "Error opening log file!" << endl;
            setColor(7);
            return;
        }

        // storing seconds in null ptr for
        time_t rawTime = time(nullptr);

        // using built in function to convert seconds into readable format
        tm* timeInfo = localtime(&rawTime);

  
        fout << USERNAME << " logged in on "; // log which user logged in or out
        fout << put_time(timeInfo, "%Y-%m-%d at %H:%M:%S") << endl; //put time structures time using ur layout
        
        
        fout.close(); 

    }

    else if(action == "registeration") {
        ofstream fout("data/logs/registeration_logs.txt", ios::app);
        if (!fout) {
            setColor(12);
            cout << "Error opening log file!" << endl;
            setColor(7);
            return;
        }

        // storing seconds in null ptr for
        time_t rawTime = time(nullptr);

        // using built in function to convert seconds into readable format
        tm* timeInfo = localtime(&rawTime);

  
        fout << temporaryUsername << " registered on "; // log which user registered
        fout << put_time(timeInfo, "%Y-%m-%d at %H:%M:%S") << endl; //put time structures time using ur layout
        
        
        fout.close(); 

    }

    else 
    {
        ofstream fout("data/logs/event_logs.txt", ios::app);
        if (!fout) {
            setColor(12);
            cout << "Error opening log file!" << endl;
            setColor(7);
            return;
        }

        // storing seconds in null ptr for
        time_t rawTime = time(nullptr);

        // using built in function to convert seconds into readable format
        tm* timeInfo = localtime(&rawTime);

  
        fout << USERNAME << " triggered " << action << " on "; // log triggered event
        fout << put_time(timeInfo, "%Y-%m-%d at %H:%M:%S") << endl; //put time structures time using ur layout
        
        
        fout.close(); 

    }
    
}
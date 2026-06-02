void printBullets(ofstream &file, string data)
{
    string temp= "";
    for(int i=0; i<= data.length(); i++)
    {
        if(data[i] == ',' || i == data.length())
        {
            if(!temp.empty())
            {
                file<< "  • " << temp << endl;
            }
            temp= "";
        }
        else
        {
            temp += data[i];
        }
    }
}

void resumeLauncher()
{
    string name, email, phone, address, education, skills, experience;
    cout<< "========= CV GENERATOR =========\n\n";
    cin.ignore(); // Clear input buffer before taking string input
    bool valid;  // name entry
    do{
        valid= true;
        cout << "Please enter Full Name (alphabets only): ";
        getline(cin, name);

        for(int i= 0; i<name.length(); i++) 
        {
            if(!( (name[i]>= 'A' && name[i]<= 'Z') ||(name[i]>= 'a' && name[i]<= 'z') ||name[i] == ' ' )) 
            {
                valid= false;
            }
        }
    if (!valid || name.empty())
        {
            setColor(12); // Red for error
            cout<< "Invalid Name! Try again.\n";
            setColor(7); // Reset to default
        }
    } while (!valid);

    // convert name to uppercase
for(int i=0; i< name.length(); i++) 
{
    if(name[i]>= 'a' && name[i]<= 'z')
        name[i]= name[i] - 32;
}

    cout<< "\nEnter Email: ";    // email entry
    cin>> email;

    while(email.find('@')== -1 || email.find('.')== -1) 
    {
        setColor(12); // Red for error
        cout<< "Invalid Email!" << endl;
        setColor(7); // Reset to default
        cout<< "Enter again: ";
        cin>> email;
    }

    // phone entry
    do{
        valid= true;
        cout<< "\nEnter Phone (11 digits): ";
        cin>> phone;

        if(phone.length() != 11)
            valid= false;

        for(int i=0; i< phone.length(); i++) 
        {
            if(phone[i]< '0' || phone[i]> '9')
                valid = false;
        }

        if(!valid)
        {
            setColor(12); // Red for error
            cout<< "Invalid phone!\n";
            setColor(7); // Reset to default
        }

    }while(!valid);

    cin.ignore();

    cout << "\nEnter Address: ";
    getline(cin, address);

    cout<< "\nEnter Education (Please use commas for multiple inputs):\n";
    cout<< "Example: ICS - College, BSCS - University\n> ";
    getline(cin, education);

    cout<< "\nEnter Skills (Please use commas for multiple inputs):\n";
    cout<< "Example: C++, Communication, Leadership\n> ";
    getline(cin, skills);

    cout<< "\nEnter Experience (Please use commas for multiple inputs):\n";
    cout<< "Example: Intern at XYZ, Freelancer\n> ";
    getline(cin, experience);

    // cv.txt file creation
    string clean_name = name;
    string path = "data/resume/";
    replace(clean_name.begin(), clean_name.end(), ' ', '_'); // Replace spaces with underscores for safety
    string filename = path + clean_name + "_resume.txt";
    
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not create the file!\n";
        return;
    }

    file<< "================================================\n\n";

    int width= 50;    // for name styling in the center
    int spaces= (width - name.length()) / 2;

    for(int i=0; i< spaces; i++)
        {
            file<< " ";
        }

    file<< "🌟 " << name << " 🌟\n\n";
    file<< "================================================\n\n";

    // information box
    file<< "📧 EMAIL   : " << email << endl;
    file<< "📱 PHONE   : " << phone << endl;
    file<< "📍 ADDRESS : " << address << endl;

    // education heading
    file<< "\n🎓 EDUCATION\n\n";
    printBullets(file, education);
    

    // skills heading
    file<< "\n💼 SKILLS\n\n";
    printBullets(file, skills);
    

    // experience heading
    file<< "\n🧑‍💻 EXPERIENCE\n\n";
    printBullets(file, experience);
    

    // footer
    file<< "\n----------------------------------------\n";
    file<< "✨ REFERENCES AVAILABLE ON REQUEST\n";
    file.close();
    setColor(10); // Green for success message
    cout<< "\nCV Generated Successfully!\n";
    setColor(7); // Reset to default
    cout<< "Your resume has been saved as: " << clean_name + "_resume.txt" << endl;

}
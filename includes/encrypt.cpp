string encrypt(string data, int key) 
{
    string encrypted = data;
    for (int i= 0; i < (int)encrypted.size(); i++) 
    {
        encrypted[i] = (char)(data[i] ^ key);
    }
    log("encryption");
    return encrypted;
}
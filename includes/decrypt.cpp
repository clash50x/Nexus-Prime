string decrypt(string data, int key) 
{
    string decrypted = data;
    for (int i= 0; i<(int)decrypted.size(); i++) 
    {
        decrypted[i]= (char)(data[i] ^ key);
    }
    log("decryption");
    return decrypted;
}
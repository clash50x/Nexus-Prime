void logout()
{
    setColor(12);
    cout << "Logging out of NEXUS PRIME. Goodbye!\n\n\n" << endl;
    setColor(7);
    log("logout");
    USERNAME = "SYSTEM"; // Resets the USER before returning to the dashboard loop.
}

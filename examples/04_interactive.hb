hi
// Interactive User Input (pucho) Demo
say "Please enter your name:";
pucho username;

say "Please enter your age:";
pucho userage;

say "Hello, " + username + "! You are " + userage + " years old.";

agar (userage > 18)
{
  say "Status: Eligible to vote!";
}
warna
{
  say "Status: Minor.";
}
bye

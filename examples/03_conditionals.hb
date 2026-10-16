hi
// Control Flow & Branching (agar / warna) Demo
say "--- Conditional Branching Tests ---";

remember score = 85;
say "Student Score: " + score;

agar (score > 90)
{
  say "Grade: A+ (Outstanding)";
}
warna
{
  agar (score > 75)
  {
    say "Grade: A (Very Good)";
  }
  warna
  {
    say "Grade: Needs Improvement";
  }
}

remember x = 42;
agar (x = 42)
{
  say "Equality check passed!";
}
warna
{
  say "Equality check failed!";
}
bye

#define True 1
#define False 0

#define INT_0_PIN 2
#define PLAYER_1_BUTTON 12
#define PLAYER_2_BUTTON 13

#define A_PIN 9
#define B_PIN 10
#define C_PIN 4
#define D_PIN 5
#define E_PIN 6
#define F_PIN 7
#define G_PIN 8

#define BASE_T_ON_TIME 500 // ms
#define TOTAL_POINTS_TO_END 10
#define P1_WIN_STATE 0
#define P2_WIN_STATE 4

volatile short p1_points = 0;
volatile short p2_points = 0;
float t_on = BASE_T_ON_TIME;
short current_pos = 0;
short int_1_triggered = False;
short int_2_triggered = False;
short game_over = False;
short already_scored = False; // to avoid double tapping to button in one round
signed short direction = 1;

short pins[7] = {A_PIN, B_PIN, G_PIN, E_PIN, D_PIN, C_PIN, F_PIN};
short order[8] = {A_PIN, B_PIN, G_PIN, E_PIN, D_PIN, C_PIN, G_PIN, F_PIN};

// handles the interrupt for both players
void handleInterrupt();

// updates on time of the individual segments according to the total_points
void updateTon(short total_points);

// resets the game and starts a new one
void resetFunction();

// prints out intermediate points and end-game-message if end is set
void printPoints(short end);

// toggles all LEDs to the supplied state
void toggleAllLeds(short state);

// ends the Game
void endGame();

// resets interrupt variables
void resetinterrupts();

// sets nextpos for next segment on display
void nextpos();

void setup()
{
  attachInterrupt(digitalPinToInterrupt(INT_0_PIN), handleInterrupt, FALLING); // add interrupt 0 handling function
  for (short i = 0; i < 7; ++i)
  {
    pinMode(pins[i], OUTPUT);
  }
  pinMode(PLAYER_1_BUTTON, INPUT);
  pinMode(PLAYER_2_BUTTON, INPUT);
  Serial.begin(9600);
  Serial.println("Player 1 : Player 2");
}

void loop()
{
  nextpos();
  digitalWrite(order[current_pos], HIGH);
  if (already_scored || game_over)
  {
    printPoints(game_over);
  }
  already_scored = False;
  delay(t_on);
  digitalWrite(order[current_pos], LOW);
}

void updateTon(short total_points)
{
  t_on = BASE_T_ON_TIME / (total_points + 1);
}

void toggleAllLeds(short state)
{
  for (short i = 0; i < 7; ++i)
  {
    digitalWrite(pins[i], state);
  }
}

void resetinterrupts()
{
  int_1_triggered = False;
  int_2_triggered = False;
}

void nextpos()
{
  (direction == 1 ? (current_pos == 7 ? current_pos = 0 : ++current_pos) : (current_pos == 0 ? current_pos = 7 : --current_pos));
}

void endGame()
{
  while (1) // wait until game is reset!
  {
    if (digitalRead(PLAYER_1_BUTTON) && digitalRead(PLAYER_2_BUTTON))
    {
      break;
    }
    toggleAllLeds(HIGH);
    delay(BASE_T_ON_TIME);
    toggleAllLeds(LOW);
    delay(BASE_T_ON_TIME);
  }
  resetFunction();
}

void printPoints(short end)
{
  Serial.print((int)p1_points);
  Serial.print(" - ");
  Serial.println((int)p2_points);
  if (end)
  {
    if (p1_points == p2_points)
    {
      Serial.println("Game has ended in a Draw!");
    }
    else
    {
      Serial.print("Game has ended and Player ");
      Serial.print((int)(p1_points > p2_points ? 1 : 2));
      Serial.println(" won!!!");
    }
    endGame();
  }
}

void handleInterrupt()
{
  if (game_over)
  {
    return;
  }
  int_1_triggered = digitalRead(PLAYER_1_BUTTON);
  int_2_triggered = digitalRead(PLAYER_2_BUTTON);
  if (int_1_triggered && current_pos == P1_WIN_STATE && !already_scored)
  {
    already_scored = True;
    p1_points += 1;
    short total = p1_points + p2_points;
    total >= TOTAL_POINTS_TO_END ? game_over = True : game_over = False;
    updateTon(total);
  }
  else if (int_2_triggered && current_pos == P2_WIN_STATE && !already_scored)
  {
    already_scored = True;
    p2_points += 1;
    short total = p1_points + p2_points;
    total >= TOTAL_POINTS_TO_END ? game_over = True : game_over = False;
    updateTon(total);
  }
  else if (int_1_triggered || int_2_triggered)
  {
    direction = -direction;
  }
  resetinterrupts();
  return;
}

void resetFunction()
{
  Serial.println("Player 1 : Player 2");
  p1_points = 0;
  p2_points = 0;
  t_on = BASE_T_ON_TIME;
  current_pos = 0;
  int_1_triggered = False;
  int_2_triggered = False;
  game_over = False;
  already_scored = False;
  return;
}

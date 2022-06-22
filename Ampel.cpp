#define True 1
#define False 0

// Pins
#define LIGHT_SENSOR_PIN 0
#define INT_0_PIN 2
#define RED_PED_PIN 3
#define GREEN_PED_PIN 5
#define RED_CAR_PIN 6
#define YELLOW_CAR_PIN 9
#define GREEN_CAR_PIN 11

#define DAY_THRESHOLD 400   // Tweak this later in lab!
#define BLINK_DELAY 500     // ms
#define BLINK_COUNT 4       // number of times the traffic light blinks green
#define GREEN_TIME_PED 5000 // ms

#define NIGHT_OFF 0
#define NIGHT_BLINK 1
#define TRAFFIC_GREEN 2
#define TRAFFIC_GREEN_BLINK 3
#define TRAFFIC_YELLOW 4
#define TRAFFIC_RED 5
#define PED_GREEN_BLINK 6
#define TRAFFIC_YELLOW_RED 7

short leds[6] = {RED_PED_PIN, GREEN_PED_PIN, RED_CAR_PIN, YELLOW_CAR_PIN, GREEN_CAR_PIN}; // the pin that the LED is attached to
volatile short int_triggered = False;
volatile short light_intensity;

short light_states[8][6] = {
    // maps leds
    {LOW, LOW, LOW, LOW, LOW},   // night off
    {LOW, LOW, LOW, HIGH, LOW},  // night blink
    {HIGH, LOW, LOW, LOW, HIGH}, // traffic green
    {HIGH, LOW, LOW, LOW, LOW},  // traffic green blink
    {HIGH, LOW, LOW, HIGH, LOW}, // traffic yellow
    {LOW, HIGH, HIGH, LOW, LOW}, // traffic red - ped green
    {LOW, LOW, HIGH, LOW, LOW},  // ped green blink
    {HIGH, LOW, HIGH, HIGH, LOW} // traffic yellow red
};

// traffic light routine for the night
void nightMode();

// traffic light routine for the day
void dayMode();

// sets LEDs to the supplied states
void setLeds(short state);

// requests the light for the pedestrian crossing
void requestPedLight();

void setup()
{
  // declare pins to be an output
  for (short i = 0; i < 6; i++)
  {
    pinMode(leds[i], OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(INT_0_PIN), requestPedLight, HIGH); // add interrupt 0 handling function
  light_intensity = analogRead(LIGHT_SENSOR_PIN);
  Serial.begin(9600);
}

void loop()
{
  light_intensity = analogRead(LIGHT_SENSOR_PIN);
  Serial.println(light_intensity);
  if (light_intensity > DAY_THRESHOLD)
  {
    dayMode();
  }
  else
  {
    nightMode();
  }
}

void setLeds(short state)
{
  for (short i = 0; i < 6; i++)
  {
    digitalWrite(leds[i], light_states[state][i]);
  }
}

void nightMode()
{
  int_triggered = False;
  setLeds(NIGHT_BLINK);
  delay(BLINK_DELAY);
  setLeds(NIGHT_OFF);
  delay(BLINK_DELAY);
}

void dayMode()
{
  setLeds(TRAFFIC_GREEN);
  if (int_triggered)
  {
    Serial.println("Got int!");
    // blink Traffic green light
    short count = BLINK_COUNT;
    while (count--)
    {
      setLeds(TRAFFIC_GREEN_BLINK);
      delay(BLINK_DELAY);
      setLeds(TRAFFIC_GREEN);
      delay(BLINK_DELAY);
    }
    count = BLINK_COUNT;
    setLeds(TRAFFIC_YELLOW);
    delay(2 * BLINK_DELAY);
    setLeds(TRAFFIC_RED);
    delay(GREEN_TIME_PED - (count * 2 * BLINK_DELAY));
    while (count--)
    {
      setLeds(PED_GREEN_BLINK);
      delay(BLINK_DELAY);
      setLeds(TRAFFIC_RED);
      delay(BLINK_DELAY);
    }
    setLeds(TRAFFIC_YELLOW_RED);
    delay(2 * BLINK_DELAY);
    light_intensity = analogRead(LIGHT_SENSOR_PIN);
    if (light_intensity > DAY_THRESHOLD)
    {
      int_triggered = False;
      setLeds(TRAFFIC_GREEN);
      return;
    }
    int_triggered = False;
  }
}

void requestPedLight()
{
  light_intensity = analogRead(LIGHT_SENSOR_PIN);
  if (!int_triggered && light_intensity > DAY_THRESHOLD)
  {
    int_triggered = True;
  }
}

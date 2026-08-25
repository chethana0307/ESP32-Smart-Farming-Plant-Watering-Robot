#include <SPI.h>
#include <MFRC522.h>

// =====================================================
// SMART FARMING ROBOT
//
// - 5 IR sensor line following
// - RFID detection
// - Stop for 5 seconds at RFID
// - Relay GPIO32 ON for same 5 seconds
//
// IR:
// BLACK = 0
// WHITE = 1
//
// OUT1 = FAR LEFT
// OUT5 = FAR RIGHT
// =====================================================


// =====================================================
// IR SENSOR PINS
// =====================================================

const int IR1 = 36;   // OUT1 - Far left
const int IR2 = 39;   // OUT2 - Left
const int IR3 = 34;   // OUT3 - Center
const int IR4 = 35;   // OUT4 - Right
const int IR5 = 33;   // OUT5 - Far right


// =====================================================
// L298N MOTOR DRIVER
// =====================================================

// LEFT motors
const int ENA = 25;
const int IN1 = 26;
const int IN2 = 27;

// RIGHT motors
const int ENB = 14;
const int IN3 = 17;
const int IN4 = 16;


// =====================================================
// MOTOR SPEEDS
// =====================================================

const int BASE_SPEED = 160;
const int SLOW_SPEED = 100;
const int TURN_SPEED = 200;
const int HARD_TURN_SPEED = 250;


// =====================================================
// RFID RC522
// =====================================================

const int RFID_SS  = 5;
const int RFID_RST = 4;

MFRC522 rfid(RFID_SS, RFID_RST);


// =====================================================
// RELAY
// =====================================================

const int RELAY_PIN = 32;

// Your relay has been behaving as ACTIVE LOW
const int RELAY_ON  = LOW;
const int RELAY_OFF = HIGH;

// Stop + relay ON for 5 seconds
const unsigned long RFID_ACTION_TIME = 5000;


// =====================================================
// RFID STATE
// =====================================================

// Prevent same RFID continuously activating while
// robot is still above the card
bool cardAlreadyHandled = false;

unsigned long lastCardSeen = 0;

// Card must disappear for 700 ms before it can
// be accepted as a new detection
const unsigned long CARD_REMOVED_TIME = 700;


// =====================================================
// LAST TURN MEMORY
// =====================================================

enum Direction
{
  NONE,
  LEFT,
  RIGHT
};

Direction lastTurn = NONE;

// Search maximum 5 seconds after losing line
const unsigned long SEARCH_TIMEOUT = 5000;

bool searchingForLine = false;

unsigned long searchStartTime = 0;


// =====================================================
// LEFT MOTOR
// =====================================================

void setLeftMotor(int speed)
{
  speed = constrain(speed, -255, 255);

  if (speed > 0)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else if (speed < 0)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  else
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  analogWrite(ENA, abs(speed));
}


// =====================================================
// RIGHT MOTOR
// =====================================================

void setRightMotor(int speed)
{
  speed = constrain(speed, -255, 255);

  if (speed > 0)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else if (speed < 0)
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  else
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  analogWrite(ENB, abs(speed));
}


// =====================================================
// STOP MOTORS
// =====================================================

void stopMotors()
{
  setLeftMotor(0);
  setRightMotor(0);
}


// =====================================================
// REMEMBER TURNS
// =====================================================

void rememberLeft()
{
  lastTurn = LEFT;
}


void rememberRight()
{
  lastTurn = RIGHT;
}


// =====================================================
// RESET LINE SEARCH
// =====================================================

void resetSearch()
{
  searchingForLine = false;
  searchStartTime = 0;
}


// =====================================================
// SEARCH FOR LOST LINE
// =====================================================

void searchUsingLastTurn()
{
  // Start timer when line first disappears
  if (!searchingForLine)
  {
    searchingForLine = true;
    searchStartTime = millis();

    Serial.println("LINE LOST - searching...");
  }


  // Stop after 5 seconds
  if (millis() - searchStartTime >= SEARCH_TIMEOUT)
  {
    stopMotors();

    Serial.println("SEARCH TIMEOUT - STOP");

    return;
  }


  // Search LEFT
  if (lastTurn == LEFT)
  {
    setLeftMotor(-HARD_TURN_SPEED);
    setRightMotor(HARD_TURN_SPEED);
  }

  // Search RIGHT
  else if (lastTurn == RIGHT)
  {
    setLeftMotor(HARD_TURN_SPEED);
    setRightMotor(-HARD_TURN_SPEED);
  }

  // No previous turn known
  else
  {
    stopMotors();
  }
}


// =====================================================
// CHECK IF RFID CARD IS PRESENT
// =====================================================

bool isRFIDPresent()
{
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  MFRC522::StatusCode status =
    rfid.PICC_WakeupA(bufferATQA, &bufferSize);

  return (
    status == MFRC522::STATUS_OK ||
    status == MFRC522::STATUS_COLLISION
  );
}


// =====================================================
// PRINT RFID UID
// =====================================================

void printRFID()
{
  Serial.print("RFID UID: ");

  for (byte i = 0; i < rfid.uid.size; i++)
  {
    if (rfid.uid.uidByte[i] < 0x10)
    {
      Serial.print("0");
    }

    Serial.print(rfid.uid.uidByte[i], HEX);

    if (i < rfid.uid.size - 1)
    {
      Serial.print(" ");
    }
  }

  Serial.println();
}


// =====================================================
// RFID + RELAY ACTION
// =====================================================

bool checkRFID()
{
  // Check whether a card is physically present
  if (!isRFIDPresent())
  {
    // If card has disappeared for long enough,
    // allow another RFID scan
    if (millis() - lastCardSeen > CARD_REMOVED_TIME)
    {
      cardAlreadyHandled = false;
    }

    return false;
  }


  // Card is physically present
  lastCardSeen = millis();


  // Already handled this card while it is still here
  if (cardAlreadyHandled)
  {
    return false;
  }


  // Read UID
  if (!rfid.PICC_ReadCardSerial())
  {
    return false;
  }


  cardAlreadyHandled = true;


  // =================================================
  // RFID DETECTED
  // =================================================

  Serial.println();
  Serial.println("============================");
  Serial.println("RFID DETECTED");
  Serial.println("============================");

  printRFID();


  // =================================================
  // STOP ROBOT
  // =================================================

  stopMotors();

  // Cancel any current line-lost search timer
  resetSearch();

  Serial.println("Robot STOPPED");


  // =================================================
  // RELAY ON
  // =================================================

  digitalWrite(RELAY_PIN, RELAY_ON);

  Serial.println("Relay ON");


  // =================================================
  // WAIT 5 SECONDS
  //
  // Motors remain stopped and relay remains ON
  // during these entire 5 seconds.
  // =================================================

  delay(RFID_ACTION_TIME);


  // =================================================
  // RELAY OFF
  // =================================================

  digitalWrite(RELAY_PIN, RELAY_OFF);

  Serial.println("Relay OFF");
  Serial.println("5 second action complete");
  Serial.println("Resuming line following");
  Serial.println("============================");
  Serial.println();


  // Halt current RFID card
  rfid.PICC_HaltA();


  return true;
}


// =====================================================
// LINE FOLLOWING
// =====================================================

void followLine()
{
  // =================================================
  // READ SENSORS
  // =================================================

  int s1 = digitalRead(IR1);
  int s2 = digitalRead(IR2);
  int s3 = digitalRead(IR3);
  int s4 = digitalRead(IR4);
  int s5 = digitalRead(IR5);


  // Serial monitor
  Serial.print(s1);
  Serial.print(" ");

  Serial.print(s2);
  Serial.print(" ");

  Serial.print(s3);
  Serial.print(" ");

  Serial.print(s4);
  Serial.print(" ");

  Serial.println(s5);


  // =================================================
  // ALL WHITE
  //
  // 1 1 1 1 1
  //
  // Search according to last turn.
  // Maximum = 5 seconds
  // =================================================

  if (
    s1 == 1 &&
    s2 == 1 &&
    s3 == 1 &&
    s4 == 1 &&
    s5 == 1
  )
  {
    searchUsingLastTurn();

    return;
  }


  // =================================================
  // LINE FOUND AGAIN
  // =================================================

  if (searchingForLine)
  {
    Serial.println("LINE FOUND AGAIN");

    resetSearch();
  }


  // =================================================
  // ALL BLACK
  //
  // 0 0 0 0 0
  //
  // STOP
  // =================================================

  if (
    s1 == 0 &&
    s2 == 0 &&
    s3 == 0 &&
    s4 == 0 &&
    s5 == 0
  )
  {
    stopMotors();

    return;
  }


  // =================================================
  // CENTER
  //
  // 1 1 0 1 1
  // =================================================

  if (
    s1 == 1 &&
    s2 == 1 &&
    s3 == 0 &&
    s4 == 1 &&
    s5 == 1
  )
  {
    setLeftMotor(BASE_SPEED);
    setRightMotor(BASE_SPEED);
  }


  // =================================================
  // SLIGHT LEFT
  //
  // 1 0 1 1 1
  // =================================================

  else if (
    s1 == 1 &&
    s2 == 0 &&
    s3 == 1 &&
    s4 == 1 &&
    s5 == 1
  )
  {
    setLeftMotor(SLOW_SPEED);
    setRightMotor(TURN_SPEED);

    rememberLeft();
  }


  // =================================================
  // HARD LEFT
  //
  // 0 1 1 1 1
  // =================================================

  else if (
    s1 == 0 &&
    s2 == 1 &&
    s3 == 1 &&
    s4 == 1 &&
    s5 == 1
  )
  {
    setLeftMotor(-HARD_TURN_SPEED);
    setRightMotor(HARD_TURN_SPEED);

    rememberLeft();
  }


  // =================================================
  // STRONG LEFT
  //
  // 0 0 1 1 1
  // =================================================

  else if (
    s1 == 0 &&
    s2 == 0 &&
    s3 == 1 &&
    s4 == 1 &&
    s5 == 1
  )
  {
    setLeftMotor(-HARD_TURN_SPEED);
    setRightMotor(HARD_TURN_SPEED);

    rememberLeft();
  }


  // =================================================
  // VERY STRONG LEFT
  //
  // 0 0 0 1 1
  // =================================================

  else if (
    s1 == 0 &&
    s2 == 0 &&
    s3 == 0 &&
    s4 == 1 &&
    s5 == 1
  )
  {
    setLeftMotor(-HARD_TURN_SPEED);
    setRightMotor(HARD_TURN_SPEED);

    rememberLeft();
  }


  // =================================================
  // SLIGHT RIGHT
  //
  // 1 1 1 0 1
  // =================================================

  else if (
    s1 == 1 &&
    s2 == 1 &&
    s3 == 1 &&
    s4 == 0 &&
    s5 == 1
  )
  {
    setLeftMotor(TURN_SPEED);
    setRightMotor(SLOW_SPEED);

    rememberRight();
  }


  // =================================================
  // HARD RIGHT
  //
  // 1 1 1 1 0
  // =================================================

  else if (
    s1 == 1 &&
    s2 == 1 &&
    s3 == 1 &&
    s4 == 1 &&
    s5 == 0
  )
  {
    setLeftMotor(HARD_TURN_SPEED);
    setRightMotor(-HARD_TURN_SPEED);

    rememberRight();
  }


  // =================================================
  // STRONG RIGHT
  //
  // 1 1 1 0 0
  // =================================================

  else if (
    s1 == 1 &&
    s2 == 1 &&
    s3 == 1 &&
    s4 == 0 &&
    s5 == 0
  )
  {
    setLeftMotor(HARD_TURN_SPEED);
    setRightMotor(-HARD_TURN_SPEED);

    rememberRight();
  }


  // =================================================
  // VERY STRONG RIGHT
  //
  // 1 1 0 0 0
  // =================================================

  else if (
    s1 == 1 &&
    s2 == 1 &&
    s3 == 0 &&
    s4 == 0 &&
    s5 == 0
  )
  {
    setLeftMotor(HARD_TURN_SPEED);
    setRightMotor(-HARD_TURN_SPEED);

    rememberRight();
  }


  // =================================================
  // OTHER SENSOR COMBINATIONS
  // =================================================

  else
  {
    // LEFT
    if (s1 == 0 || s2 == 0)
    {
      setLeftMotor(SLOW_SPEED);
      setRightMotor(TURN_SPEED);

      rememberLeft();
    }


    // RIGHT
    else if (s4 == 0 || s5 == 0)
    {
      setLeftMotor(TURN_SPEED);
      setRightMotor(SLOW_SPEED);

      rememberRight();
    }


    // FORWARD
    else
    {
      setLeftMotor(BASE_SPEED);
      setRightMotor(BASE_SPEED);
    }
  }
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);


  // =================================================
  // IR
  // =================================================

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);


  // =================================================
  // L298N
  // =================================================

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();


  // =================================================
  // RELAY
  // =================================================

  pinMode(RELAY_PIN, OUTPUT);

  // Relay OFF when ESP32 starts
  digitalWrite(RELAY_PIN, RELAY_OFF);


  // =================================================
  // RFID
  // =================================================

  // ESP32 VSPI:
  // SCK  = GPIO18
  // MISO = GPIO19
  // MOSI = GPIO23
  // SS   = GPIO5

  SPI.begin(18, 19, 23, RFID_SS);

  rfid.PCD_Init();


  // =================================================
  // READY
  // =================================================

  Serial.println();
  Serial.println("================================");
  Serial.println("SMART FARMING ROBOT READY");
  Serial.println("================================");
  Serial.println("Line following: READY");
  Serial.println("RFID: READY");
  Serial.println("Relay GPIO32: READY");
  Serial.println();
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // =================================================
  // RFID GETS PRIORITY
  // =================================================

  if (checkRFID())
  {
    // RFID action just completed.
    // Start normal line following on next loop.
    return;
  }


  // =================================================
  // NORMAL LINE FOLLOWING
  // =================================================

  followLine();

  delay(10);
}

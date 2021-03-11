// Created by Daniel Hunt


// Initalising the motors to their respective pins Colour coding responds to the layout on the board

char X;

int motor1pin1 = 7; // RED
int motor1pin2 = 8; // BROWN

int motor2pin1 = 4; // YELLOW
int motor2pin2 = 5; // ORANGE

void setup() {
  //The pins connected to the motor controller are set to ouputs
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  Serial.begin(9600);

}
void Forward() {  // As it sounds this function moves both the motors in the forward direction
  digitalWrite(motor1pin1, HIGH);//This configuration sets the Left Motor moving in the forward direction
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, LOW); //This configuration sets the Right Motor moving in the Forward direction
  digitalWrite(motor2pin2, HIGH);

}
void Backwards(){
  digitalWrite(motor1pin1, LOW);//This configuration sets the Left Motor moving in the Backwards direction
  digitalWrite(motor1pin2, HIGH);
  
  digitalWrite(motor2pin1, HIGH);//This configuration sets the Right Motor moving in the Backwards direction
  digitalWrite(motor2pin2, LOW);
  
}
void AntiClockwise(){
  digitalWrite(motor1pin1, LOW);//This configuration sets the Left Motor moving in the Backwards direction
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, LOW);//This configuration sets the Right Motor moving in the Backwards direction
  digitalWrite(motor2pin2, HIGH);
}
void Clockwise(){
  
  digitalWrite(motor1pin1, HIGH);//This configuration sets the Left Motor moving in the forward direction
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, HIGH);//This configuration sets the Right Motor moving in the Backwards direction
  digitalWrite(motor2pin2, LOW);
    
  }


void loop() {
  Forward();
  delay(500);
  Backwards();
  delay(500);
  AntiClockwise();
  delay(500);
  Clockwise();
  delay(500);
  if (Serial.available() > 0){
    int X = Serial.read();
}
  if (X="A"){
    Serial.println(X);
    AntiClockwise();
    } 
  else if (X="W"){
    Serial.println(X);
    Forward();
    }
  else if (X="S"){
    Serial.println(X);
    Backwards();
    }
  else if (X="D"){
    Serial.println(X);
    Clockwise();
    }
  
}

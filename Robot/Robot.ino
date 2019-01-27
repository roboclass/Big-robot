// Моторы подключаются к клеммам M1+,M1-,M2+,M2-  
// Motor shield использует четыре контакта 6,5,7,4 для управления моторами 
#include <Ultrasonic.h>
#define SPEED_LEFT       5
#define SPEED_RIGHT      6
#define DIR_LEFT         4
#define DIR_RIGHT        7
// кф-т поворота и скорость левоого колеса повысить
// Скорость, с которой мы движемся вперёд (0-255)
#define SPEED            70


// Коэффициент, задающий во сколько раз нужно затормозить
// одно из колёс для поворота
#define BRAKE_K          6
#define turn_around      7
#define STATE_FORWARD    0
#define STATE_RIGHT      2
#define STATE_LEFT       3
#define STATE_BACKWARD   4

int state = STATE_FORWARD;

Ultrasonic right(8, 9);
Ultrasonic left (6, 7);
Ultrasonic center (12, 13);
    
void runForward() 
{
    state = STATE_FORWARD;

    // Для регулировки скорости `SPEED` может принимать значения от 0 до 255,
    // чем болше, тем быстрее. 
    analogWrite(SPEED_LEFT, SPEED);
    analogWrite(SPEED_RIGHT, SPEED);

    // Если в DIR_LEFT или DIR_RIGHT пишем HIGH, мотор будет двигать соответствующее колесо
    // вперёд, если LOW - назад.
    digitalWrite(DIR_LEFT, LOW);
    digitalWrite(DIR_RIGHT, HIGH);
}

void steerRight() 
{
    state = STATE_RIGHT;

    // Замедляем правое колесо относительно левого,
    // чтобы начать поворот
    analogWrite(SPEED_RIGHT, SPEED / BRAKE_K);
    analogWrite(SPEED_LEFT, SPEED);

    digitalWrite(DIR_LEFT, HIGH);
    digitalWrite(DIR_RIGHT, HIGH);
}

void steerLeft() 
{
    state = STATE_LEFT;

    analogWrite(SPEED_LEFT, SPEED);
    analogWrite(SPEED_RIGHT, SPEED);

    digitalWrite(DIR_LEFT, LOW);
    digitalWrite(DIR_RIGHT, HIGH);
    delay(250);
}
void steerBack()
{
    state = STATE_BACKWARD;

    analogWrite(SPEED_LEFT, SPEED);
    analogWrite(SPEED_RIGHT, SPEED);

    digitalWrite(DIR_LEFT, LOW);
    digitalWrite(DIR_RIGHT, LOW);
    delay(80);
}

void turn_aroundd(){
    state = turn_around;
    steerLeft();
    delay(250);
}



void setup() 
{
    for(int i = 4; i <= 7; i++)
        pinMode(i, OUTPUT);
    Serial.begin(9600);
    
    runForward();
} 

void loop() 
{ 
    int dist_l = left.Ranging(CM);   // get distance
    int dist_r = right.Ranging(CM);
    int dist_c = center.Ranging(CM);
    
    Serial.print(dist_l);
    Serial.print(" ");
    Serial.print(dist_c);
    Serial.print(" ");
    Serial.println(dist_r);
    
    // В какое состояние нужно перейти?
    int targetState = STATE_FORWARD;
    if (dist_l <= 7){
      targetState = STATE_RIGHT;
    } else if (dist_r <= 7){
      targetState = STATE_LEFT;
    } else if (dist_l <= 5 and dist_r <= 5 or dist_c <= 25){
      targetState = turn_around;
    } else if(dist_l <= 3 and dist_r <= 3 or dist_c <= 10){
      targetState = steerBack;
    } else targetState = STATE_FORWARD;
    if (state == targetState) {
        // мы уже делаём всё что нужно,
        // делаем измерения заново
        return;
    }

    switch (targetState) {
        case STATE_FORWARD:
            runForward();
            break;

        case STATE_RIGHT:
            steerRight();
            break;

        case STATE_LEFT:
            steerLeft();
            break;
        case STATE_BACKWARD:
            steerBack();
            break;
        case turn_around:
            turn_aroundd();
            break;
    }

    // не позволяем сильно вилять на прямой
    delay(60);
}

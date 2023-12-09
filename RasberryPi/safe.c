#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>
#include <softTone.h>
#include <math.h>
#include <string.h>
#include <wiringSerial.h>

#define trigPin 15    
#define echoPin 13    
#define buzzerPin1 12
#define buzzerPin2 18
#define ledPin 14     

// Step Motor
#define CW 0
#define CCW 1
int pin_arr[4] = {26, 16, 20, 21};
int one_two_phase[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1},};
int stepAngles[4] = {30, 120, 160, 50};
int preStepAngle = 0;
int curStepAngle = 0;
int StepMotorReady = TRUE;
pthread_mutex_t StepMotorLock;

// Bluetooth
int serialFile;
#define BAUD_RATE 115200
static const char* UART2_DEV = "/dev/ttyAMA1";

// Button
#define buttonPin 4
int buttonState = 0;
int previousState = 0;
unsigned long startTime = 0;
int isButtonPressed = 0;


// All - temp
int isAlarmOn = 0; 
int isAlarmTriggered = 0;

void projectInit() {
    if(wiringPiSetupGpio() == -1) {printf("error : wiringPiSetupGpio()"); exit(1);}
    pinMode(ledPin, OUTPUT);
    printf("led init complete\n");
    
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    printf("ultrasound sensor init complete\n");
    
    pinMode(buttonPin, INPUT);
    printf("button init complete\n");

    softToneCreate(buzzerPin1);
    softToneCreate(buzzerPin2);
    printf("buzzer init complete\n");

	for(int i = 0 ; i < 4; i++) { pinMode(pin_arr[i], OUTPUT); }
	printf("motor init complete\n");

    if (wiringPiSetup() < 0) {printf("error : wiringPiSetup()\n"); exit(1);} 
	if ((serialFile = serialOpen(UART2_DEV, BAUD_RATE)) < 0) {printf("error : serialOpen()\n"); exit(1);}
	printf("bluetooth init complete\n");
}

void rotateMotor(float angle, int dir) {
	int steps =  (int)(2048.0 / 360 * angle);
	int where = 0;

    for (int i = 0; i < steps; i++){
		if (dir == CW) where = i%4;
		else where = 3 - i%4;

		for (int j=0; j<4; j++) {
			digitalWrite(pin_arr[j], one_two_phase[where][j]);
		}
		delay(3);
	}
}

void serialWriteBytes (const int fd, const char *s) {
	write(fd, s, strlen(s));
}

void *buttonThread() {
    for(;;) {
        buttonState = digitalRead(buttonPin); // 버튼 상태 확인

        if (buttonState == HIGH && previousState == LOW) { // 버튼이 눌렸을 때
            startTime = millis(); // 시간 기록
            isButtonPressed = 1; // 버튼이 눌린 상태로 설정
        } else if (buttonState == LOW && previousState == HIGH) { // 버튼이 놓였을 때
            if (isButtonPressed) {
                isButtonPressed = 0; // 버튼이 눌린 상태 해제

                if (isAlarmOn == 0) {
                    printf("경계 모드 ON\n");
                    isAlarmOn = 1; // 경계 모드 활성화
                } else {
                    printf("경계 모드 OFF\n");
                    isAlarmOn = 0; // 경계 모드 비활성화
                    isAlarmTriggered = 0; // 사이렌이 울리는 플래그 초기화
                    softToneWrite(buzzerPin1, 0); // 소리 끄기
                    softToneWrite(buzzerPin2, 0); // 소리 끄기
                }
            }
        }

        previousState = buttonState;        
    }
}

void *buzzerThread() {
    for (;;) {

        if(isAlarmOn && isAlarmTriggered) {
            softToneWrite(buzzerPin1, 1000); // 부저1을 1000Hz 주파수로 설정
            softToneWrite(buzzerPin2, 2000); // 부저2를 2000Hz 주파수로 설정
            delay(100);
            softToneWrite(buzzerPin1, 0); // 소리 끄기
            softToneWrite(buzzerPin2, 0); // 소리 끄기
            delay(100);
        }        
    }

}

void *ledThread() {
    for(;;) {
        while (isAlarmOn && isAlarmTriggered) {
            digitalWrite(ledPin, LOW); // LED 켜기
            delay(100);
            digitalWrite(ledPin, HIGH); // LED 끄기
            delay(100);
        }
    }
}

void *distanceThread() { // 거리 측정 쓰레드
    for (;;) {
        if (isAlarmOn) {
            // 거리 측정
            digitalWrite(trigPin, LOW);  delayMicroseconds(2);
            digitalWrite(trigPin, HIGH); delayMicroseconds(20);
            digitalWrite(trigPin, LOW);

            while (digitalRead(echoPin) == LOW);
            unsigned long startTime = micros();

            while (digitalRead(echoPin) == HIGH);
            unsigned long travelTime = micros() - startTime;

            int distance = travelTime / 58;
            distance *= 10;

            printf("Distance: %dmm\n", distance);

            // 거리에 도달할 때마다 사진찍음
            if (distance < 50) {
                // 완전히 작업이 끝나고 나야 다음 사진 찍을 수 있음! - 락걸었음
                if(pthread_mutex_trylock(&StepMotorLock)==0){
                    printf("사진 on");
                    curStepAngle = (curStepAngle + 1)%4;
                    pthread_mutex_unlock(&StepMotorLock);
                }
            }

            // 거리에 따른 사이렌 소리 제어
            if (distance < 50 && !isAlarmTriggered) {
                isAlarmTriggered = 1; // 사이렌이 울리도록 플래그 설정
            }

        }

        delay(200); // 측정 주기
    }
    
    return NULL;
}

void* takePictureThread() {
  
	while(1){

		if (preStepAngle != curStepAngle){
			
			if(pthread_mutex_trylock(&StepMotorLock)==0) {

				// // 현재 각도와 입력받은 각도의 차를 구함
				float moveAngle = stepAngles[curStepAngle] - stepAngles[preStepAngle];
				printf("현재 무브앵글 %f\n",moveAngle);

				// 마이너스 방향인 경우
				if (moveAngle < 0) {
					rotateMotor(fabs(moveAngle), CCW);
				}
				// 아닌경우 -> 시계방향으로 회전
				else {
					rotateMotor(moveAngle, CW);
				}

				// 사진 찍으라고 전송
				serialWriteBytes(serialFile, "picture");
				
				// preStepAngle 업데이트
				preStepAngle = curStepAngle;
				delay(2000);

				pthread_mutex_unlock(&StepMotorLock);
			}
			
		}
	}
}



int main() {
    pthread_t distanceThreadID;
    pthread_t takePictureThreadID;
    pthread_t buttonThreadID;
    pthread_t buzzerThreadID;
    pthread_t ledThreadID;

    // 초기화
    projectInit();

    // 거리 측정 쓰레드 생성
    pthread_create(&distanceThreadID, NULL, distanceThread, NULL);
    pthread_create(&takePictureThreadID, NULL, takePictureThread, NULL);
    pthread_create(&buttonThreadID, NULL, buttonThread, NULL);
    pthread_create(&buzzerThreadID, NULL, buzzerThread, NULL);
    pthread_create(&ledThreadID, NULL, ledThread, NULL);

    for(;;) {

    }

    return 0;
}

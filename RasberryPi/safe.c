#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringSerial.h>
int isAlarmOn = 0; 
int isAlarmTriggered = 0;

// Ultrasound Sensor
#define TRIG_PIN     15    
#define ECHO_PIN     13    

// LED
#define LED_PIN      14

// Buzzer
#include <softTone.h>
#define BUZZER_PIN_1 12
#define BUZZER_PIN_2 18

// Step Motor
#define CW 0
#define CCW 1
int pin_arr[4] = {26, 16, 20, 21};
int one_two_phase[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1},};
int stepAngles[2] = {30, 120}; // 카메라가 바라보고 있어야 하는 각도
int preStepAngle = 0;
int curStepAngle = 0;
int stepMotorStart = FALSE;
int StepMotorReady = TRUE;
pthread_mutex_t StepMotorLock;

// Bluetooth
int serialFile;
#define BAUD_RATE 115200
static const char* UART2_DEV = "/dev/ttyAMA1";

// Button
#define BUTTON_PIN 4
int buttonState = 0;
int previousState = 0;
unsigned long startTime = 0;
int isButtonPressed = 0;

// Socket
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 22222
int server_fd, new_socket;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[1024] = {0};


void projectInit() {
    printf("Device preparing...\n");

    if(wiringPiSetupGpio() == -1) {printf("error : wiringPiSetupGpio()"); exit(1);}
    pinMode(LED_PIN, OUTPUT);
    //printf("led init complete\n");
    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    //printf("ultrasound sensor init complete\n");
    
    pinMode(BUTTON_PIN, INPUT);
    //printf("button init complete\n");

    softToneCreate(BUZZER_PIN_1);
    softToneCreate(BUZZER_PIN_2);
    //printf("buzzer init complete\n");

	for(int i = 0 ; i < 4; i++) { pinMode(pin_arr[i], OUTPUT); }
	//printf("motor init complete\n");

    if (wiringPiSetup() < 0) {printf("error : wiringPiSetup()\n"); exit(1);} 
	if ((serialFile = serialOpen(UART2_DEV, BAUD_RATE)) < 0) {printf("error : serialOpen()\n"); exit(1);}
	//printf("bluetooth init complete\n");

    printf("Devices init complete\n");
    printf("Socket preparing...\n");

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(1);
    }

    // 소켓 옵션 설정
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓을 로컬 주소와 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(1);
    }

    printf("Socket init complete\n");

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

void serialWrite(const int fd, const unsigned char c) {
    write(fd, &c, 1); //write 함수를 통해 1바이트 씀
}

void *buttonThread() {
    for(;;) {
        buttonState = digitalRead(BUTTON_PIN); // 버튼 상태 확인

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
                    softToneWrite(BUZZER_PIN_1, 0); // 소리 끄기
                    softToneWrite(BUZZER_PIN_2, 0); // 소리 끄기
                }
                

            }
        }

        previousState = buttonState;        
    }
}

void *buzzerThread() {
    for (;;) {

        if(isAlarmOn && isAlarmTriggered) {
            softToneWrite(BUZZER_PIN_1, 1000); // 부저1을 1000Hz 주파수로 설정
            softToneWrite(BUZZER_PIN_2, 2000); // 부저2를 2000Hz 주파수로 설정
            delay(100);
            softToneWrite(BUZZER_PIN_1, 0); // 소리 끄기
            softToneWrite(BUZZER_PIN_2, 0); // 소리 끄기
            delay(100);
        }        
    }

}

void *ledThread() {
    for(;;) {
        while (isAlarmOn && isAlarmTriggered) {
            digitalWrite(LED_PIN, LOW); // LED 켜기
            delay(100);
            digitalWrite(LED_PIN, HIGH); // LED 끄기
            delay(100);
        }
    }
}

void *distanceThread() { // 거리 측정 쓰레드
    for (;;) {
        if (isAlarmOn) {
            // 거리 측정
            digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
            digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(20);
            digitalWrite(TRIG_PIN, LOW);

            while (digitalRead(ECHO_PIN) == LOW);
            unsigned long startTime = micros();

            while (digitalRead(ECHO_PIN) == HIGH);
            unsigned long travelTime = micros() - startTime;

            int distance = travelTime / 58;
            distance *= 10;

            printf("[0]Distance: %dmm\n", distance);

            // 거리에 도달할 때마다 사진찍음
            if (distance < 50) {
                // 완전히 작업이 끝나고 나야 다음 사진 찍을 수 있음! - 락걸었음 - 확인해봐야
                if(pthread_mutex_trylock(&StepMotorLock)==0){
                    printf("사진 on\n");
                    curStepAngle = 0;
                    stepMotorStart = TRUE;
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

void *takePictureThread() {
  
	while(1){

		if ( stepMotorStart == TRUE ){ // 동작 다시 확인해봐야
			
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
				//serialWriteBytes(serialFile, "P");
                serialWrite(serialFile, 'P');
				
				// preStepAngle 업데이트
				preStepAngle = 1;
				delay(2000);

				pthread_mutex_unlock(&StepMotorLock);
			}
			stepMotorStart = FALSE;
		}
	}
}

void *handleClient(void *arg) {
    printf("connected\n");
    int client_socket = *((int *)arg);
    char buffer[1024];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        if (read(client_socket, buffer, sizeof(buffer)) <= 0) {
            break;
        }

        // 알람을 설정하지 않았을 때는 무시
        if(isAlarmOn) {
            // "Detect" 문자열과 비교
            if (strcmp(buffer, "Detect") == 0) {
                // 경보울리기
                if(pthread_mutex_trylock(&StepMotorLock)==0){
                    curStepAngle = 1;
                    stepMotorStart = TRUE;
                    pthread_mutex_unlock(&StepMotorLock);
                }
                isAlarmTriggered = 1;
            } else {
                printf("%10c[1]Distance: %smm\n",' ',buffer);
            }
        } else {}
    }

    printf("disconnected\n");
    close(client_socket);
    pthread_exit(NULL);
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
    pthread_create(&distanceThreadID,    NULL, distanceThread,    NULL);
    pthread_create(&takePictureThreadID, NULL, takePictureThread, NULL);
    pthread_create(&buttonThreadID,      NULL, buttonThread,      NULL);
    pthread_create(&buzzerThreadID,      NULL, buzzerThread,      NULL);
    pthread_create(&ledThreadID,         NULL, ledThread,         NULL);
 
    printf("server start. listening..\n");

    while (1) {
        // 소켓을 수신 모드로 설정하고 연결 대기
        if (listen(server_fd, 1) < 0) {
            perror("listen error");
            continue;
        }

        // 클라이언트 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept error");
            continue;
        }

        // 새로운 클라이언트를 위한 스레드 생성
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handleClient, &new_socket) != 0) {
            perror("pthread_create error");
            close(new_socket);
            continue;
        }

        // 스레드가 종료될 때까지 대기
        pthread_join(thread_id, NULL);
    }



    return 0;
}

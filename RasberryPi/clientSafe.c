#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

// Ultrasound Sensor
#define TRIG_PIN     15    
#define ECHO_PIN     18  

#define PORT 22222

int main() {

    int client_socket;
    struct sockaddr_in server_address;
    char buffer[1024];

    // 소켓 생성
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // 서버의 IP 주소 설정
    if (inet_pton(AF_INET, "192.168.43.42", &server_address.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    // 클라이언트는 이벤트가 발생할 때마다 서버로 문자열을 전송
    if(wiringPiSetupGpio() == -1) {printf("error : wiringPiSetupGpio()"); exit(1);}
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    while (1) {
        digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(20);
        digitalWrite(TRIG_PIN, LOW);

        while (digitalRead(ECHO_PIN) == LOW);
        unsigned long startTime = micros();

        while (digitalRead(ECHO_PIN) == HIGH);
        unsigned long travelTime = micros() - startTime;

        int distance = travelTime / 58;
        distance *= 10;

        printf("Distance: %dmm\n", distance);

        // if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        //     perror("send");
        //     break;
        // }

        // 거리에 따른 사이렌 소리 제어
        if (distance < 50) {
            printf("DETECTED\n");
            if (send(client_socket, "Detect", strlen("Detect"), 0) == -1) {
                perror("send");
                break;
            }
        }

        delay(200); // 측정 주기
    }

    // 연결 종료
    close(client_socket);

    return 0;
}

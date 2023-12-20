# Safe_Guard 
**임베디드 팀 프로젝트 (2팀)**

### 목차
[1. 프로젝트 소개](#1-프로젝트-소개)<br/>
[2. 개발 일정](#2-개발-일정)<br/>
[3. 전체 시스템 구조](#3-전체-시스템-구조)<br/>
[4. 멤버 구성](#4-멤버-구성)<br/>
[5. 사용 모듈](#5-사용-모듈)<br/>
[6. 사용 스레드](#6-사용-스레드)<br/>
[7. 문제점 및 해결방안](#7-문제점-및-해결방안)<br/> 
[8. 데모 영상](#8-데모-영상)<br/>
[9. 소감](#9-소감)<br/><br/>

## 1. 프로젝트 소개
- 초음파 센서를 이용한 방범 시스템

## 2. 개발 일정
![image](https://github.com/YJunSuk/Safe_Guard/assets/42082748/9f95fd13-5072-499d-94f1-312cfaab54ba)

## 3. 전체 시스템 구조
![시스템 구조](https://lh7-us.googleusercontent.com/8sgYoFK8dgrUsec9hG7DIF7kPEJnbWSt5gWRpa_N-LG2o8tDMMLHd2ww23hG28vNvqyvWT6K2FgXHSqqfSay_bh57wjXuVBv8pTXKvZQvQvKyuGJtqrPr8bfm0O86Ni9XK_h-6JxU1EVswLHen--MTy7YQ=s2048)

1) **스위치** <br/>
감지 시스템의 작동을 시작하거나 중지하는데 사용한다. 사용자가 감지 시스템을 활성화하려고 할 때 이 스위치를 누르면 isAlarmOn의 값을 변경하며 On/Off은 각각 해당 변수값의 1과 0에 해당한다.<br/><br/>
2) **초음파 센서** <br/>
이 센서는 거리를 측정하여 거수자가 감지 범위 안에 들어오는지를 감지한다. 두 개의 센서가 존재해 최대 두가지 방향을 감지 가능하며, 더 넓은 범위를 커버할 수 있다. 거리 값은 mm단위로 값을 읽어와서 distance에 저장한다. <br/><br/>
3) **메인 라즈베리파이** <br/>
초음파 센서에서 받은 신호를 처리하고, 시스템의 다른 구성 요소들을 제어하는 중앙 처리 장치 역할을 한다. isAlarmOn값을 확인하여 On/Off인지 확인하고 On일때 서브 라즈베리파이와의 소켓통신을 위한 서버를 작동시키고 초음파 센서의 값인 distance값을 확인하여 설정된 값 미만으로 탐지되면 isAlarmTriggered의 값을 1로 설정하여 경보를 울리도록 설정한다.<br/><br/>
4) **서브 라즈베리파이** <br/>
초음파 센서에서 받은 신호를 처리하고, isAlarmOn값을 확인하여 On/Off인지 확인하고 On일때 초음파 센서의 값인 distance값을 확인하여 설정된 값 미만으로 탐지되면 메인 라즈베리파이에 ‘Detect’라는 신호를 메인 라즈베리파이에 보내 isAlarmTriggered의 값을 1로 설정하여 경보를 울리도록 설정한다. <br/><br/>
5) **액추에이터 작동**
   - **LED**: isAlarmTriggered의 값이 1이되면 LED를 반복적으로 점등해 경고 신호로 사용한다.
   - **스피커**: isAlarmTriggered의 값이 1이되면 부저를 반복적으로 울려 경고 신호로 사용한다.
   - **스텝 모터**: 카메라의 방향을 조정하기 위해 사용된다. isAlarmTriggered의 값이 1이되면, 감지 신호를 보낸 해당 라즈베리파이의 방향으로 카메라를 회전시키는 역할을 한다.  
   - **CCTV(휴대폰)**: 스텝모터가 신호를 보낸 라즈베리파이의 방향으로 회전한 후에 촬영을 시작한다.<br/><br/>
6) **CCTV(휴대폰)** <br/>
메인 라즈베리파이에서 발생하는 감지 신호를 수신한 후, 이 핸드폰은 촬영된 사진을 데이터베이스에 저장하는 역할을 수행한다. 특히, 수신된 값이 "P"인 경우, CCTV용 핸드폰은 사진을 촬영하여 파이어베이스 스토리지에 저장한다. 이후, 촬영된 시간과 사진의 URL 정보를 파이어스토어 데이터베이스에 저장한다.<br/><br/>
7) **휴대폰(사용자)** <br/>
사용자 핸드폰은 파이어스토어 데이터베이스에 리스너를 등록하고, 데이터베이스의 정보 변화(새로운 사진과 시간 정보)를 감지한다. 이러한 정보 변화를 감지하면, 사용자 핸드폰은 이 정보를 읽어와서 현재 핸드폰의 사진 목록을 업데이트한다. 이러한 과정을 통해 사용자는 실시간으로 CCTV의 상황을 파악할 수 있다. 이 시스템은 사용자의 안전과 보안을 위한 중요한 역할을 수행한다.

## 4. 멤버 구성

| 학번     | 이름   | 내용 |
| -------- | ------ | ---- |
| 20180074 | 권태현 |  CCTV App    |
| 20190491 | 박재성 |  초음파센서, LED, 스피커, 스레드 통합     |
| 20190622 | 손현락 |  초음파센서, 스탭모터, 블루투스, 소켓통신    |
| 20200694 | 양준석 |  유저알림 App    |<br/>

## 5. 사용 모듈
| SR04 초음파센서 모듈 | 노랑 버튼 텍트스위치 |
| --- | --- |
|<img src="https://github.com/Mne-pr/Safe_Guard/assets/42082748/48db0c2e-af33-4713-ba6f-a9d25ddfe0a8" width="200" alt="초음파센서"> | <img src="https://github.com/Mne-pr/Safe_Guard/assets/42082748/e4c7afa8-1707-4690-8b4f-07064eab940a" width="200" height="200" alt="스위치">|

|HC-06 블루투스 모듈|LED|
|--|--|
|<img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/65b9af80-f1bf-4ac6-b6fa-02da6d186d5c" width="200" alt="블루투스모듈">|<img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/981651c1-29e4-454e-82ba-ab1a1264e1eb" width="200" alt="LED">|

|5V 스테핑모터|수동부저|
|--|--|
|<img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/089bfa37-0518-4a37-83dc-ff85223a6ec6" width="200" height="200" alt="스텝모터">|<img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/57cddefd-9733-47fe-ba49-6abbd54907dc" width="200" alt="부저">|

## 6. 사용 스레드
![쓰레드 구조](https://github.com/YJunSuk/Safe_Guard/assets/117419920/47222bfc-4c09-41c0-ac82-2e0be854103d)

| 스레드명            | 역할 |
| ----------------- | ---- |
| buttonThread      |    버튼의 이벤트를 담당하는 스레드  |
| buzzerThread      |   스피커의 동작을 담당하는 스레드   |
| ledThread         |   LED의 동작을 담당하는 스레드    |
| distanceThread    |  초음파센서를 사용해 장애물 간 거리를 측정하는 스레드    |
| takePictureThread |  스텝모터를 회전한 뒤, 사진촬영 신호를 보내는 스레드    |
| handleClient      |    서브 라즈베리파이와의 소켓통신을 담당하는 스레드  |


## 7. 문제점 및 해결방안
1. **초음파 모듈 미작동 오류**</br>
   - 원인 : 저항의 과도한 삽입으로 브레드보드 내에서 합선 발생</br>
   - 해결방안 : 저항을 모두 제거 후 적당한 깊이로 조정 및 배선 정리 실시</br></br>
   
2. **스텝모터 오작동 오류**</br>
   - 원인 : 스텝모터 회전 시 신호를 받는 경우 Race Condition이 발생하여 오작동 발생</br>
   - 해결방안 : StepMotorLock을 이용하여 회전 시 스레드에 락을 걸어서 Race Condition발생을 예방</br></br>
   
3. **휴대폰 연결 시 라즈베리파이 탐지 오류**</br>
   - 원인 : 라즈베리파이에서 사용하는 UART통신은 주로 Bluetooth Classic을 사용하지만 개발 중 BLE패키지를 사용하여 발생</br>
   - 해결방안 : 기존의 BLE패키지를 Classic패키지로 변경</br></br>

## 8. 데모 영상(youtube)

<a href="https://youtube.com/shorts/1B8Lb1gBz9w?feature=share" target="_blank">
  <img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/b981be05-4f95-40a6-91f6-170bfebb0e09" width="200" alt="데모영상">
</a>


## 9. 소감
- 권태현 : 
- 박재성 : 
- 손현락 : 
- 양준석 : 

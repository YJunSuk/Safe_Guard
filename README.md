# 임베디드 팀 프로젝트 (2팀)

### 목차
1. [프로젝트 소개](#프로젝트-소개)<br/>
2. [개발 기간](#개발-기간)<br/>
3. [멤버 구성](#멤버-구성)<br/>
4. [기능](#기능)<br/>
5. [모듈](#모듈)<br/>
6. [전체 시스템 구조](#전체-시스템-구조)<br/>
7. [문제점 및 해결방안](#문제점-및-해결방안)<br/> 
8. [데모 영상](#데모-영상)<br/>
9. [소감](#소감)<br/><br/>

### 프로젝트 소개
-나주에

### 시스템 구조
![](https://lh7-us.googleusercontent.com/8sgYoFK8dgrUsec9hG7DIF7kPEJnbWSt5gWRpa_N-LG2o8tDMMLHd2ww23hG28vNvqyvWT6K2FgXHSqqfSay_bh57wjXuVBv8pTXKvZQvQvKyuGJtqrPr8bfm0O86Ni9XK_h-6JxU1EVswLHen--MTy7YQ=s2048)

1. 스위치(Switch): 감지 시스템의 작동을 시작하거나 중지하는데 사용한다. 사용자가 감지 시스템을 활성화하려고 할 때 이 스위치를 누르면 isAlarmOn의 값을 변경하며 On/Off은 각각 해당 변수값의 1과 0에 해당한다.
2. 초음파 센서(Ultrasonic Sensor): 이 센서는 거리를 측정하여 거수자가 감지 범위 안에 들어오는지를 감지한다. 두 개의 센서가 존재해 최대 두가지 방향을 감지 가능하며, 더 넓은 범위를 커버할 수 있다. 거리 값은 mm단위로 값을 읽어와서 distance에 저장한다. 
3. 메인 라즈베리파이(Main Raspberry Pi): 초음파 센서에서 받은 신호를 처리하고, 시스템의 다른 구성 요소들을 제어하는 중앙 처리 장치 역할을 한다. isAlarmOn값을 확인하여 On/Off인지 확인하고 On일때 서브 라즈베리파이와의 소켓통신을 위한 서버를 작동시키고 초음파 센서의 값인 distance값을 확인하여 설정된 값 미만으로 탐지되면 isAlarmTriggered의 값을 1로 설정하여 경보를 울리도록 설정한다.
4. 서브 라즈베리파이(Sub Raspberry Pi): 초음파 센서에서 받은 신호를 처리하고, isAlarmOn값을 확인하여 On/Off인지 확인하고 On일때 초음파 센서의 값인 distance값을 확인하여 설정된 값 미만으로 탐지되면 메인 라즈베리파이에 ‘Detect’라는 신호를 메인 라즈베리파이에 보내 isAlarmTriggered의 값을 1로 설정하여 경보를 울리도록 설정한다. 
5. 액추에이터 작동
   - LED: isAlarmTriggered의 값이 1이되면 LED를 반복적으로 점등해 경고 신호로 사용한다.
   - 스피커(Speaker): isAlarmTriggered의 값이 1이되면 부저를 반복적으로 울려 경고 신호로 사용한다.
   - 스텝 모터(Stepper Motor): 카메라의 방향을 조정하기 위해 사용된다. isAlarmTriggered의 값이 1이되면, 감지 신호를 보낸 해당 라즈베리파이의 방향으로 카메라를 회전시키는 역할을 한다.  
   - CCTV(카메라): 스텝모터가 신호를 보낸 라즈베리파이의 방향으로 회전한 후에 촬영을 시작한다.

### 1. 개발 기간


### 멤버 구성

| 학번     | 이름   | 내용 |
| -------- | ------ | ---- |
| 20180074 | 권태현 |  CCTV App    |
| 20190491 | 박재성 |  초음파센서, LED, 스피커, 스레드 통합     |
| 20190622 | 손현락 |  초음파센서, 스탭모터, 블루투스, 소켓통신    |
| 20200694 | 양준석 |  유저알림 App    |

### 기능

### 모듈
<p align="center">
  <img src="https://github.com/Mne-pr/Safe_Guard/assets/42082748/48db0c2e-af33-4713-ba6f-a9d25ddfe0a8" width="300" alt="초음파센서">
</p>
<p align="center"><strong>SR04 초음파센서 모듈</strong></p>

<p align="center">
  <img src="https://github.com/Mne-pr/Safe_Guard/assets/42082748/e4c7afa8-1707-4690-8b4f-07064eab940a" width="300" alt="스위치">
</p>
<p align="center"><strong>노랑 버튼 택트스위치</strong><p/>

<p align="center">
  <img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/65b9af80-f1bf-4ac6-b6fa-02da6d186d5c" width="300" alt="블루투스모듈">
</p>
<p align="center"><strong>HC-06 블루투스 모듈</strong><p/>

<p align="center">
  <img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/981651c1-29e4-454e-82ba-ab1a1264e1eb" width="300" alt="LED">
</p>
<p align="center"><strong>LED</strong><p/>

<p align="center">
  <img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/089bfa37-0518-4a37-83dc-ff85223a6ec6" width="300" alt="스텝모터">
</p>
<p align="center"><strong>5V 스테핑모터</strong><p/>

<p align="center">
  <img src="https://github.com/YJunSuk/Safe_Guard/assets/42082748/57cddefd-9733-47fe-ba49-6abbd54907dc" width="300" alt="부저">
</p>
<p align="center"><strong>수동부저</strong></p>

### 전체 시스템 구조
- **시스템 구조**
![시스템 구조](https://github.com/YJunSuk/Safe_Guard/assets/117419920/0361c188-5f3c-4c84-b4a2-5e30d5c2eb9c)
- 라즈베리파이
  - Actuator : 초음파, LED, 스피커
- App
  - CCTV 기능
  - 알림감지 기능
- Firebase
  - 찍힌 이미지 관리
------------------------------------------------------
- **스레드 구조**
![쓰레드 구조](https://github.com/YJunSuk/Safe_Guard/assets/117419920/47222bfc-4c09-41c0-ac82-2e0be854103d)

### 문제점 및 해결방안

### 데모 영상

### 소감




### 추가기능(시간 남을 시)
- 라즈베리파이 2개 사용(초음파 센서 여러개)
- 스마트폰에서 알림 끌 수 있게 작동(LED, 스피커)
### 주제발표 
- PPT 제작 및 발표 : 박재성

- 자료 조사 및 작성 : 손현락, 권태현, 양준석

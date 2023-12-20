# 임베디드 팀 프로젝트 (2팀)

### 목차
[0. 프로젝트 소개](#-프로젝트-소개)<br/>
[1. 개발 기간](#-개발-기간)<br/>
[2. 멤버 구성](#-멤버-구성)<br/>
[3. 기능](#-기능)<br/>
[4. 모듈](#-모듈)<br/>
[5. 전체 시스템 구조](#-전체-시스템-구조)<br/>
[6. 문제점 및 해결방안](#-문제점-및-해결방안)<br/> 
[7. 데모영상](#-데모영상)<br/>
[8. 소감](#-소감)<br/><br/>

### 프로젝트 소개
초음파 센서와 카메라를 이용한 방범 시스템<br/>

### 개발 기간


### 멤버 구성

**손현락**<br/>
초음파센서, 스탭모터, 블루투스, 소켓통신

**박재성**<br/>
초음파센서, LED, 스피커, 스레드 통합 
   
**권태현**<br/>
CCTV App

**양준석**<br/>
유저알림 App

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

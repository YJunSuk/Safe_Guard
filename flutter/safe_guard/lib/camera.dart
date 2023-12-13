import 'dart:convert';
import 'dart:typed_data';
import 'dart:io';

import 'package:camera/camera.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_storage/firebase_storage.dart';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:logger/logger.dart';
import 'package:permission_handler/permission_handler.dart';

import 'image_picker.dart';

Logger log = Logger(printer: PrettyPrinter());
const String addr = "98:D3:11:FD:D3:E6";

class Camera extends StatefulWidget {
  final CameraDescription camera;
  const Camera({super.key, required this.camera});

  @override
  State<Camera> createState() => _CameraState();
}

class _CameraState extends State<Camera> {
  late CameraController _controller;
  late Future<void> _initializeControllerFuture;

  @override
  void initState() {
    super.initState();
    _controller = CameraController(
      widget.camera,
      ResolutionPreset.medium,
    );
    _initializeControllerFuture = _controller.initialize();
  }

  @override
  void dispose() {
    super.dispose();
    _controller.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(),
      body: SingleChildScrollView(
        child: Column(
          children: [
            buttonSection(),
            // connectableList(),
          ],
        ),
      ),
    );
  }

  Widget buttonSection() {
    return ElevatedButton(
      onPressed: () {
        // connectToDevice(addr);
        takePicture();
      },
      child: const Text("Connect")
    );
  }

  void connectToDevice(String address) async {
    try {
      BluetoothConnection connection = await BluetoothConnection.toAddress(address);
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: const Text("연결 완료")));
      log.i("연결 완료");

      connection.input!.listen((Uint8List data) {
        log.i('데이터 수신: ${ascii.decode(data)}');
        if (ascii.decode(data) == "P") {
          takePicture();
        }
        // 사진 촬영
        // 사진 저장
      }).onDone(() {
        ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: const Text("연결 해제")));
        log.i('연결 해제');
      });
    } catch (exception) {
      log.e(exception);
    }
  }

  void takePicture() async {
    await _controller.setFlashMode(FlashMode.off);
    final image = await _controller.takePicture();
    uploadPicture(image.path);
  }

  void uploadPicture(String imagePath) async {
    File image = File(imagePath);
    String imageName = imagePath.split('/').last;

    try{
      // Firebase Storage에 이미지 업로드
      Reference storageReference =
      FirebaseStorage.instance.ref().child('pictures/$imageName');
      UploadTask uploadTask = storageReference.putFile(image);
      await uploadTask.whenComplete(() => null); // 업로드 완료까지 대기

      // 업로드 이미지의 URL 가져오기
      String imageUrl = await storageReference.getDownloadURL();
      log.i(imageUrl);

      // 이미지 url 데이터베이스에 저장
      saveUrlToFirestore(imageUrl);

      // 플래쉬 끄기

    } on Exception catch(e){
      log.e("uploadPicture error", error: e);
    }
  }

  void saveUrlToFirestore(String url) async {
    try{
      // 'pictures' 컬렉션의 문서 수를 얻습니다.
      QuerySnapshot querySnapshot = await FirebaseFirestore.instance.collection('pictures').get();
      int documentCount = querySnapshot.docs.length;
      String newDocumentId = documentCount.toString();

      // 새 문서 ID로 문서를 추가합니다.
      FirebaseFirestore.instance.collection('pictures').doc(newDocumentId).set({
        'image': url,
        'time': DateTime.now()
      });
    } on Exception catch(e){
      log.e("saveUrlToFirstore error", error: e);
    }

    log.i('image url 저장 성공');
  }
}

































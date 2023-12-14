import 'dart:io';

import 'package:camera/camera.dart';
import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:safe_guard/firebase_options.dart';
import 'package:safe_guard/camera.dart';


class Home extends StatefulWidget {
  final CameraDescription camera;
  Home({super.key, required this.camera});

  @override
  _HomeState createState() => _HomeState();
}

class _HomeState extends State<Home> {
  final FirebaseFirestore firestore = FirebaseFirestore.instance;

  // Firestore에서 'name' 필드를 가져오는 함수
  Future<String> getTitleName(String docid) async {
    var document = await firestore.collection('pictures').doc(docid).get();
    if (document.exists) {
      return document.data()!['name'] ?? 'No Title';
    } else {
      return 'No Title';
    }
  }

  // Firestore에서 이미지 URL을 가져오는 함수
  Future<String> getImageUrl(String docid) async {
    var document = await firestore.collection('pictures').doc(docid).get();
    if (document.exists) {
      return document.data()!['image'] ?? ''; // 'image' 필드에 URL이 있다고 가정
    } else {
      return ''; // 문서가 존재하지 않을 경우 빈 문자열 반환
    }
  }

  Widget title(String listnum){
    return FutureBuilder<String>(
          future: getTitleName(listnum),
          builder: (context, snapshot){
            if(snapshot.connectionState == ConnectionState.waiting){
              return Text('Loading...');
            }
            return Text(snapshot.data ?? 'No Title');
          }
          );
  }

  Widget image(String listnum){
    return FutureBuilder<String>(
        future: getImageUrl(listnum),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return CircularProgressIndicator(); // 로딩 중
          }
          if (snapshot.hasData && snapshot.data!.isNotEmpty) {
            return Center(
              child: Image.network(snapshot.data!), // 이미지 표시
            );
          } else {
            return const Center(
              child: Text('이미지를 불러올 수 없습니다.'),
            );
          }
        },
      );
  }



  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Safe_Guard'),
      ),
      body: StreamBuilder<QuerySnapshot>(
        stream: firestore.collection('pictures').snapshots(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return CircularProgressIndicator();
          }
          if (!snapshot.hasData) {
            return Text('No data available');
          }

          List<DocumentSnapshot> docs = snapshot.data!.docs;

          return ListView.builder(
            itemCount: docs.length,
            itemBuilder: (context, index) {
              Map<String, dynamic> data = docs[index].data() as Map<String, dynamic>;
              String name = data['name'] ?? 'No Title';
              String imageUrl = data['image'] ?? '';

              return Container(
                height: 150,
                child: Row(
                  children: [
                    Expanded(child: Text(name)),
                    imageUrl.isNotEmpty ? Expanded(child: Image.network(imageUrl)) : Container(),
                  ],
                ),
              );
            },
          );
        },
      ),
      floatingActionButtonLocation: FloatingActionButtonLocation.startFloat,
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          Navigator.push(context, MaterialPageRoute(builder: (context){
            return Camera(camera: widget.camera,);
          },
          ));
        },
        tooltip: "CCTV 전환",
        child: Icon(
            Icons.camera_alt_rounded,
            size: 50
        ),
      ),
    );
  }
}
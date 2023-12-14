import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:intl/intl.dart';
import 'package:safe_guard/camera.dart';

class ImageDetailScreen extends StatelessWidget {
  final String imageUrl;
  final DateTime timestamp; // 추가: timestamp를 받아오기 위한 변수

  ImageDetailScreen({required this.imageUrl, required this.timestamp});

  @override
  Widget build(BuildContext context) {
    String formattedDate = DateFormat('yyyy-MM-dd HH:mm').format(timestamp);

    return Scaffold(
      appBar: AppBar(
        title: Text('$formattedDate'), // timestamp를 사용하여 앱바 제목 설정
      ),
      body: Center(
        child: Image.network(
          imageUrl,
          fit: BoxFit.contain,
        ),
      ),
    );
  }
}

class Home extends StatefulWidget {
  final CameraDescription camera;

  Home({Key? key, required this.camera}) : super(key: key);

  @override
  _HomeState createState() => _HomeState();
}

class _HomeState extends State<Home> {
  final FirebaseFirestore firestore = FirebaseFirestore.instance;
  List<DocumentSnapshot> docs = []; // 추가: DocumentSnapshot 리스트 선언
  Future<DateTime> getTimeStamp(String docid) async {
    var document = await firestore.collection('pictures').doc(docid).get();

    return (document.data()!['time'] as Timestamp).toDate();
  }

  Future<String> getImageUrl(String docid) async {
    var document = await firestore.collection('pictures').doc(docid).get();
    if (document.exists) {
      return document.data()!['image'] ?? '';
    } else {
      return '';
    }
  }

  Widget timestamp(String listnum) {
    return FutureBuilder<DateTime>(
      future: getTimeStamp(listnum),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return Text('Loading...');
        }
        String formattedDate =
            DateFormat('yyyy-MM-dd HH:mm').format(snapshot.data!);

        return Text(
          "촬영 일시 : $formattedDate",
          textAlign: TextAlign.center,
          style: TextStyle(fontWeight: FontWeight.bold, fontSize: 16),
        );
      },
    );
  }

  Widget image(String listnum, int index) {
    return FutureBuilder<String>(
      future: getImageUrl(listnum),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return CircularProgressIndicator();
        }
        if (snapshot.hasData && snapshot.data!.isNotEmpty) {
          return GestureDetector(
            onTap: () async {
              DateTime timestamp = await getTimeStamp(docs[index].id);
              // 이미지를 클릭했을 때의 동작 정의
              Navigator.push(
                context,
                MaterialPageRoute(
                  builder: (context) => ImageDetailScreen(
                    imageUrl: snapshot.data!,
                    timestamp: timestamp,
                  ),
                ),
              );
            },
            child: Image.network(
              snapshot.data!,
              fit: BoxFit.cover,
              width: 100,
              height: double.infinity,
            ),
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
        title: Text(
          'Safe_Guard',
          style: TextStyle(fontWeight: FontWeight.bold),
          ),
        backgroundColor: Colors.blueGrey,
      ),
      body: StreamBuilder<QuerySnapshot>(
        stream: firestore.collection('pictures').orderBy('time', descending: true).snapshots(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return CircularProgressIndicator();
          }
          if (!snapshot.hasData) {
            return Text('No data available');
          }

          // docs를 업데이트
          var docs = snapshot.data!.docs;
          docs.sort((doc1, doc2) {
            var time1 = ((doc1.data() as Map<String, dynamic>)['time'] as Timestamp).toDate();
            var time2 = ((doc2.data() as Map<String, dynamic>)['time'] as Timestamp).toDate();
            return time2.compareTo(time1); // 최신순으로 정렬
          });

          return ListView.separated(
            itemCount: docs.length,
            separatorBuilder: (context, index) =>
                SizedBox(height: 10), // Add spacing between items
            itemBuilder: (context, index) {
              Map<String, dynamic> data =
                  docs[index].data() as Map<String, dynamic>;
              String imageUrl = data['image'] ?? '';

              return Card(
                elevation: 5,
                child: Container(
                  height: 150,
                  child: Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: Row(
                      children: [
                        imageUrl.isNotEmpty
                            ? image(
                                docs[index].id, index) // Use the image widget
                            : Container(),
                        SizedBox(width: 10),
                        timestamp(docs[index].id),
                      ],
                    ),
                  ),
                ),
              );
            },
          );
        },
      ),
      floatingActionButtonLocation: FloatingActionButtonLocation.startFloat,
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          Navigator.push(
            context,
            MaterialPageRoute(
              builder: (context) {
                return Camera(
                  camera: widget.camera,
                );
              },
            ),
          );
        },
        tooltip: 'CCTV 전환',
        child: Icon(Icons.camera_alt_rounded, size: 50),
      ),
    );
  }
}

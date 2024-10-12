import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Water alarm clock',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.blue),
        useMaterial3: true,
      ),
      home: const MyHomePage(title: 'Water alarm clock'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  TimeOfDay selectedTime = TimeOfDay.now();

  Future<void> _postData(hours, minutes) async {
    String ip = '0.0.0.0';

    final res = await http.get(Uri.parse('http://62.169.17.47:1029/ip'));
    ip = res.body;

    try {
      final response = await http.post(
        Uri.parse('http://${ip}/alarm'),
        headers: <String, String>{
          'alarmHour': hours,
          'alarmMin': minutes,
        },
      );

      if (response.statusCode == 200) {
        // Successful POST request, handle the response here
        final responseData = jsonDecode(response.body);
        print(responseData);
      } else {
        // If the server returns an error response, throw an exception
        throw Exception('Failed to post data');
      }
    } catch (e) {
      setState(() {});
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Text("Water will be released at: ", style: TextStyle(fontSize: 25)),
            Text(
              "${selectedTime.hour}:${selectedTime.minute}",
              style: TextStyle(fontSize: 50),
            ),
            ElevatedButton(
              child: const Text("Choose Time"),
              onPressed: () async {
                final TimeOfDay? timeOfDay = await showTimePicker(
                  context: context,
                  initialTime: selectedTime,
                  initialEntryMode: TimePickerEntryMode.dial,
                );
                if (timeOfDay != null) {
                  // print(selectedTime);
                  setState(() {
                    selectedTime = timeOfDay;
                  });

                  _postData(
                      timeOfDay.hour.toString(), timeOfDay.minute.toString());
                }
              },
            ),
          ],
        ),
      ),
    );
  }
}

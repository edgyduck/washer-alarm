/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on NodeMCU.

  Note: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right NodeMCU module
  in the Tools -> Board menu!

  For advanced settings please follow ESP examples :
   - ESP8266_Standalone_Manual_IP.ino
   - ESP8266_Standalone_SmartConfig.ino
   - ESP8266_Standalone_SSL.ino

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "*******************************";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "************";
char pass[] = "********";

#define vibrationPin D0
int stopCount = 0;
int stopThreshold = 360; // checking every 1 second for 6 minutes
int startCount = 0;
int startThreshold = 45; // if vibration is detected for 45 seconds means washer was started
long duration; // pulseIn duration
int notificationSent = 1; // once "washer stopped" notification is sent this value is set to 1, after detecting start of washer this is reset to 0
int restPeriod = 0; // to mark the rest period during washing

void setup()
{
  Serial.begin(115200);
  pinMode(vibrationPin, INPUT);
  Blynk.begin(auth, ssid, pass);
  Blynk.notify("\xc4\xae\x72\x65\x6e\x67\x69\x6e\x79\x73\x20\xc4\xaf\x6a\x75\x6e\x67\x74\x61\x73\x2e");  // "Įrenginys įjungtas"
  Serial.println("Let's start.");
}

void loop()
{
  Blynk.run();

  duration = pulseIn(vibrationPin, HIGH); // check for vibration sensor state change
  if (duration != 0)
  {
    // Washer machine is running, sent start notification
    startCount = startCount + 1;
    Serial.print("start count: ");
    Serial.println(startCount);
    Serial.print("Duration: ");
    Serial.println(duration);
    if (startCount == startThreshold && restPeriod == 0 && notificationSent == 1)
    {
      // If multiple times in a row vibration is on
      Serial.print("Washer has started. Sending notification. Start count: ");
      Serial.println(startCount);
      Blynk.notify("\x53\x6b\x61\x6c\x62\x69\x6d\x61\x73\x20\x70\x72\x61\x64\xc4\x97\x74\x61\x73."); // Skalbimas pradėtas
      notificationSent = 0; // reset notification sent marker
    }
    stopCount = 0; // reset stop counter
  }
  else
  {
    stopCount = stopCount + 1;
    Serial.print("stop count: ");
    Serial.println(stopCount);
    Serial.print("Duration: ");
    Serial.println(duration);
    if (stopCount == stopThreshold)
    {
      if (restPeriod == 0 && notificationSent == 0)
      {
        restPeriod = 1; // first time stop counter is reached means it's calm/rest washing stage
        Serial.print("Rest stage detected. Stop count: ");
        Serial.println(stopCount);
      }
      else if (notificationSent == 0) // so we don't sent notification on initial power on
      {
        Blynk.notify("\x4c\x61\x69\x6b\x61\x73\x20\x64\xc5\xbe\x69\x61\x75\x74\x69\x20\x72\xc5\xab\x62\x75\x73!"); //"Laikas džiauti rūbus"
        Serial.println("Laundry washing complete.");
        notificationSent = 1;
        restPeriod = 0;
      }      
    }
    startCount = 0;
  } 
  delay(1000);
}

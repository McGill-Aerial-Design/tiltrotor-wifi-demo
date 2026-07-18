#include <ESP8266WiFi.h>
#include <DNSServer.h>

WiFiServer server(80);
DNSServer dnsServer;
IPAddress apIP(172, 217, 28, 1);

int flap = 0;  // 0, 1, 2
int roll = 0;  // -2, -1, 0, 1, 2
int pitch = 0; // -2, -1, 0, 1, 2
int yaw = 0;   // -2, -1, 0, 1, 2
bool horizontalMode = false;


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Secret Santa", "azertyui");
  dnsServer.start(53, "*", apIP);
}

void loop() {
  WiFiClient client = server.available();

  if (client){
    while (Serial.available()) {Serial.read();}

    unsigned int t = 0;
    while(!client.available()){
      delay(1);
      t += 1;
      if (t >= 3000){
        client.stop();
        t=0;
        return;
      }
    }



    String req = client.readStringUntil('\n');
    req.remove(req.length() - 10);
    


    if (req.startsWith("GET /robots.txt") or req.startsWith("GET /boaform/admin") or req.startsWith("GET /sitemap.xml") or req.startsWith("GET /.well-known/security.txt") or req.startsWith("GET /.vscode/sftp.json") or req.startsWith("GET /client/get_targets") or req.startsWith("GET /showLogin.cc") or req.startsWith("GET http://") or req.startsWith("GET /HNAP") or req.startsWith("GET /config/getuser?") or req.startsWith("GET /.git/config") or req.startsWith("GET jira/jira-webapp-dist/pom.properties")) {
      client.print("HTTP/1.1 404 Not Found\n");
      client.print("Connection: close\n\n");
    }

    else if (req.startsWith("GET /favicon.ico")) { // will deal with this later
      client.print("HTTP/1.1 404 Not Found\n");
      client.print("Connection: close\n\n");
    }

    else {
      client.print("HTTP/1.1 200 OK\n");
      client.print("Connection: close\n\n");
      client.print("<!doctype html>\n");
      client.print("<html style='text-align:center;'>\n");
      client.print("<head> <meta charset=\"UTF-8\">\n");
      client.print("<title>MAD Tilt-Rotor Control</title>\n");
      client.print("</head> <body>\n");

      client.print("<h1>McGill Aerial Design</h1>\n");
      client.print("<p>This page allows you to send commands to the tilt-rotor drone to demonstrate how the control surfaces work.</p>\n");

      client.print("<p><b>Current state:</b></p>\n");

      if (horizontalMode) {client.print("<p>transition mode: horizontal flight</p\n");}
      else {client.print("<p>transition mode: vertical flight</p\n");}

      client.print("<p>flaps: " + String(flap) + " / 2</p>\n");

      String s = "neutral";
      if (pitch == -2) {s = "strong down";}
      if (pitch == -1) {s = "weak down";}
      if (pitch == 1) {s = "weak up";}
      if (pitch == 2) {s = "strong up";}
      client.print("<p>pitch: " + s + "</p>\n");

      s = "neutral";
      if (roll == -2) {s = "strong left";}
      if (roll == -1) {s = "weak left";}
      if (roll == 1) {s = "weak right";}
      if (roll == 2) {s = "strong right";}
      client.print("<p>roll: " + s + "</p>\n");



      client.print("<br><br><br><p>Disclaimer to anyone complaining about how ugly this website looks:</p>\n");
      client.print("<p>This was made by someone in mech eng. I don't give a **** about graphics or UI.</p>\n");
      client.print("<p>Just be happy I know enough C++ to make the motors turn the right way without starting a fire.</p>\n");
      client.print("</body></html>\n");

    }
    client.println();
    client.flush();
    client.stop();
  }
}

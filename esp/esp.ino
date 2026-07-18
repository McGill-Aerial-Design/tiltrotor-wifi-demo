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

const char* redirectToHome = "<br><p>You will be automatically redirected to the main page in 5 seconds.</p>\n<meta http-equiv = 'refresh' content = '2; url = /' />\n";
const char* niceTryBro = "<h1>Nice Try Bro</h1>\n<p>Did you really think I would let you break stuff by injecting arbitrary values everywhere?</p>\n<p>Needless to say, your request was blocked</p>\n";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("MAD tiltrotor demo");
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
    


    if (req.startsWith("POST") or req.startsWith("GET /robots.txt") or req.startsWith("GET /boaform/admin") or req.startsWith("GET /sitemap.xml") or req.startsWith("GET /.well-known/security.txt") or req.startsWith("GET /.vscode/sftp.json") or req.startsWith("GET /client/get_targets") or req.startsWith("GET /showLogin.cc") or req.startsWith("GET http://") or req.startsWith("GET /HNAP") or req.startsWith("GET /config/getuser?") or req.startsWith("GET /.git/config") or req.startsWith("GET jira/jira-webapp-dist/pom.properties")) {
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

      if (req.startsWith("GET /transition")){
        client.print("<p>Transitioning...</p>");
        client.print(redirectToHome);
        horizontalMode = !horizontalMode;
      }

      else if (req.startsWith("GET /flap")){
        int newVal = req.substring(9).toInt();
        if (newVal >= 0 and newVal <= 2) {
          client.print("<p>Flaps set to " + String(newVal) + "</p>");
          flap = newVal;
        }
        else {client.print(niceTryBro);}
        client.print(redirectToHome);
      }

      else if (req.startsWith("GET /pitch")){
        int newVal = req.substring(10).toInt();
        if (newVal >= -2 and newVal <= 2) {
          client.print("<p>Pitch set to " + String(newVal) + "</p>");
          pitch = newVal;
        }
        else {client.print(niceTryBro);}
        client.print(redirectToHome);
      }

      else if (req.startsWith("GET /roll")){
        int newVal = req.substring(9).toInt();
        if (newVal >= -2 and newVal <= 2) {
          client.print("<p>Roll set to " + String(newVal) + "</p>");
          roll = newVal;
        }
        else {client.print(niceTryBro);}
        client.print(redirectToHome);
      }

      else if (req.startsWith("GET /yaw")){
        int newVal = req.substring(9).toInt();
        if (newVal >= -2 and newVal <= 2) {
          client.print("<p>Yaw set to " + String(newVal) + "</p>");
          yaw = newVal;
        }
        else {client.print(niceTryBro);}
        client.print(redirectToHome);
      }



      else {
        client.print("<h1>McGill Aerial Design</h1>\n");
        client.print("<p>This page allows you to send commands to the tilt-rotor drone to demonstrate how the control surfaces work.</p>\n");

        client.print("<p>Current mode: ");

        if (horizontalMode) {client.print("horizontal");}
        else {client.print("vertical");}
        client.print(" flight   <a href='/transition'><button>Transition</button></a> </p>\n");

        client.print("<h3>Flaps</h3>\n");
        client.print("<p>currently: " + String(flap) + " / 2</p>\n");
        client.print("<p> <a href='/flap0'><button>0</button></a>");
        client.print("<a href='/flap1'><button>1</button></a>");
        client.print("<a href='/flap2'><button>2</button></a> </p>\n");

        client.print("<h3>Pitch</h3>\n");
        String s = "neutral";
        if (pitch == -2) {s = "strong down";}
        if (pitch == -1) {s = "weak down";}
        if (pitch == 1) {s = "weak up";}
        if (pitch == 2) {s = "strong up";}
        client.print("<p>currently: " + s + "</p>\n");
        client.print("<p> <a href='/pitch-2'><button>strong down</button></a>");
        client.print("<a href='/pitch-1'><button>1</weak down></a>");
        client.print("<a href='/pitch0'><button>1</neutral></a>");
        client.print("<a href='/pitch+1'><button>1</weak up></a>");
        client.print("<a href='/pitch+2'><button>2</strong up></a> </p>\n");

        client.print("<h3>Roll</h3>\n");
        s = "neutral";
        if (roll == -2) {s = "strong left";}
        if (roll == -1) {s = "weak left";}
        if (roll == 1) {s = "weak right";}
        if (roll == 2) {s = "strong right";}
        client.print("<p>currently: " + s + "</p>\n");
        client.print("<p> <a href='/roll-2'><button>strong left</button></a>");
        client.print("<a href='/roll-1'><button>1</weak left></a>");
        client.print("<a href='/roll0'><button>1</neutral></a>");
        client.print("<a href='/roll+1'><button>1</weak right></a>");
        client.print("<a href='/roll+2'><button>2</strong right></a> </p>\n");

        client.print("<h3>Yaw</h3>\n");
        s = "neutral";
        if (yaw == -2) {s = "strong left";}
        if (yaw == -1) {s = "weak left";}
        if (yaw == 1) {s = "weak right";}
        if (yaw == 2) {s = "strong right";}
        client.print("<p>currently: " + s + "</p>\n");
        client.print("<p> <a href='/yaw-2'><button>strong left</button></a>");
        client.print("<a href='/yaw-1'><button>1</weak left></a>");
        client.print("<a href='/yaw0'><button>1</neutral></a>");
        client.print("<a href='/yaw+1'><button>1</weak right></a>");
        client.print("<a href='/yaw+2'><button>2</strong right></a> </p>\n");
      }



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

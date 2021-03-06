#include <Arduino.h>
#include <ArtnetnodeWifi.h>
#include <ESP8266WiFi.h>
#include <secrets.h>

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

// That seems to be the LED on my XLR Plug
const int ledPin = 0x0c;

//DMX Pin
const int dmxPin = 0x02;

// WiFiUDP UdpSend;
ArtnetnodeWifi artnetnode;

boolean ConnectWifiBeAp(void)
{
  boolean result = WiFi.softAP("ArtNet DMX WiFi Adapter", "kolping1963");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
}

boolean ConnectWifiToAp(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(IPAddress(WiFi.localIP()));
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  digitalWrite(ledPin, 1);

  boolean tail = false;

  Serial.print("DMX: Univ: ");
  Serial.print(universe, DEC);
  Serial.print(", Seq: ");
  Serial.print(sequence, DEC);
  Serial.print(", Data (");
  Serial.print(length, DEC);
  Serial.print("): ");
  Serial.println();

  if (length > 16) {
    length = 16;
    tail = true;
  }
  // send out the buffer
  for (int i = 0; i < length; i++)
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  // Send "break" as a "slow" zero.
  Serial1.begin(54700, SERIAL_5N2);
  Serial1.write(0);

  delayMicroseconds(100);

  Serial1.begin(250000, SERIAL_8N2);
  Serial1.write(0); // Start-Byte

  // send out the buffer
  for (int i = 0; i < 512; i++)
  {
    Serial1.write(data[i]);
  }

  digitalWrite(ledPin, 0);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Hello 123");

  Serial1.begin(250000, SERIAL_8N1);
  Serial.print("Serial DMX Starting...\n");

  // set LED port
  pinMode(ledPin, OUTPUT);
  // pinMode(dmxPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // ConnectWifiToAp();
  ConnectWifiBeAp();
  // max. 17 characters
  artnetnode.setShortName("DMX Art-Net-Node");
  // max. 63 characters
  artnetnode.setLongName("ESP8266 DMX Art-Net-Node");
  // set a starting universe if you wish, defaults to 0
  //artnetnode.setStartingUniverse(4);
  artnetnode.setNumPorts(4);
  artnetnode.enableDMXOutput(1);
  artnetnode.enableDMXOutput(2);
  artnetnode.enableDMXOutput(3);
  artnetnode.enableDMXOutput(4);
  artnetnode.begin();

  // this will be called for each packet received
  artnetnode.setArtDmxCallback(onDmxFrame);
  digitalWrite(ledPin, LOW);
}

void loop()
{
  // Serial.print("Hello 456");
  // Serial.printf("Hello again...\n");

  // we call the read function inside the loop
  artnetnode.read();

  digitalWrite(ledPin, LOW);
}

#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>


byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x6A, 0x6D
};
IPAddress ip(192, 168, 23, 177);

unsigned int localPort = 2355;      // local port to listen on
unsigned int heartBeatPin = 8;
unsigned int sirenenPin = 9;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

#define AN 1
#define AUS 0

void sirene(int what) {
  if (what == AN) {
    digitalWrite(sirenenPin, HIGH);  
  } else {
    digitalWrite(sirenenPin, LOW);  
  }
}

enum state_t {WAIT, SINGLE, ON};
unsigned long timer = 0;
const unsigned int hb_interval = 50;
unsigned long hb_timer = hb_interval;
state_t state = WAIT;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  pinMode(sirenenPin, OUTPUT);
  pinMode(heartBeatPin, OUTPUT);

  sirene(AUS);
  Serial.begin(9600);
}

void loop() {
  if (hb_timer-- == 0) {
      hb_timer = hb_interval;
      int value = digitalRead(heartBeatPin);
      digitalWrite(heartBeatPin, !value);
  }
  
  switch (state) {
    case(WAIT):
      break;
    case(SINGLE):
      timer=60;
      sirene(AN);
      state=ON;
      break;
    case(ON):
      timer--;
      if (timer == 0) {
        sirene(AUS);
        state = WAIT;
      }
      break;
    default:
      sirene(AUS);
      state = WAIT;
  }
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    if (packetBuffer[0] == 'a' &&
        packetBuffer[1] == 'l' &&
        packetBuffer[2] == 'a' &&
        packetBuffer[3] == 'r' &&
        packetBuffer[4] == 'm' &&
        packetBuffer[5] == '2' &&
        packetBuffer[6] == '3' &&
        packetBuffer[7] == 'c') {
      switch(packetBuffer[8]) {
        case('s'):
          state = SINGLE;
          break;
        case('l'):
          break;
        default:
          break;
      }
    }
  }
  delay(10);
}


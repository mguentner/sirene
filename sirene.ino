#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>


byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x6A, 0x6D
};
IPAddress ip(169,254, 38,200);

unsigned int localPort = 2355;      // local port to listen on
unsigned int sirenenPin = 9;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE] = "fnord";  //buffer to hold incoming packet,

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

enum state_t {WAIT, PACKET, ON};
unsigned long interval = 0;
unsigned long timer = 0;
unsigned long pause = 0;
const unsigned int hb_interval = 50;
unsigned long hb_timer = hb_interval;
state_t state = WAIT;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  pinMode(sirenenPin, OUTPUT);

  sirene(AUS);
  Serial.begin(9600);
}

void loop() {
  if (hb_timer-- == 0) {
      hb_timer = hb_interval;
  }
  
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  int finterval;
  int ftimer;
  int fpause;
  int wert = sscanf( packetBuffer+7," %u %u %u", &finterval, &ftimer, &fpause);
  
  switch (state) {
    case(WAIT):
      break;
    case(PACKET):
      switch(wert){
        case(-1):
          interval=1;
          timer=100;
          pause=137;
          sirene(AN);
          state=ON;
          break;
        case(1):
          interval=finterval;
          timer=100;
          pause=137;
          sirene(AN);
          state=ON;
          break;
        case(2):
          interval=finterval;
          timer=ftimer;
          pause=137;
          sirene(AN);
          state=ON;
          break;
        case(3):
          interval=finterval;
          timer=ftimer;
          pause=fpause;
          sirene(AN);
          state=ON;
          break;
        default:
          break;
      }
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
  delay(10);
}


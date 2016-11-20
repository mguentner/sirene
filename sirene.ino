#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>


byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x6A, 0x6D
};
IPAddress ip(169,254, 38,200);

unsigned int localPort = 2355;      // local port to listen on
unsigned int heartBeatPin = 8;
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

enum state_t {WAIT, STATEw0arg, STATEw1arg, STATEw2arg, STATEw3arg, ON};
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
  //pinMode(heartBeatPin, OUTPUT);

  sirene(AUS);
  Serial.begin(9600);
}


void loop() {
  if (hb_timer-- == 0) {
      hb_timer = hb_interval;
      //int value = digitalRead(heartBeatPin);
      //digitalWrite(heartBeatPin, !value);
  }

  switch (state) {
    case(WAIT):
      break;
    case(STATEw0arg):
      interval=1;
      timer=100;
      pause=137;
      sirene(AN);
      state=ON;
      break;
    case(STATEw1arg):
      interval="fd";
      timer=100;
      pause=137;
      sirene(AN);
      state=ON;
      break;
    case(STATEw2arg):
      interval="fd";
      time="fz";
      pause=137;
      sirene(AN);
      state=ON;
      break;
    case(STATEw3arg):
      interval="fd";
      time="fz";
      pause="fp";
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
  Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  char packetBuffer[] = "alarm23 23 420 137";
  int fd;
  int fz;
  int fp;
  int wert = sscanf( packetBuffer+7," %u %u %u", &fd, &fz, &fp);

  switch(wert){
    // Keine Parameter
    case(-1):
      //state = SINGLE
      printf( "Parameter: %d singlestate\n", wert);
      break;
    // 1 Parameter
    case(1):
      // state mit (fd)auer + (fz)eit(state = SINGLE) & (fp)ause(state = SINGLE)
      printf( "Parameter: %d i: %d fnordstate mit dauer(i)\n", wert, fd);
      break;
    case(2):
      // state mit (fd)auer & (fz)eit + (fp)ause(state = SINGLE)
      printf( "Parameter: %d i: %d l: %d fnordstate mit dauer(i) mit laenge(l)\n", wert, fd, fz);
      break;
    case(3):
      // state mit (fd)auer & (fz)eit & (fp)ause
      printf( "Parameter: %d i: %d l: %d p: %d fnordstate mit dauer(i) mit laenge(l) mit pause(p)\n", wert, fd, fz, fp);
      break;
  }
  
  delay(10);
}


#include <JeeLib.h>

/*
       RF12

    []      []
D4  []      []  D7
    []      []
    []      []
    []      []

    []      []
    []      []
    []      []
    []      []
    []      []

       FTDI
*/

#define LED_PIN 4

enum {
  MCP_IODIR, MCP_IPOL, MCP_GPINTEN, MCP_DEFVAL, MCP_INTCON, MCP_IOCON,
  MCP_GPPU, MCP_INTF, MCP_INTCAP, MCP_GPIO, MCP_OLAT
};

PortI2C myport (1);
DeviceI2C relay (myport, 0x26);

byte recvCount;
char password[16] = "123DOOP";
char msgChar;
bool allGood;

void unlock (int time) {
    Serial.println("Unlocking...");
    digitalWrite(LED_PIN, 1);
    exp_write(0x01);
    
    delay(time);
    
    exp_write(0x00);
    Serial.println("Locked!");
    digitalWrite(LED_PIN, 0);
}

void blink () {
    digitalWrite(LED_PIN, 1);
    delay(150);
    digitalWrite(LED_PIN, 0);
}

bool checkPassword () {
    allGood = true;
    for (byte i = 0; i < rf12_len; ++i) {
        msgChar = (int) rf12_data[i];
        if(msgChar != password[i]) {
            allGood = false;
        }
    }
    return allGood;
}

static void exp_write (byte value) {
  relay.send();
  relay.write(MCP_GPIO);
  relay.write(value);
  relay.stop();
}

static void exp_setup() {
  relay.send();
  relay.write(MCP_IODIR);
  relay.write(0);  // all outputs
  relay.stop();
}

void setup () {
    Serial.begin(57600);
    Serial.println("IAmALockBert");
    
    Serial.println("Setting up relay");
    exp_setup();
    exp_write(0x00);
    
    pinMode(LED_PIN, OUTPUT);
    Serial.println("Initialising rf");
    rf12_initialize(1, RF12_868MHZ);
    rf12_encrypt(RF12_EEPROM_EKEY);
    blink();
    Serial.println("Done setup");
}

void loop () {
    if (rf12_recvDone() && rf12_crc == 0) {
        if(checkPassword()) {
            // Unlock
            Serial.println("YEY!");
            unlock(5000);
        } else {
            // Alarms!
            Serial.println("NOT YEY!");
        }
    }
}


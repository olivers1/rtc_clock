/*
 Name:		rtc_clock.ino
 Created:	7/29/2020 11:45:21 AM
 Author:	olivers
*/
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include "InternalClock.h"
#include "arduino_secrets.h"

int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;      // your network SSID (name)
char pass[] = SECRET_PASS;      // your network password
unsigned int localPort = 2390;  // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28);   // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;     // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

// instatiate objects
InternalClock internalClock(InternalClock::Weekday::Saturday, 15, 20, 50);

void setup() 
{
    InitializeArduino();
    
}

void loop() 
{
    sendNTPpacket(timeServer); // send an NTP packet to a time server
      // wait to see if a reply is available
    delay(1000);
    if (Udp.parsePacket()) 
    {
        Serial.println("packet received");
        // We've received a packet, read the data from it
        Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, esxtract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        Serial.print("Seconds since Jan 1 1900 = ");
        Serial.println(secsSince1900);

        // now convert NTP time into everyday time:
        Serial.print("Unix time = ");
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;
        // subtract seventy years:
        unsigned long epoch = secsSince1900 - seventyYears;
        // print Unix time:
        Serial.println(epoch);

        unsigned int hour = epoch % 86400L / 3600;
        unsigned int minute = epoch % 3600 / 60;
        unsigned int second = epoch % 60;
        Serial.print("hour: ");
        Serial.println(hour);
        Serial.print("minute: ");
        Serial.println(minute);
        Serial.print("second: ");
        Serial.println(second);

        // print the hour, minute and second:
        Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
        Serial.print((epoch % 86400L) / 3600); // print the hour (86400 equals secs per day)
        Serial.print(':');
        Serial.print((epoch % 3600) / 60); // print the minute (3600 equals secs per minute)
        Serial.print(':');
        Serial.println(epoch % 60); // print the second
    }
    // wait ten seconds before asking for the time again
    delay(10000);
}

void printWifiData() 
{
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println(ip);

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print("MAC address: ");
    printMacAddress(mac);
}

void printCurrentNet() 
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print the MAC address of the router you're attached to:
    byte bssid[6];
    WiFi.BSSID(bssid);
    Serial.print("BSSID: ");
    printMacAddress(bssid);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);

    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
}

void printMacAddress(byte mac[]) 
{
    for (int i = 5; i >= 0; i--) {
        if (mac[i] < 16) {
            Serial.print("0");
        }
        Serial.print(mac[i], HEX);
        if (i > 0) {
            Serial.print(":");
        }
    }
    Serial.println();
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) 
{
    //Serial.println("1");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    //Serial.println("2");
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    //Serial.println("3");

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    //Serial.println("4");
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    //Serial.println("5");
    Udp.endPacket();
    //Serial.println("6");
}

void printWifiStatus() 
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

ISR(RTC_CNT_vect)
{
    internalClock.InternalClockwork();  // makes the clockwork tick every time the function is called
    Serial.print("clock: ");
    Serial.println(internalClock.GetTimeInt());
    
    RTC.INTFLAGS = 0x01;    // clear overflow flag by writing '1'
}

void InitializeArduino()
{
    // I/O-pins


    // serial monitor setup
    Serial.begin(9600);

    // connect to wifi
    while (!Serial)     // open serial communications and wait for port to open:
    {
        // wait for serial port to connect
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) 
    {
        Serial.println("Please upgrade the firmware");
    }

    while (status != WL_CONNECTED)  // attempt to connect to Wifi network:
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(5000);   // wait 5 seconds for connection
    }

    Serial.print("You're connected to the network");    // you're connected now, so print out the data
    printCurrentNet();
    printWifiData();
    printWifiStatus();
    Serial.println("\nStarting connection to server...");
    Udp.begin(localPort);

    // RTC timer interrupt setup
    cli();      // disable any external interrupts
    while (RTC.STATUS != 0)
    {
        // wait until CTRLBUSY flag is cleared
    }
    RTC.CLKSEL = 0x00;  // 32.768 clock selected
    RTC.PERL = 0x00;
    RTC.PERH = 0x80;    // overflow value set to 32'768 equals to a signal frequency of 1 Hz
    RTC.INTCTRL = 0x01;     // overflow interrupt enabled
    RTC.CTRLA = 0x01;   // activate RTC interrup. no prescaler set
    sei();      // enable any external interrupts again
}
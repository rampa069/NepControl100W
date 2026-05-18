//----------------------------------------------------------------------------------------------
//    global Config file
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
#define VERSION "Neptune PA Control replacement V2.1 by DL1BZ (remote cmd support by EA5IUE)"
#define DEBUG_STATE
#define DEBUG_ADC
// #define STATICIP               // if using Static IP, not DHCP


//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// ESP32 Dev Module GPIO PIN definition (remapped from ESP32-S3)
// GPIO 1,3 = UART0 | GPIO 6-11 = SPI flash (NO usar en ESP32 clasico)
const int ptt2PIN = 32;    // PTT2 input
const int bvHLPIN = 36;    // bandvoltage input from HL2 (input-only pin)
const int pttPIN = 33;     // PTT input line at RCA socket
const int pwrPIN = 5;      // Power ON LED
const int txPIN = 4;       // RX/TX relais + TX LED
const int biasPIN = 14;    // BC547C base for bias on/off
const int PIN160 = 25;     // LPF 160m relais
const int PIN80 = 26;      // LPF 80m relais
const int PIN6040 = 27;    // LPF 60m+40m relais
const int PIN3020 = 18;    // LPF 30m+20m relais
const int PIN1715 = 19;    // LPF 17m+15m relais
const int PIN1210 = 23;    // LPF 12m+10m relais
const int PIN6 = 22;       // LPF 6m relais
const int ledPIN = 21;     // onboard LED
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// global vars (start with G_)
boolean G_LEDState;
const int G_PTTDelay = 20; // in ms, delay for BIAS on
int G_bandcode;

volatile int G_bandMode = 0;       // 0=AUTO (HL2 bandvoltage), 1=MANUAL (remote band from Zeus)
volatile int G_remoteBand = 0;     // remote band in metres (only used when G_bandMode==1)
volatile bool G_remotePTT = false; // remote PTT override (ORed with hardware PTT)
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
// Timer variables
unsigned long lastTime = 0;
unsigned long pLastTime = 0;
unsigned long timerDelay = 500; // in ms, 1000 = 1s
//----------------------------------------------------------------------------------------------

#ifdef DASHBOARD
//----------------------------------------------------------------------------------------------
// WiFi Setup
//----------------------------------------------------------------------------------------------
// Replace with your network credentials
// const char* ssid = "YOUR_SSID";
// const char* password = "YOUR_WIFI_PASSWORD";

const char* ssid = "jumanji_land";
const char* password = "pandicornio";

const char* hostname = "NEPTUNE-PA";             // define hostname for network
const char* ntpServer = "192.53.103.108";        // define NTP server you will use
//----------------------------------------------------------------------------------------------
#endif
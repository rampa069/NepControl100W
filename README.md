# NepControl for the RF Amplifier Neptune 100W (automatic band switch)

add an WAVESHARE ESP32-S3 Zero to control the amp better and fix the issue "PA switched OFF because faulty frequency detection" in mode SSB.

The issue isn't a really issue (PA switched immediately back to OFF/bypass if using mode SSB) - is a decision of the manufacturers device firmware design to protect the LPF against wrong band selection.
For me this firmware design in it's form is not acceptable, because it continually breaks the normal use of this PA in mode SSB and in my case if running FT8 too.

My solution to bring back the PA to a usable condition: I take the control over the main control signals like BIAS on/off and RX/TX relais with my own ESP32 controller and changed PA switch logic, which fix this "issue" completly. The "issue" is gone after my mod.<br>
Additional I replace with this modification the automatic band detection through band select with bandvoltage output from the Hermes-Lite 2.<br>
That means, you LOST the original automatic band selection through the frequency detection. But with bandvoltage as option an automatic band select is back again.

__All code refers to my schematic. You need to look in for understand all.__

## Webdashboard as option

I implement Wifi and a webserver with websockets for show the status of the amp in a webbrowser as an option.<br>
Default this option is deactivated. If you want to use it, you need change in NepControl100W.ino this:
<code>
// #define DASHBOARD
</code>
with
<code>
#define DASHBOARD
</code>
__Don't forget to define your WiFi credentials in the <config.h>__

## Remote control via WebSocket

When the DASHBOARD is enabled, the WebSocket server at `ws://<ip>/ws` accepts
JSON commands for remote control from the Zeus plugin or any other WebSocket
client. The legacy plaintext `"getReadings"` command from the built-in
dashboard is preserved for backward compatibility.

### Commands

All commands are JSON objects with a `"cmd"` field:

| Command | JSON | Description |
|---|---|---|
| Set band manually | `{"cmd":"setBand","band":20}` | Switch to MANUAL mode, select LPF for the given band (160/80/60/40/30/20/17/15/12/10/6) |
| Auto band from HL2 | `{"cmd":"setAutoBand"}` | Switch back to AUTO mode, LPF follows HL2 bandvoltage |
| Remote PTT override | `{"cmd":"setPTT","state":true}` | Force PTT on/off remotely (ORed with hardware PTT) |
| Request telemetry | `{"cmd":"getStatus"}` | Request an immediate telemetry push |

### Telemetry format

The ESP32 pushes telemetry JSON every ~500 ms and on `getStatus` request:

```json
{
  "ptt": true,
  "bias": true,
  "rxtxrelais": true,
  "bv": "1842",
  "band": "20",
  "bandMode": "AUTO",
  "ssid": "MyNetwork",
  "rssi": "-55",
  "rst": "S7",
  "time": "18.05.2026 14:30:00"
}
```

The `bandMode` field is `"AUTO"` (HL2 bandvoltage drives LPF selection) or
`"MANUAL"` (remote `setBand` command drives LPF selection).

### Command response format

Each command returns a JSON acknowledgement broadcast to all WebSocket clients:

```json
{"cmd":"setBand","ok":true,"msg":"band set to 20m, MANUAL mode"}
```

## Requirements

- the code was developed with the Arduino IDE 2.x and the additional ESP32 board extensions, you need the .ino file and all .h files in the same folder
- the config.h is the central project configuration file, look in for changes if needed 
- all is tested only with a WAVESHARE ESP32-S3 Zero
- for programming this ESP32-S3 Zero look in the Wiki at WAVESHAREs website, there are some special features to consider
- you know how to work with the Arduino IDE &  install additional libraries
- you can read and understand schematics

<hr>

## Agreement

If you want to use my mod, you confirm the following:<BR>
No guarantee, no warranty !<BR>
You do all at your own risk !<BR>
You know, what you do and why !<BR>
Only for use in amateur radio, because you need to be a licensed hamradio operator !<br>
This code is free for personal use, but commercial use is not allowed !

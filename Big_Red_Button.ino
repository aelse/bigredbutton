#include <Bounce2.h>
#include <TaskScheduler.h>
#include <SoftwareSerial.h>

#define LED_PIN D6
#define BUTTON_PIN D7
#define HC_05_TX D4
#define HC_05_RX D3
#define HC_05_ST D2

Bounce debouncer = Bounce();
Scheduler runner;
SoftwareSerial BTSerial(HC_05_TX, HC_05_RX); // RX | TX : reversed relative to HC-05 module

void setLedState();
void checkConnectedState();

Task tSetLedState(150, TASK_FOREVER, &setLedState);
Task tCheckConnectedState(500, TASK_FOREVER, &checkConnectedState);

bool btConnected = false;
int ledState = LOW;
// True when switch is pressed
bool switchState;

void setup() {
  pinMode(HC_05_ST, INPUT);
  pinMode(LED_PIN, OUTPUT);

  debouncer.attach(BUTTON_PIN, INPUT_PULLUP);
  debouncer.interval(25);
  switchState = debouncer.read() == LOW;  // closed == LOW

  runner.init();
  runner.addTask(tSetLedState);
  tSetLedState.enable();
  runner.addTask(tCheckConnectedState);
  tCheckConnectedState.enable();

  Serial.begin(38400);
  BTSerial.begin(38400);  // HC-05 factory default (some modules come preconfigured 9600 bps)
  delay(100);
  Serial.println("Ready");
}

void loop() {
  runner.execute();
  updateButtons();
}

void updateButtons() {
  debouncer.update();
  if (debouncer.rose() || debouncer.fell()) {
    // Invert switch state on rise or fall.
    switchState = !switchState;
    if (btConnected)
      BTSerial.write(switchState ? "pressed\r\n" : "reset\r\n");
    Serial.println(switchState ? "Button was pressed" : "Button was reset");
  }
}

void checkConnectedState() {
  bool lastState = btConnected;
  btConnected = digitalRead(HC_05_ST) == HIGH;
  if (lastState != btConnected) {
    Serial.println(btConnected ? "Bluetooth connected" : "Bluetooth disconnected");
    if (btConnected) {
      BTSerial.write("Initial switch state: ");
      BTSerial.write(switchState ? "pressed\r\n" : "reset\r\n");
    }
  }
}

void setLedState() {
  if (btConnected) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Flash LED when not connected
    ledState = ledState ? LOW : HIGH;
    digitalWrite(LED_PIN, ledState);
  }
}
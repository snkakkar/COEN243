/*
 * COEN 243
 * Matthew Findlay, Kevin Velchich, Esai Morales, Schachi Kakkar
 * 
 * Client code for for our school project Agrotile
 * 
 *This is put on the clients, it sends soil readings to the server and goes to sleep when the server tell its to
 */
#include "namedMesh.h"

#define   MESH_SSID          "Agrotile"
#define   MESH_PASSWORD      "Agrotile"
#define   MESH_PORT           5555
#define   uS_TO_S_FACTOR      1000000
#define   READ_INTERVAL       30 //Read from sensor every 30 seconds while awake
#define   SPINS_BEFORE_SLEEP  1000
#define   SENSOR_PIN          32

Scheduler          userScheduler;
namedMesh          mesh;
String nodeName = "node4"; // Node name, needs to be unique

/*
 * Function Definitions
 */
String getValue(String data, char separator, int index)
/*
 * Used for splitting strings. We use this to extract
 * sleep time from the root server.
 */
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void deep_sleep(int t) {
  /*
   * Put the esp32 to sleep when notified by the server
   * Takes in a time in seconds
   */
  int i = 0;
  
  //Let mesh broadcast sleep message to all nodes before going to sleep
  while(i < SPINS_BEFORE_SLEEP){
    userScheduler.execute();
    mesh.update();
    ++i;
  }
  Serial.printf("Going to sleep");
  esp_sleep_enable_timer_wakeup(t * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

Task taskSendMessage( TASK_SECOND*READ_INTERVAL, TASK_FOREVER, []() {
  /*
   * Read from the soil sensor and send the value to the server.
   */

    String msg = String(":") + String(analogRead(SENSOR_PIN));
    String to = "server";
    Serial.printf("sending to server");
    mesh.sendSingle(to, msg); 
});

void setup() {
  Serial.begin(115200);
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);\
  
  //Setup mesh using painlessMesh APIs
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.setName(nodeName); // This needs to be an unique name! 

  //Set Message callbacks
  mesh.onReceive([](uint32_t from, String &msg) {
    /*
     * Recieve callback
     * If device recieves a sleep message from the server,
     * extract sleep time and go into deep sleep power saving mode
     */

    String m = getValue(msg, ':', 0);
    String sleep_time = getValue(msg, ':', 1);
    if(m == "sleep") {
      Serial.printf("HIT SLEEP\nReceived message by id from: %u, %s\n", from, msg.c_str());
      Serial.printf("sleeping for time: ");
      Serial.printf(sleep_time.c_str());
      deep_sleep(sleep_time.toInt());
    }
    Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str());
  });

  mesh.onChangedConnections([]() {
    /*
     * Upon mesh connection update
     */
    Serial.printf("Changed connection\n");
  });
  /*
   * Add tasks to scheduler and enable
   */
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

  pinMode(SENSOR_PIN, INPUT)
}

void loop() {
  //Execute scheduler and update the mesh
  userScheduler.execute();
  mesh.update();
}

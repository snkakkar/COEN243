/*
*  Authors:        Matthew Findlay, Kevin Velcich,
*                  Esai Morales, Schachi Kakkar
*
*  Description:    Program for a sensor node for reading moisture sensors and
*                  sending the information to the connected server node.
*
*                  When starting, the node wakes up and will begin joining/
*                  creating the mesh. Upon joining, it will read the soil
*                  sensors and send the readings to the server node. When it
*                  recieves a broadcast message to sleep, it will sleep for a
*                  set amount of time, wake up and repeat.
*/

# include "namedMesh.h"
# include "MeshUtils.h"

# define MOISTURE_PIN 34

static namedMesh _mesh;
static Scheduler _nodeScheduler;
static String _nodeName = "Node4"; // Node name, needs to be unique with no spaces

// Task to send a moisture reading to the the server node, repeats every so often until disabled
Task taskSendMessage(TASK_SECOND * READ_INTERVAL, TASK_FOREVER, []() {
    String msg = String(":") + String(analogRead(MOISTURE_PIN));
    Serial.println("Sending message '" + msg + "' to '" + SERVER_NAME + "'");
    _mesh.sendSingle(SERVER_NAME, msg);
});

// Task to put the device to sleep. First iteration it will wait for message to propogate
// throughout the mesh, second time it will enter deep sleep.
static bool wait = true;
Task taskDeepSleep(TASK_SECOND * WAIT_BEFORE_SLEEP, TASK_FOREVER, []() {
    if (!wait) {
        Serial.println("Entering deep sleep");
        esp_sleep_enable_timer_wakeup(SLEEP_TIME);
        esp_deep_sleep_start();
    }
    Serial.println("Waiting a cycle to sleep");
    wait = false;
});

void setup() {
    Serial.begin(115200);

    // Setup node/mesh
    _mesh.init(MESH_SSID, MESH_PASS, &_nodeScheduler, MESH_PORT);
    _mesh.setName(_nodeName); // This needs to be an unique name!

    //Set message callbacks
    _mesh.onReceive([](String &source, String &msg) {
        Serial.println("Message received from '" + source + "', containing '" + msg + "'");
        if (msg == ACK_MESSAGE)
            taskSendMessage.disable();
        else if (msg == SLEEP_MESSAGE) {
            _nodeScheduler.addTask(taskDeepSleep);
            taskDeepSleep.enable();
        }
    });
    _mesh.onChangedConnections([]() { Serial.println("Network Topology has changed"); });

    _nodeScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();
}

void loop() {
    _nodeScheduler.execute();
    _mesh.update();
}

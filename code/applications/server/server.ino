 /*
 *  Authors:        Matthew Findlay, Kevin Velcich,
 *                  Esai Morales, Schachi Kakkar
 *
 *  Description:    Program for a server node to receive values read from
 *                  sensor nodes and host a AsyncWebServer page for a client to
 *                  view said readings in a graph.
 *
 *                  Server is constantly awake. When a message is received, it
 *                  will keep store the information for that node. After the
 *                  mesh has been fully built and the server recieves readings
 *                  from all nodes, it sends a broadcast to put all nodes to
 *                  sleep.
 *
 *                  NOTE: This program is very large, but can definitely fit on
 *                  an ESP32. However, the default partitioning doesn't support
 *                  a program of this size. To fix this I created a new
 *                  partitioning file and board configuration for the ESP32.
 *                  For more information check the post below:
 *                  https://desire.giesecke.tk/index.php/2018/04/20/change-partition-size-arduino-ide/
 *
 *                  Additionally, we needed to modify the implementation of ESP
 *                  Async Web Server library. The issue was that for template
 *                  variables, by default they are signified by the percent
 *                  character ('%'). However, this caused a lot of conflicts with
 *                  JavaScript code where between % symbols were picked up as
 *                  templates. Commit #366 on the GitHub repository seemed to
 *                  provide a small workaround, but didn't seem to work for us.
 *                  Instead we just modified the template placeholder to be
 *                  the tilde character ('~') as it doesn't appear in our JS
 *                  code. This change occurs on line 63 of src/WebResponseImpl.h
 *                  in the ESPAsyncWebServer library.
 */

# include <AgroTile.h>
# include <AsyncTCP.h>
# include <ESPAsyncWebServer.h>
# include <IPAddress.h>
# include "MeshUtils.h"

void receivedCallback(const uint32_t &from, const String &msg);

static namedMesh _mesh;
static AsyncWebServer _server(80);

static SimpleList<uint32_t> _nodes;
static unsigned long _timer = 0;
static unsigned char _receiveCount = 0;

// Converting a moisture resistance value to a percentage
// Probably should be abstracted out into a AgroTile.cpp
static unsigned char convertToPercent(unsigned short value) {
    float percent = -20.0/419.0 * (float) value * 1.0 + 81900.0/419.0;
    if (percent > 100) return 100;
    return percent;
}

// Handler for when server recieves a message.
void receiveCallback(String &source, String &message) {
    Serial.printf("Message received from '%s', containing '%s'\n", source.c_str(), message.c_str());
    String value = message.substring(1);
    // Only read the moisture value if it is a a number
    if (!isValidNumber(value)) {
        Serial.println("NAN, throwing out message");
        return;
    }

    // Send ack to sender so they stop updating
    sendAck(_mesh, source);
    int moisture = convertToPercent(value.toInt());
    recordMoisture(source, new Moisture(millis(), moisture));

    // Start timing the messages, if we received the first one
    if (_timer == 0) _timer = millis();
    ++ _receiveCount;
}

// When we get a async server request, respond with the html page
void AsyncRequestCallback(AsyncWebServerRequest *request) {
    Serial.println("Sending...");
    request->send_P(200, "text/html", index_html, processor);
    Serial.println("Sent...");
}

// This will tell all devices in the mesh to sleep if we have either received enough
// messages or waited enough time. 
static void sleepIfComplete() {
    if (_timer == 0) return;

    // Put all nodes to sleep if all nodes are accounted for or waited enough time
    unsigned long elapsed = millis() - _timer;
    if ((elapsed >= SERVER_MIN_TIME && _receiveCount == _nodes.size())
            || elapsed >= SERVER_MAX_TIME) {
        // Broadcast sleep and reset timer and counter
        Serial.println("Putting nodes to sleep.");
        broadcastSleep(_mesh);
        _timer = _receiveCount = 0;
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize server & mesh
    allocate();
    //_mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
    _mesh.init(MESH_SSID, MESH_PASS, MESH_PORT, WIFI_AP_STA, 6);
    _mesh.setName(SERVER_NAME);

    // Set call back methods for the mesh to be called upon specific events
    _mesh.onReceive(&receiveCallback);
    _mesh.onChangedConnections([](void) {
        _nodes = _mesh.getNodeList();
        Serial.printf("Network topology changed, there are now %d nodes connected\n", _nodes.size());
        _server.on("/", HTTP_GET, &AsyncRequestCallback);
        _server.begin();
    });

    // When server is requested, send HTML
    _server.on("/", HTTP_GET, &AsyncRequestCallback);
    _server.begin();

    // Getting Access Point IP, and printing it
    IPAddress apIP = IPAddress(_mesh.getAPIP());
    Serial.printf("AP IP is '%s'\n", apIP.toString().c_str());
}

void loop() {
    _mesh.update();
    sleepIfComplete();
}

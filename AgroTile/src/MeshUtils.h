# ifndef MESHUTILS_H
# define MESHUTILS_H

# include <Arduino.h>
# include "namedMesh.h"

# define MESH_SSID          "AgroTile"
# define MESH_PASS          "123456789"
# define MESH_PORT          5555

# define SERVER_MIN_TIME    10 * 1000       // Minimum number of milliseconds server must receive messages to put all nodes to sleep
# define SERVER_MAX_TIME    30 * 10000      // Number of milliseconds until the server puts all nodes to sleep
# define WAIT_BEFORE_SLEEP  10
# define SLEEP_TIME         20 * 1000000    // Amount of time for nodes to sleep in microseconds
# define READ_INTERVAL      5              // Interval for nodes to read from sensor while awake
# define RECORD_DELTA       0 //SLEEP_TIME / 1000 / 2 // The amount of time between sensor readings for us to record

// defined in MeshUtils.cpp
extern String ACK_MESSAGE;
extern String SERVER_NAME;
extern String SLEEP_MESSAGE;

void broadcastSleep(namedMesh &mesh);
void sendAck(namedMesh &mesh, String &dest);
void sendMessage(namedMesh &mesh, String &dest, String &message);

# endif /* MESHUTILS_H */

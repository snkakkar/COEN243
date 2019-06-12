# include "MeshUtils.h"

String ACK_MESSAGE = "ACK_MSG";
String SERVER_NAME = "ServerNode";
String SLEEP_MESSAGE = "SLEEP_MSG";

// These functions aren't too useful, but are abstracted in case functionality is extended
void broadcastSleep(namedMesh &mesh) {
    mesh.sendBroadcast(SLEEP_MESSAGE);
}

void sendAck(namedMesh &mesh, String &dest) {
    mesh.sendSingle(dest, ACK_MESSAGE);
}

void sendMessage(namedMesh &mesh, String &dest, String &message) {
    mesh.sendSingle(dest, message);
}
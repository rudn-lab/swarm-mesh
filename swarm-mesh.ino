#include "painlessMesh.h"

#define   MESH_PREFIX     "esp-mesh-network"
#define   MESH_PASSWORD   "0123456789abcdef"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

enum MessageType {
  Broadcast = 0,
  Single,
  None,
};

String messageTypeNames[3] = {
  "broadcast",
  "single",
  "none"
};

struct Message {
  MessageType type;
  int destId;
  String content;
};

Message msg;

String input = "";
bool isInputFinished = false;

void serialEvent() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      isInputFinished = true;
    } else {
      input += c;
    }
  }
}

void execCommand() {
  if (!isInputFinished) {
    return;
  }

  int cmd_delimiter_idx = input.indexOf(":");
  String cmd = input.substring(0, cmd_delimiter_idx);
  String val = input.substring(cmd_delimiter_idx + 1);
  Serial.printf("> %s:%s\n", cmd, val);

  if (cmd == "msg") {
    if (val == "new") {
      msg.type = MessageType::None;
      msg.destId = 0;
      msg.content = "";
    } else if (val == "show") {
      Serial.printf("Message:\n");
      Serial.printf("\ttype:%s\n", messageTypeNames[msg.type]);
      Serial.printf("\tdestId:%i\n", msg.destId);
      Serial.printf("\tcontent:%s\n", msg.content);
    } else if (val == "send") {
      Serial.printf("Sending the message");
      if (msg.type == MessageType::Broadcast) {
        mesh.sendBroadcast(msg.content);
      } else if (msg.type == MessageType::Single) {
        mesh.sendSingle(msg.destId, msg.content);
      }
    } else if (val == "time") {
      mesh.sendBroadcast(String(mesh.getNodeTime()));
    }
  } else if (cmd == "type") {
    if (val == "broadcast") {
      msg.type = MessageType::Broadcast;
    } else if (val == "single") {
      msg.type = MessageType::Single;
    }
  } else if (cmd == "destid") {
    msg.destId = val.toInt();
  } else if (cmd == "content") {
    msg.content = val;
  }

  // Reset stuff
  isInputFinished = false;
  input = "";
}

Task taskExecCommand(TASK_IMMEDIATE, TASK_FOREVER, &execCommand);

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  userScheduler.addTask(taskExecCommand);
  taskExecCommand.enable();

  Serial.printf("My nodeId: %i.\n", mesh.getNodeId());
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}

// Enable debug prints
//#define MY_DEBUG
// Enable passive mode
#define MY_PASSIVE_NODE
// Passive mode requires static node ID
#define MY_NODE_ID 100
// Enable and select radio type attached
#define MY_RADIO_NRF5_ESB

#include <MySensors.h>
#include "core/MyTransport.h"

#define CHILD_ID 0  
//#define MY_BAUD_RATE 115200

// Initialize general message
MyMessage msg(CHILD_ID, V_TEMP);

extern uint32_t sleepingPeriod;
extern String inMsg;
extern void sendMsg(int &attempts, int nodeId, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep);
extern void receive(const MyMessage &message);
extern String parseToString(const MyMessage &message);
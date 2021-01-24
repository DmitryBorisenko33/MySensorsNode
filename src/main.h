//#define MY_DEBUG
//#define MY_BAUD_RATE 115200
#define MY_PASSIVE_NODE
#define MY_NODE_ID 100
#define MY_RADIO_NRF5_ESB

#include <MySensors.h>
#include <pinMap.h>

extern uint32_t sleepingPeriod;
extern uint16_t attamptsNumber;
extern String inMsg;

extern void sendMsgEchoAck(int &attempts, int nodeId, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep);
extern void sendMsgFastAck(int &attempts, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep);
extern void receive(const MyMessage &message);
extern String parseToString(const MyMessage &message);
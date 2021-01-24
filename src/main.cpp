#include "main.h"

String inMsg;
uint32_t sleepingPeriod = 10000;

void setup() {
}

void presentation() {
    sendSketchInfo("Passive node", "1.0");
    present(CHILD_ID, S_TEMP);
}

void loop() {
    int nodeId;
    int ChildId;
    float value;
    String outMsg;
    static int attempts = 0;

    //отправка первого сообщения============================================================================
    nodeId = 0;
    ChildId = 0;
    value = random(100, 150);
    send(msg.setDestination(nodeId).setSensor(ChildId).set(value, 2), true);
    outMsg = String(nodeId) + "," +
             String(ChildId) + "," +
             String(value) + ";";
    Serial.println("sended: " + outMsg);
    wait(1500, C_SET, S_CUSTOM);
    if (inMsg == outMsg) {
        Serial.println("Msg 1 delivered");
        attempts = 0;
        //sleep(sleepingPeriod);
        inMsg = "";
    } else {
        attempts++;
        Serial.println("Msg 1 not delivered, attempt: " + String(attempts));
        if (attempts >= 5) {
            attempts = 0;
            Serial.println("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + " sec");
            sleep(sleepingPeriod);
        }
    }
    Serial.println("==============================================");

    //отправка второго сообщения============================================================================
    nodeId = 0;
    ChildId = 1;
    value = random(1000, 1500);
    send(msg.setDestination(nodeId).setSensor(ChildId).set(value, 2), true);
    outMsg = String(nodeId) + "," +
             String(ChildId) + "," +
             String(value) + ";";
    Serial.println("sended: " + outMsg);
    wait(1500, C_SET, S_CUSTOM);
    if (inMsg == outMsg) {
        Serial.println("Msg 2 delivered");
        attempts = 0;
        sleep(sleepingPeriod);
        inMsg = "";
    } else {
        attempts++;
        Serial.println("Msg 2 not delivered, attempt: " + String(attempts));
        if (attempts >= 5) {
            attempts = 0;
            Serial.println("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + "sec");
            sleep(sleepingPeriod);
        }
    }
    Serial.println("==============================================");
}

void receive(const MyMessage &message) {
    if (message.isEcho()) {
        inMsg = String(message.getSender()) + "," +
                String(message.getSensor()) + "," +
                parseToString(message) + ";";

        Serial.println("received: " + inMsg);
    }
}

String parseToString(const MyMessage &message) {
    String value = "error";
    switch (message.getPayloadType()) {
        case 0:  //Payload type is string
            value = message.getString();
            return value;
        case 1:  //Payload type is byte
            value = String(message.getByte());
            return value;
        case 2:  //Payload type is INT16
            value = String(message.getInt());
            return value;
        case 3:  //Payload type is UINT16
            value = String(message.getUInt());
            return value;
        case 4:  //Payload type is INT32
            value = String(message.getInt());
            return value;
        case 5:  //Payload type is UINT32
            value = String(message.getUInt());
            return value;
        case 6:  //Payload type is binary
            value = String(message.getBool());
            return value;
        case 7:  //Payload type is float32
            value = String(message.getFloat());
            return value;
        default:
            return value;
    }
}

/*
    Метод проверки доставки сообщения (пока не работает из за того что функция всегда возвращает false)
    for (int i = 0; i < 5; i++) {
        if (send(msgSendStatus1.setDestination(nodeId).setSensor(ChildId).set(value, 2), false)) {  //если отправилось
            wait(10);
            return;
        } else {
            Serial.println("Msg not delivered, attempt " + String(i));
            wait(100);
        }
    }
    */

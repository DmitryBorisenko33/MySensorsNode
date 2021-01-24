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
    static int attempts = 0;
    sendMsg(attempts, 0, 0, V_TEMP, random(1000, 1500), false);
    Serial.println("==============================================");
    sendMsg(attempts, 0, 1, V_TEMP, random(100, 150), true);
    Serial.println("==============================================");
}

void sendMsg(int &attempts, int nodeId, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep) {
    MyMessage msg(ChildId, dataType);
    send(msg.setDestination(nodeId).setSensor(ChildId).set(value, 2), true);  //отправляем сообщение
    String outMsg = String(nodeId) + "," +                                    //формируем его сигнатуру в виде 0,0,12.5;
                    String(ChildId) + "," +
                    String(value) + ";";
    Serial.println("sended: " + outMsg);
    long prevMillis = millis();
    wait(1500, C_SET, dataType);  //ждем пока получим echo в функции receive
    long ackTime = millis() - prevMillis;
    if (inMsg == outMsg) {  //если сигнатура полученного эха совпала с отправленным сообщением - сообщение было доставлено
        Serial.println("Msg " + String(ChildId) + " delivered, ack time = " + String(ackTime) + " ms");
        attempts = 0;
        if(goToSleep) sleep(sleepingPeriod);
        inMsg = "";
    } else {  //если не совпала значит в эхо ничего не пришло
        attempts++;
        _transportSM.failedUplinkTransmissions = 0;  //сбросим счетчик в ноль что бы нода не ушла в поиск сети
        Serial.println("Msg " + String(ChildId) + " not delivered, attempt: " + String(attempts));
        if (attempts >= 5) {
            attempts = 0;
            Serial.println("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + " sec");
            sleep(sleepingPeriod);
        }
    }
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

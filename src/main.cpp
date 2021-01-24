#include "main.h"

String inMsg = "";
uint32_t sleepingPeriod = 10000;  //время сна в милисекундах
uint16_t attamptsNumber = 5;      //количество попыток повторных пересылок сообщений

void setup() {
    Serial.println("====================Started=====================");
}

void presentation() {
    sendSketchInfo("IoT Manager Node", "1.0.0");
    present(0, S_TEMP);
    present(1, S_TEMP);
}

//void before() {
//    //NRF_POWER->DCDCEN = 1;
//    //sleep(400);
//}

void loop() {
    static int attempts = 0;

    sendMsgEchoAck(attempts, 0, 0, V_TEMP, random(1000, 1500), false);
    Serial.println("==============================================");
    sendMsgEchoAck(attempts, 0, 1, V_TEMP, random(100, 150), true);  // у последнего сообщения в loop должно стоять true, у остальных false
    Serial.println("==============================================");

    //sendMsgFastAck(attempts, 2, V_TEMP, random(10, 15), false);
    //Serial.println("==============================================");
    //sendMsgFastAck(attempts, 3, V_TEMP, random(100, 150), true);
    //Serial.println("==============================================");
}

void sendMsgEchoAck(int &attempts, int nodeId, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep) {
    MyMessage msg(ChildId, dataType);
    send(msg.setDestination(nodeId).setSensor(ChildId).set(value, 2), true);  //отправляем сообщение
    String outMsg = String(nodeId) + "," +                                    //формируем его уникальную сигнатуру в виде 0,0,12.5;
                    String(ChildId) + "," +
                    String(value) + ";";
    Serial.println("sended: " + outMsg);
    long prevMillis = millis();
    wait(1500, C_SET, dataType);  //ждем пока получим echo в функции receive
    long ackTime = millis() - prevMillis;
    if (inMsg == outMsg) {  //если сигнатура полученного эхо совпала с отправленным сообщением - сообщение было доставлено
        Serial.println("Msg " + String(ChildId) + " delivered, ack time = " + String(ackTime) + " ms");
        attempts = 0;
        if (goToSleep) sleep(sleepingPeriod);
        inMsg = "";
    } else {  //если не совпала значит в эхо ничего не пришло
        attempts++;
        _transportSM.failedUplinkTransmissions = 0;  //сбросим счетчик в ноль что бы нода не ушла в поиск сети
        Serial.println("Msg " + String(ChildId) + " not delivered, attempt: " + String(attempts));
        if (attempts >= attamptsNumber) {
            attempts = 0;
            Serial.println("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + " sec");
            sleep(sleepingPeriod);
        }
    }
}

void sendMsgFastAck(int &attempts, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep) {
    MyMessage msg(ChildId, dataType);
    if (send(msg.set(value, 2))) {  //если отправилось
        attempts = 0;
        Serial.println("Msg " + String(ChildId) + " delivered, value = " + String(value));
        if (goToSleep) sleep(sleepingPeriod);
    } else {
        attempts++;
        Serial.println("Msg " + String(ChildId) + " not delivered, attempt: " + String(attempts));
        wait(1500);
        if (attempts >= attamptsNumber) {
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

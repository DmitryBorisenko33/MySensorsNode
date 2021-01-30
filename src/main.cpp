#include "main.h"

String inMsg = "";
uint32_t sleepingPeriod = 30 * 60 * 1000;  //первое число - минуты
uint16_t attamptsNumber = 3;               //количество попыток повторных пересылок сообщений
long totalErrors = 0;
long fatalErrors = 0;



void preHwInit() {
    // pinMode(25, OUTPUT);
    // digitalWrite(25, HIGH);
    // pinMode(27, OUTPUT);
    // digitalWrite(27, HIGH);
}

void before() {
    //NRF_POWER->DCDCEN = 1; //включение режима оптимизации питания, расход снижается на 40%, но должны быть установленны емкости
    //NRF_NFCT->TASKS_DISABLE = 1; //останавливает таски, если они есть
    //NRF_UICR->NFCPINS = 0;  //отключает nfc и nfc пины становятся доступными для использования
    //NRF_NVMC->CONFIG = 0;   //
    NRF_UART0->ENABLE = 0;  //отклчает uart, прежде чем использовать в коде не должно быть serial.print
}

void setup() {
    SerialPrintln("====================Started=====================");
}

void presentation() {
    sendSketchInfo("IoT Manager Node", "1.0.0");
    present(0, S_TEMP);
    present(1, S_TEMP);
}

void loop() {
    static int attempts = 0;
    float batteryVoltage = (float)hwCPUVoltage() / 1000.00;

    sendMsgEchoAck(attempts, 0, 0, V_VOLTAGE, batteryVoltage, false);
    wait(5);
    SerialPrintln("==============================================");
    sendMsgEchoAck(attempts, 0, 1, V_TEMP, random(1000, 1500), false);
    wait(5);
    SerialPrintln("==============================================");
    sendMsgEchoAck(attempts, 0, 2, V_TEMP, random(100, 150), false);
    wait(5);
    SerialPrintln("==============================================");
    sendMsgEchoAck(attempts, 0, 3, V_CUSTOM, fatalErrors, false);  
    wait(5);
    SerialPrintln("==============================================");
    sendMsgEchoAck(attempts, 0, 4, V_CUSTOM, totalErrors, true);  // у последнего сообщения в loop должно стоять true, у остальных false
    SerialPrintln("==============================================");

    //sendMsgFastAck(attempts, 0, V_VOLTAGE, batteryVoltage, false);
    //wait(5);
    //SerialPrintln("===========================================");
    //sendMsgFastAck(attempts, 1, V_TEMP, random(1000, 1500), false);
    //wait(5);
    //SerialPrintln("===========================================");
    //sendMsgFastAck(attempts, 2, V_TEMP, random(100, 150), false);
    //wait(5);
    //SerialPrintln("===========================================");
    //sendMsgFastAck(attempts, 3, V_CUSTOM, fatalErrors, false);  
    //wait(5);
    //SerialPrintln("===========================================");
    //sendMsgFastAck(attempts, 4, V_CUSTOM, totalErrors, true);  // у последнего сообщения в loop должно стоять true, у остальных false
    //SerialPrintln("==============================================");
}

void sendMsgEchoAck(int &attempts, int nodeId, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep) {
    inMsg = "";
    MyMessage msg(ChildId, dataType);
    send(msg.setDestination(nodeId).setSensor(ChildId).set(value, 2), true);  //отправляем сообщение
    String outMsg = String(nodeId) + "," +                                    //формируем его уникальную сигнатуру в виде 0,0,12.5;
                    String(ChildId) + "," +
                    String(value) + ";";
    SerialPrintln("sended: " + outMsg);
    long prevMillis = millis();
    wait(1500, C_SET, dataType);  //ждем пока получим echo в функции receive
    long ackTime = millis() - prevMillis;
    if (inMsg == outMsg) {  //если сигнатура полученного эхо совпала с отправленным сообщением - сообщение было доставлено
        SerialPrintln("Msg " + String(ChildId) + " delivered, ack time = " + String(ackTime) + " ms");
        attempts = 0;
        if (goToSleep) sleep(sleepingPeriod);
    } else {  //если не совпала значит в эхо ничего не пришло
        attempts++;
        totalErrors++;
        _transportSM.failedUplinkTransmissions = 0;  //сбросим счетчик в ноль что бы нода не ушла в поиск сети
        SerialPrintln("Msg " + String(ChildId) + " not delivered, attempt: " + String(attempts));
        if (attempts >= attamptsNumber) {
            attempts = 0;
            fatalErrors++;
            SerialPrintln("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + " sec");
            sleep(sleepingPeriod);
        }
    }
}

void sendMsgEchoAck(int &attempts, int nodeId, int ChildId, const mysensors_data_t dataType, long value, bool goToSleep) {
    inMsg = "";
    MyMessage msg(ChildId, dataType);
    send(msg.setDestination(nodeId).setSensor(ChildId).set(value), true);  //отправляем сообщение
    String outMsg = String(nodeId) + "," +                                    //формируем его уникальную сигнатуру в виде 0,0,12.5;
                    String(ChildId) + "," +
                    String(value) + ";";
    SerialPrintln("sended: " + outMsg);
    long prevMillis = millis();
    wait(1500, C_SET, dataType);  //ждем пока получим echo в функции receive
    long ackTime = millis() - prevMillis;
    if (inMsg == outMsg) {  //если сигнатура полученного эхо совпала с отправленным сообщением - сообщение было доставлено
        SerialPrintln("Msg " + String(ChildId) + " delivered, ack time = " + String(ackTime) + " ms");
        attempts = 0;
        if (goToSleep) sleep(sleepingPeriod);
    } else {  //если не совпала значит в эхо ничего не пришло
        attempts++;
        totalErrors++;
        _transportSM.failedUplinkTransmissions = 0;  //сбросим счетчик в ноль что бы нода не ушла в поиск сети
        SerialPrintln("Msg " + String(ChildId) + " not delivered, attempt: " + String(attempts));
        if (attempts >= attamptsNumber) {
            attempts = 0;
            fatalErrors++;
            SerialPrintln("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + " sec");
            sleep(sleepingPeriod);
        }
    }
}

void sendMsgFastAck(int &attempts, int ChildId, const mysensors_data_t dataType, float value, bool goToSleep) {
    MyMessage msg(ChildId, dataType);
    if (send(msg.set(value, 2), false)) {  //если отправилось
        attempts = 0;
        SerialPrintln("Msg " + String(ChildId) + " delivered, value = " + String(value));
        if (goToSleep) sleep(sleepingPeriod);
    } else {
        attempts++;
        totalErrors++;
        SerialPrintln("Msg " + String(ChildId) + " not delivered, attempt: " + String(attempts));
        wait(10);
        if (attempts >= attamptsNumber) {
            attempts = 0;
            fatalErrors++;
            SerialPrintln("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + " sec");
            sleep(sleepingPeriod);
        }
    }
}

void sendMsgFastAck(int &attempts, int ChildId, const mysensors_data_t dataType, long value, bool goToSleep) {
    MyMessage msg(ChildId, dataType);
    if (send(msg.set(value), false)) {  //если отправилось
        attempts = 0;
        SerialPrintln("Msg " + String(ChildId) + " delivered, value = " + String(value));
        if (goToSleep) sleep(sleepingPeriod);
    } else {
        attempts++;
        totalErrors++;
        SerialPrintln("Msg " + String(ChildId) + " not delivered, attempt: " + String(attempts));
        wait(10);
        if (attempts >= attamptsNumber) {
            attempts = 0;
            fatalErrors++;
            SerialPrintln("Go to sleep, gate missing, try again after " + String(sleepingPeriod / 1000) + " sec");
            sleep(sleepingPeriod);
        }
    }
}

void receive(const MyMessage &message) {
    if (message.isEcho()) {
        inMsg = String(message.getSender()) + "," +
                String(message.getSensor()) + "," +
                parseToString(message) + ";";

        SerialPrintln("received: " + inMsg);
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

void SerialPrintln(String text) {
#ifdef SERIAL_PRINT
    Serial.println(text);
#endif
}

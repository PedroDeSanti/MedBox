#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

#include <ESP32Servo.h>

#include <time.h>

#include "../lib/proxy/servo.hpp"

// Replace with your network credentials
const char* ssid = " ";
const char* password = " ";

// NTP server details
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -10800;  // UTC-3 (Brazil) = -3 * 3600 seconds
const int   daylightOffset_sec = 0;  // Brazil doesn't use daylight saving time anymore

// Global timeinfo struct and last sync timestamp
struct tm     timeinfo;
unsigned long lastSyncMillis = 0;  // Last sync time in milliseconds

// Initialize Telegram BOT
#define BOTtoken " "  // your Bot Token (Get from Botfather)
#define SERVO_COUNT 3

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
// #define CHAT_ID ""

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure     client;
UniversalTelegramBot bot(BOTtoken, client);

Servo     meuServo;
const int pinoServo = 15;

// Checks for new messages every 1 second.
int           botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool      ledState = LOW;

enum AlarmeModo {
    NENHUM,
    TEMPO_DIARIO,
    INTERVALO
};

AlarmeModo modoAtual = NENHUM;
// Variáveis para o modo TEMPO_DIARIO
int  horaAlarmeDiario = -1;
int  minutoAlarmeDiario = -1;
bool alarmeDisparadoHoje = false;

// Variáveis para o modo INTERVALO
unsigned long intervaloEmMs = 0;
unsigned long ultimoDisparoIntervalo = 0;

// void set_timer_tempo() {
//     // Reseta as variáveis do modo TEMPO_DIARIO
//     horaAlarmeDiario = -1;
//     minutoAlarmeDiario = -1;
//     alarmeDisparadoHoje = false;
// }

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));

    for (int i = 0; i < numNewMessages; i++) {
        // Chat id of the requester
        String chat_id = String(bot.messages[i].chat_id);

        // // Debug: Print the chat ID to Serial Monitor
        // Serial.print("Chat ID: ");
        // Serial.println(chat_id);

        // if (chat_id != CHAT_ID) {
        //     bot.sendMessage(chat_id, "Unauthorized user", "");
        //     continue;
        // }

        // Print the received message
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
        Serial.println("Mensagem recebida: " + text);

        if (text == "/start") {
            String welcome = "Olá, " + from_name + "!\n";
            welcome += "Escolha um modo de alarme:\n\n";
            welcome += "1. /tempo HH:MM\nDefine um alarme para tocar uma vez por dia no horário especificado. (Ex: "
                       "/tempo 08:30)\n\n";
            welcome += "2. /alarme HH:MM\nDefine um alarme para tocar repetidamente a cada X horas e Y minutos. (Ex: "
                       "/alarme 04:00 para acionar a cada 4 horas)\n\n";
            welcome += "Atenção: Definir um modo desativa o outro.";
            bot.sendMessage(chat_id, welcome, "");
        }

        else if (text.startsWith("/tempo ")) {
            // set_timer_tempo();

            String horario = text.substring(7);
            if (horario.length() == 5 && horario.charAt(2) == ':') {
                int hora = horario.substring(0, 2).toInt();
                int minuto = horario.substring(3, 5).toInt();
                if (hora >= 0 && hora <= 23 && minuto >= 0 && minuto <= 59) {
                    horaAlarmeDiario = hora;
                    minutoAlarmeDiario = minuto;
                    alarmeDisparadoHoje = false;
                    modoAtual = TEMPO_DIARIO;  // Ativa o modo de tempo diário
                    String msg =
                        "Modo de Alarme Diário ATIVADO para " + horario + ".\nO modo de intervalo foi desativado.";
                    bot.sendMessage(chat_id, msg, "");
                    Serial.println(msg);
                } else {
                    bot.sendMessage(chat_id, "Hora inválida.", "");
                }
            } else {
                bot.sendMessage(chat_id, "Formato inválido. Use /tempo HH:MM", "");
            }
        }

        else if (text.startsWith("/alarme ")) {
            String intervaloStr = text.substring(8);
            if (intervaloStr.length() == 5 && intervaloStr.charAt(2) == ':') {
                long horas = intervaloStr.substring(0, 2).toInt();
                long minutos = intervaloStr.substring(3, 5).toInt();
                long totalMinutos = (horas * 60) + minutos;

                if (totalMinutos < 30) {
                    bot.sendMessage(chat_id, "Erro: O intervalo mínimo é de 30 minutos.", "");
                } else {
                    intervaloEmMs = totalMinutos * 60 * 1000;
                    ultimoDisparoIntervalo = millis();  // Inicia a contagem a partir de agora
                    modoAtual = INTERVALO;              // Ativa o modo de intervalo
                    String msg = "Modo de Alarme por Intervalo ATIVADO!\nPróximo acionamento em " + String(horas) +
                                 "h e " + String(minutos) + "min.\nO modo de alarme diário foi desativado.";
                    bot.sendMessage(chat_id, msg, "");
                    Serial.println(msg);
                }
            } else {
                bot.sendMessage(chat_id, "Formato inválido. Use /alarme HH:MM", "");
            }
        } else if (text.startsWith("/servo ")) {
            String arg = text.substring(7);
            int    idx = arg.toInt();

            String msg = "Acionando servo " + String(idx);
            bot.sendMessage(chat_id, msg, "");
            Serial.println(msg);

            meuServo.write(idx);
            Serial.println("Servo posicionado em " + String(idx) + " graus.");

            bot.sendMessage(chat_id, "Teste completo no servo " + String(idx), "");
            // }
        } else if (text == "/liberar") {
            String msg = "Liberando caixinha!";
            bot.sendMessage(chat_id, msg, "");
            Serial.println(msg);

            meuServo.write(140);
            delay(1000);
            meuServo.write(95);

            msg = "Caixinha liberada!";
            bot.sendMessage(chat_id, msg, "");
            Serial.println(msg);
        }
    }
}

void syncTime() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // Configure time with NTP server
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println("\nESP32 Time synchronized with NTP server.");
    Serial.print("Current time: ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    //   // Sync the RTC with the NTP time
    //   rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
    //                       timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));

    lastSyncMillis = millis();  // Record the last sync time in milliseconds
}

void checkTimeAndSync() {
    // Check if 10 seconds have passed since the last sync (10 seconds = 10000 milliseconds)
    if (millis() - lastSyncMillis >= 10000) {
        Serial.println("Synchronizing time with NTP...");
        syncTime();
    }
}

void setup() {
    Serial.begin(9600);

    meuServo.attach(pinoServo);
    meuServo.write(95);

#ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");  // get UTC time via NTP
    client.setTrustAnchors(&cert);     // Add root certificate for api.telegram.org
#endif

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, ledState);

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
#ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
#endif
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    // Print ESP32 Local IP Address
    Serial.println("Connected to the WiFi network");
    Serial.println(WiFi.localIP());
}

void loop() {
    checkTimeAndSync();

    // print current time
    // Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    if (millis() > lastTimeBotRan + botRequestDelay) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages) {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}

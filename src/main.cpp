// #include <Arduino.h>

// #include "led.hpp"

// void setup() { }

// void loop() { }

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "";
const char* password = "";

// Initialize Telegram BOT
#define BOTtoken ""  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
// #define CHAT_ID ""

#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure     client;
UniversalTelegramBot bot(BOTtoken, client);

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
        }
    }
}

void setup() {
    Serial.begin(9600);

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
    Serial.println(WiFi.localIP());
}

void loop() {
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

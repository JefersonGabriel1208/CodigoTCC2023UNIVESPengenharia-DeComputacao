#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLkSZZzXll"
#define BLYNK_DEVICE_NAME "Quickstart Template"             // device blynk
#define BLYNK_AUTH_TOKEN "coQ4EmghhrmA9cOlWvSRF_7E8i46qX-v" // token blynk
#define BLYNK_FIRMWARE_VERSION "1.1.0"

#include <BlynkSimpleEsp8266.h> //Biblioteca para uso do ESP8266 no Blynk
#include <ESP8266WebServer.h>   //Biblioteca para o ESP funcionar como servidor
#include <ESP8266WiFi.h>        //Biblioteca para funcionamento do WiFi do ESP8266
#include <WiFiClient.h>         //biblioteca para cliente conseguir acessar pagina
#include <DHT.h>                //biblioteca sensor DHT11

char auth[] = "coQ4EmghhrmA9cOlWvSRF_7E8i46qX-v"; // token blynk
const char *ssid = "Sx";                          // Rede WiFi
const char *password = "felicidade23";            // Senha da Rede WiFi

// sensordht11
#define DHTPIN 12         // Pino digital D6 (GPIO5) conectado ao DHT11
#define DHTTYPE DHT11     // Tipo do sensor DHT11
DHT dht(DHTPIN, DHTTYPE); // objeto dht, instancia da classe DHT, que recebe dois parâmetros DHTPIN e DHTTYPE
BlynkTimer timer;

// Sensor de presença
#define pinPIR 14 // Pino digital D5 (GPIOS) conectado ao Sensor pir hc-sr501

// multiplexador cd74hc2067
#define S0 D0 // S0, S1, S2, S3 são portas do multiplexador cd74hc2067 conectadas às portas D0, D1, D2, D3 do esp8266
#define S1 D1
#define S2 D2
#define S3 D3
#define smokeA0 A0 // variável utilizada para entrada analógica do esp8266
#define LED D8

int sensor0mq2;       // variável para sensor Mq2
int sensor1mq7;       // variável para sensor Mq2
int sensorchama = D7; // Variável para definição do pino digital do sensor de chamas
int buzzer = D4;      // variável para definição pino do buzzer
int sensorGLP = 400;  // variável com nome sensorGLP que armazena o valor critico dos gases em ppm (partes por milhão)
float temperatura;    // variável para armazenar a temperatura
float umidade;        // Variável para armazenar a umidade

void setup()
{
  Serial.begin(115200);              // Inicializa a comunicação serial
  delay(50);                         // ?Intervalo para aguardar a estabilização do sistema
  dht.begin();                       // Inicializa o sensor DHT11
  Blynk.begin(auth, ssid, password); // inicializa o blynk pegando suas credenciais

  /* Timers */
  timer.setInterval(1000L, getSensor0mq2); // executa a função getSensor0mq2 a cada 1 segundo
  timer.setInterval(2500L, getSensor1mq7); // executa a função getSensor1mq7 a cada 2.5 segundos
  // é necessário pois com o multiplexador os pinos não funcionam ao mesmo tempo, tendo uma pequena diferença não terá conflitos

  // sensor Mq2 e Sensor de Chamas
  pinMode(sensorchama, INPUT); // definição do pino do sensor de chamas como entrada
  pinMode(buzzer, OUTPUT);     // definição do pino buzzer como saída
  pinMode(smokeA0, INPUT);     // definição do pino A0 como entrada
  pinMode(pinPIR, INPUT);      // definição do pino D5 ( sensor de chamas) como entrada

  /* Multiplexer */
  pinMode(S0, OUTPUT); // S0, S1, S2, S3 do multiplexador configurados como saída, e o led também
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(LED, OUTPUT);

  Serial.println("Conectando a Rede: "); // Imprime na serial a mensagem
  Serial.println(ssid);                  // Imprime na serial o nome da Rede Wi-Fi
  WiFi.begin(ssid, password);            // Inicialização da comunicação Wi-Fi

  // Verificação da conexão
  while (WiFi.status() != WL_CONNECTED)
  { // Enquanto estiver aguardando status da conexão
    delay(1000);
    Serial.print("."); // Imprime pontos
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); // Função para exibir o IP da ESP8266
}

void loop()
{
  server.handleClient(); // Chama o método handleClient()
  Blynk.run();           // Chama a função Blynk.run()
  timer.run();           // Chama a função timer.run()

  // Sensor pir Hc- SR501
  bool valorPIR = digitalRead(pinPIR); // variável booleana valorPIR responsável por armazenar valor lido atraves do comando digitalRead

  if (valorPIR == HIGH) // lógica se valor do sensor de presença for TRUE executa os comandos listados
  {
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(buzzer, HIGH);
    delay(1000);
    Serial.print("Movimento Detectado");
  }
  else
  {
    Serial.println("Nenhum Movimento Detectado");
    digitalWrite(LED, LOW);
    digitalWrite(buzzer, LOW);
  }
  Blynk.virtualWrite(V0, valorPIR); // envia no pino virtual V0 o valor auferido pelo sensor de presença

  // Sensor de Chamas
  int digitalsensor = digitalRead(sensorchama); // variável  digitalsensor responsável por armazenar valor lido atraves do comando digitalRead
  Blynk.virtualWrite(V7, digitalsensor);        // envia no pino virtual V7 o valor auferido pelo sensor de chamas
  Serial.println(digitalsensor);
  delay(1000);

  if (digitalsensor == 0) // sensor de chamas adquirido para projeto veio com lógica contrária por isso se for =0 indica fogo/chamas
  {                       // comandos a serem executados caso o sensor detecte fogo/chamas
    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    delay(1000);
    Serial.println("Fogo Detectado");
  }
  else
  {
    digitalWrite(LED, LOW);
    digitalWrite(buzzer, LOW);
    Serial.println("Ausência de Fogo");
  }

  // DHT 11
  temperatura = dht.readTemperature(); // Realiza a leitura da temperatura
  umidade = dht.readHumidity();        // Realiza a leitura da umidade
  Serial.begin(115200);
  Serial.print("Temperatura: ");
  Serial.print(temperatura); // exibe no monitor serial o valor da temperatura lida
  Serial.println(" ºC");
  Serial.print("Umidade: ");
  Serial.print(umidade); // exibe no monitor serial o valor da umidade lida
  Serial.println(" %");
  delay(1000);
  Blynk.virtualWrite(V5, umidade);     // envia no pino virtual V5 o valor de h (umidade)
  Blynk.virtualWrite(V6, temperatura); // envia no pino virtual V6 o valor de t (temperatura)
}

void getSensor0mq2() // função para funcionamento sensor MQ2
{                    /*abaixo verfica-se comando digitalwrite para definição de pinos do multiplexador cd74hc2067
                     este multiplexador utiliza lógica binária, neste caso o Mq2 utiliza 0 0 0 0 para ser conectado ao pino Y0 do multiplexador*/

  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  sensor0mq2 = analogRead(smokeA0); // le o valor analógico auferido pelo sensor MQ2

  Blynk.virtualWrite(V2, sensor0mq2); // envia no pino virtual V2 o valor auferido pelo sensor MQ2
  Serial.print("Mq2: ");
  Serial.println(sensor0mq2);

  if (sensor0mq2 > sensorGLP) // if para se o valor do sensor for maior que sensorThres que é 400, acende led e aciona o buzzer
  {
    digitalWrite(LED, HIGH);
    digitalWrite(buzzer, HIGH);
    delay(1000);
  }
  else
  {
    digitalWrite(LED, LOW);
    digitalWrite(buzzer, LOW);
    delay(1000);
  }
}

void getSensor1mq7() // função para funcionamento sensor MQ2
/*abaixo verfica-se comando digitalwrite para definição de pinos do multiplexador cd74hc2067
   este multiplexador utiliza lógica binária, neste caso o Mq2 utiliza 0 0 1 1 para ser conectado ao pino Y3 do multiplexador*/
{

  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  sensor1mq7 = analogRead(smokeA0); // le o valor analógico auferido pelo sensor MQ7

  Blynk.virtualWrite(V1, sensor1mq7); // envia no pino virtual V1 o valor auferido pelo sensor MQ7
  Serial.print("Mq7: ");
  Serial.println(sensor1mq7);

  if (sensor1mq7 > 15) // if para se o valor do sensor for maior que 15ppm , acende o led e aciona o buzzer
  {
    digitalWrite(buzzer, HIGH);
    digitalWrite(LED, HIGH);
    delay(1000);
  }
  else
  {
    digitalWrite(LED, LOW);
    digitalWrite(buzzer, LOW);
    delay(1000);
  }
}

void handle_OnConnect()
{
}
void handle_NotFound()
{ // Função para lidar com o erro 404
  server.send(404, "text/plain", "Não encontrado");
}
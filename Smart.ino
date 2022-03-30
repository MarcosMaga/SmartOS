#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

Ticker time1000;
WiFiClient wifiClient;

int bt3 = D5;
int bt2 = D6;
int bt1 = D7;
int buzzer = D8;

int hora;
int minuto = 58;
int segundo;
int dia;
int tempBloq = 10;
int timerBloq;

int btEsquerda;
int btDireita;
int btCentro;
int preparaD;
int preparaC;
int preparaE;

int indice;
int indiceConfig;

int horaConfig;
int minutoConfig;
int i;

int indiceWifi;
int nav;
int tempor;
int n;
int navNet;

int indiceDespertador;
int horaDespertador;
int minutoDespertador;
int horaBck;
int minutoBck;
int soneca = 5;

bool direita;
bool centro;
bool esquerda;

bool acao;

bool bloquear;

bool configWifi;
bool wifi;

bool som;

bool despertador;
bool configDespertador;
bool acionado;

unsigned long sinc;
unsigned long delay1;

float versao = 1.28;
float leitura;
float bateria;

char caracteres[64] = {'a', 'b', 'c', 'd','e','f','g','h','i','j','k','l','n','m','o','p','q','r','s','t','u','v','w','y','z','A','B','C','D','E','F','G','H','I','J','K','L','N','O','P','Q','R','S','T','U','V','W','Y','Z','0','1','2','3','4','5','6','7','8','9', '.', ',','_',' ', '!'};

String ssidT;
String passwordT;
String dias[7] = {"Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado", "Domingo"};

long totalMillis;

//MQTT
const char* BROKER_MQTT = "mqtt.eclipse.org";
int BROKER_PORT = 1883;
int indiceMQTT;
int navMQTT;
String Protocoll[2] = {"Computador", "Sair"};

#define ID_MQTT "RIOT"
PubSubClient MQTT(wifiClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  time1000.attach(1.0, timer);

  pinMode(buzzer,OUTPUT);
  pinMode(bt3, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(bt1, INPUT_PULLUP);
  
  
  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(10,10);
  display.println("SMART!");
  display.setTextSize(0);
  display.setCursor(90,20);
  display.print(versao);
  display.print("v");
  display.display();
  tone(buzzer, 4000, 100);
  delay(200);
  tone(buzzer, 4000, 100);
  delay(2500);
  display.clearDisplay();
  display.display();

}

void loop() {
  // put your main code here, to run repeatedly:
  btEsquerda = digitalRead(bt3);
  btCentro = digitalRead(bt2);
  btDireita = digitalRead(bt1);
  leitura = analogRead(A0);

  bateria = ((3.3 * leitura) / 1024) * 2;
  Serial.println(bateria);

  if(MQTT.connected())
  {
    MQTT.loop();
    Serial.println("Conectado");
  }
  
  if(hora == horaDespertador && minuto == minutoDespertador && segundo == 0 && despertador)
  {
    acionado = true;
    bloquear = false;
    indice = 1;
  }

  if(WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.begin();
    ArduinoOTA.handle();
  }
  
  Relogio();
  Teclado();
 
  if(btCentro + btEsquerda + btDireita != 3)
  {

    if(bloquear)
    {
      bloquear = false;
    }

    if(som)
    {
      tone(buzzer, 4000, 100);
    }
    
    timerBloq = 0;
  }

  if(!bloquear)
  {
     switch(indice)
      {
        case 0: Menu(); break;
        case 1: Despertador(); break;
        case 2: MQTTprotocol(); break;
        case 3: WifiConfig(); break;
        case 4: Config(); break;
      }
  }

  if(timerBloq >= tempBloq)
  {
    bloquear = true;
    timerBloq = tempBloq;
  }

  if(bloquear)
  {
    display.clearDisplay();
    display.display();
  }
  
  if(indice < 0)
  {
    indice = 4;
  }
  else if(indice > 4)
  {
    indice = 0;
  }

  if(!acao)
  {
    if(esquerda)
    {
      indice--;
      display.clearDisplay();
    }
    else if(direita)
    {
      indice++;
      display.clearDisplay();
    }
  }

}

void timer()
{
  segundo++;
  timerBloq++;
}

void Relogio()
{

  if(segundo >= 60)
  {
    minuto++;
    segundo = 0;
  }

  if(minuto >= 60)
  {
    hora++;
    minuto = 0;
  }

  if(hora >= 24)
  {
    hora = 0;
    dia++;
  }

  if(dia > 6)
  {
    dia = 0;
  }
  else if(dia < 0)
  {
    dia = 6;
  }
}

void Teclado()
{
 
  if(btDireita == 0)
  {
    preparaD = 1;
  }
  else
  {
    if(preparaD == 1)
    {
      direita = true;
      preparaD = 0;
    }
    else
    {
      direita = false;
    }
  }

   if(btEsquerda == 0)
  {
    preparaE = 1;
  }
  else
  {
    if(preparaE == 1)
    {
      esquerda = true;
      preparaE = 0;
    }
    else
    {
      esquerda = false;
    }
  }

   if(btCentro == 0)
  {
    preparaC = 1;
  }
  else
  {
    if(preparaC == 1)
    {
      centro = true;
      preparaC = 0;
    }
    else
    {
      centro = false;
    }
  }
}


void Menu()
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(dias[dia]);
  display.setCursor(30,18);
  display.setTextSize(2);
  if(hora < 10){display.print("0");}  
  display.print(hora);
  display.print(":");
  if(minuto < 10){display.print("0");}
  display.print(minuto);
  display.print(":");
  display.setTextSize(1);
  display.print(segundo);
  display.display();
}

void Config()
{

  if(centro && !acao)
  {
    acao = true;
    display.clearDisplay();
    centro = false;
  }
  
  if(!acao)
  {
    i = 0;
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(0);
    display.setCursor(45,0);
    display.print("CONFIG");
    display.setCursor(0,10);
    display.println("Aperte para iniciar!");
    display.display();
  }
  else
  {
    timerBloq = 0;

    if(horaConfig >= 24)
    {
      horaConfig = 0;
    }
    else if(horaConfig < 0)
    {
      horaConfig = 23;
    }

    if(minutoConfig > 59)
    {
      minutoConfig = 0;
    }
    else if(minutoConfig < 0)
    {
      minutoConfig = 59;
    }

    if(indiceConfig == 0)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.setTextSize(0);
      display.print("USE < OU > PARA MUDAR");
      display.println("Temp Bloq: ");
      display.print(tempBloq);
      display.print("s");
      display.display();

      if(esquerda)
      {
        tempBloq--;
      }
      else if(direita)
      {
        tempBloq++;
      }
      else if(centro)
      {
        indiceConfig++;
      }
    }
    else if(indiceConfig == 1)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(50,0);
      display.setTextSize(0);
      display.println("SOM");
      if(som)
      {
        display.print("On");
      }
      else
      {
        display.print("Off");
      }
      display.display();

      if(esquerda || direita)
      {
        som = !som;
      }
      else if(centro)
      {
        indiceConfig++;
        centro = false;
      }
    }
    else if(indiceConfig == 2)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(50,0);
      display.setTextSize(0);
      display.println("DIA");
      display.println(dias[dia]);
      display.display();

      if(esquerda)
      {
        dia--;
      }
      else if(direita)
      {
        dia++;
      }
      else if(centro)
      {
        indiceConfig++;
        centro = false;
      }
    }
    else if(indiceConfig == 3)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(40,0);
      display.setTextSize(0);
      display.println("SONECA");
      display.setTextSize(2);
      display.print(soneca);
      display.println("s");
      display.display();

      if(soneca < 1)
      {
        soneca = 59;
      }
      else if(soneca > 59)
      {
        soneca = 1;
      }

      if(esquerda)
      {
        soneca--;
      }
      else if(direita)
      {
        soneca++;
      }
      else if(centro)
      {
        indiceConfig++;
        centro = false;
      }
    }
    else if(indiceConfig == 4)
    {
      if(i == 0)
    {
      if(esquerda)
      {
        horaConfig--;
      }
      else if(direita)
      {
        horaConfig++;
      }
      else if(centro)
      {
        i++;
      }
    }
    else if(i == 1)
    {
      if(esquerda)
      {
        minutoConfig--;
      }
      else if(direita)
      {
        minutoConfig++;
      }
      else if(centro)
      {
        i++;
      }
    }
    else
    {
      hora = horaConfig;
      minuto = minutoConfig;
      segundo = 0;
      horaConfig = 0;
      minutoConfig = 0;
      indiceConfig = 0;
      acao = false;
    }
  
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.setTextSize(0);
    display.print("USE < OU > PARA MUDAR");
    display.setCursor(30,18);
    display.setTextSize(2);
    if(horaConfig < 10){display.print("0");}  
    display.print(horaConfig);
    display.print(":");
    if(minutoConfig < 10){display.print("0");}
    display.print(minutoConfig);
    display.display();
    }
      
  } 
}

void WifiConfig()
{

  if(nav > 63)
  {
    nav = 0;
  }
  else if(nav < 0)
  {
    nav = 63;
  }
  
  if(!configWifi)
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(45,0);
    display.println("WI-FI");
    display.println("Aperte p/ configurar");
    display.print("Status: ");

      switch(WiFi.status())
      {
        case WL_CONNECTED: display.println("Conectado"); display.println(WiFi.localIP()); break;
        case WL_NO_SHIELD: display.println("Sem shield"); break;
        case WL_IDLE_STATUS: display.println("Desconfig"); break;
        case WL_NO_SSID_AVAIL: display.println("SSID indisponivel"); break;
        case WL_CONNECT_FAILED: display.println("Conexao falhou"); break;
        case WL_CONNECTION_LOST: display.println("Conexão perdida"); break;
        case WL_DISCONNECTED: display.println("Desconectado"); break;
      } 
    
    display.display();

    if(centro)
    {
      configWifi = true;
      acao = true;
      n = WiFi.scanNetworks();
      centro = false;
    }
  }
  else
  {
    timerBloq = 0;

    if(indiceWifi == 0)
    {
      WiFi.disconnect();
      indiceWifi++;
    }
    else if(indiceWifi == 1)
    {
      
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(30,0);
      display.println("NOME DA REDE");
      display.print("Nome:");

      if(navNet >= n)
      {
        display.println("Adicionar rede");
      }
      else
      {
         display.println(WiFi.SSID(navNet));
      }

      display.display();
      
      if(navNet > n)
      {
        navNet = 0;
      }
      else if(navNet < 0)
      {
        navNet = n;
      }

      if(esquerda)
      {
        navNet--;
      }
      else if(direita)
      {
        navNet++;
      }
      else if(centro && navNet < n)
      {
        ssidT = WiFi.SSID(navNet);
        indiceWifi++;
        centro = false;
      }
      else if(centro && navNet >= n)
      {
        indiceWifi = 6;
        centro = false;
      }
      
    }
    else if(indiceWifi == 2)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(30,0);
      display.println("SENHA DA REDE");
      display.print("Senha:");
      display.println(passwordT);
      display.println(caracteres[nav]);
      display.display();

      if(esquerda)
      {
        nav--;
      }
      else if(direita)
      {
        nav++;
      }
      else if(centro && nav != 63)
      {
        passwordT += caracteres[nav];
      }
      else if(centro && nav == 63)
      {
        indiceWifi++;
        centro = false;
      }
    }
    else if(indiceWifi == 3)
    {
     WiFi.begin(ssidT, passwordT);
      indiceWifi++;
    }
    else if(indiceWifi == 4)
    {   
      while(WiFi.status() != WL_CONNECTED && tempor < 200)
      {
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(0,0);
          display.println("Conectando...");
          display.display();
          tempor++;
      }
      tempor = 0;
        
        indiceWifi++;
    }
    else if(indiceWifi == 5)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(0,0);
      display.println("Conectando...");

      switch(WiFi.status())
      {
        case WL_CONNECTED: display.println("Conectado"); display.println(WiFi.localIP()); break;
        case WL_NO_SSID_AVAIL: display.println("SSID indisponivel"); display.println("Conexao falhou"); break;
        case WL_CONNECT_FAILED: display.println("Conexao falhou"); break;
        case WL_CONNECTION_LOST: display.println("Conexão perdida"); break;
        case WL_DISCONNECTED: display.println("Desconectado"); break;
      } 
      display.println("Aperte para sair");
      display.display();

      if(centro)
      {
        
        configWifi = false;
        acao = false;
        indiceWifi = 0;
        centro = false;
      }
    }
    else if(indiceWifi == 6)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(30,0);
      display.println("NOME DA REDE");
      display.print("NOME:");
      display.println(ssidT);
      display.println(caracteres[nav]);
      display.display();

      if(esquerda)
      {
        nav--;
      }
      else if(direita)
      {
        nav++;
      }
      else if(centro && nav != 63)
      {
        ssidT += caracteres[nav];
      }
      else if(centro && nav == 63)
      {
        indiceWifi = 2;
        centro = false;
      }
    }
  }
}

void Despertador()
{
 
  if(!acionado)
  {
    
      if(horaDespertador > 23)
      {
        horaDespertador = 0;
      }
      else if(horaDespertador < 0)
      {
        horaDespertador = 23;
      }
    
      if(minutoDespertador > 59)
      {
        minutoDespertador = 0;
      }
      else if(minutoDespertador < 0)
      {
        minutoDespertador = 59;
      }
    
     if(!despertador)
    {
  
        if(indiceDespertador == 0)
        {
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(30,0);
          display.println("DESPERTADOR");
          display.setCursor(7,20);
          display.print("Aperte para ativar");
          display.display();
  
          if(centro)
          {
            indiceDespertador++;
            acao = true;
            centro = false;
          }
        }
        else if(indiceDespertador == 1)
        {
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.setTextSize(0);
          display.print("USE < OU > PARA MUDAR");
          display.setCursor(30,18);
          display.setTextSize(2);
          if(horaDespertador < 10){display.print("0");}  
          display.print(horaDespertador);
          display.print(":");
          if(minutoDespertador < 10){display.print("0");}
          display.print(minutoDespertador);
          display.display();
  
          if(esquerda)
          {
            horaDespertador--;
          }
          else if(direita)
          {
            horaDespertador++;
          }
          else if(centro)
          {
            indiceDespertador++;
            centro = false;
          }
        }
        else if(indiceDespertador == 2)
        {
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setCursor(0,0);
          display.setTextSize(0);
          display.print("USE < OU > PARA MUDAR");
          display.setCursor(30,18);
          display.setTextSize(2);
          if(horaDespertador < 10){display.print("0");}  
          display.print(horaDespertador);
          display.print(":");
          if(minutoDespertador < 10){display.print("0");}
          display.print(minutoDespertador);
          display.display();
  
          if(esquerda)
          {
            minutoDespertador--;
          }
          else if(direita)
          {
            minutoDespertador++;
          }
          else if(centro)
          {
            acao = false;
            despertador = true;
            horaBck = horaDespertador;
            minutoBck = minutoDespertador;
            indiceDespertador = 0;
            centro = false;
          }
        }
        
    }
    else
    {
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setCursor(30,0);
          display.setTextSize(0);
          display.println("DESPERTADOR");
          display.println("Ativado para:");
          display.setCursor(30,18);
          display.setTextSize(2);
          if(horaDespertador < 10){display.print("0");}  
          display.print(horaDespertador);
          display.print(":");
          if(minutoDespertador < 10){display.print("0");}
          display.print(minutoDespertador);
          display.display();
  
          if(centro)
          {
            despertador = false;
            horaDespertador = 0;
            minutoDespertador = 0;
          }
    }
  }
  else
  {
      tone(buzzer, 4000, 1000);
      acao = true;
      timerBloq = 0;

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(30,0);
      display.setTextSize(0);
      display.println("DESPERTADOR");
      display.println("<Soneca    Desligar>:");
      display.display();

      if(horaDespertador > 23)
      {
        horaDespertador = 0;
      }

      if(esquerda)
      {

        minutoDespertador += soneca;
        
        if(minutoDespertador > 59)
        {
          minutoDespertador = minutoDespertador - 60;
          horaDespertador++;
        }
        noTone(buzzer);
        acao = false;
        acionado = false;
      }
      else if(direita)
      {
        minutoDespertador = minutoBck;
        horaDespertador = horaBck;
        noTone(buzzer);
        acao = false;
        acionado = false;
      }
  }
}

void MQTTprotocol()
{
  if(indiceMQTT == 0)
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(50,0);
    display.println("MQTT");
  
      if(WiFi.status() == WL_CONNECTED)
      {
        display.println("Aperte para iniciar");
      }
      else
      {
        display.println("Sem internet!");
      }

      if(MQTT.connected())
      {
        display.println("MQTT Conectado");
      }
      else
      {
        display.println("MQTT Desconectado");
      }
      display.display();

      if(centro)
      {
        indiceMQTT++;
        centro = false;
      }
   }
   if(WiFi.status() == WL_CONNECTED && indiceMQTT != 0)
   {
    if(!MQTT.connected())
    {
      if(indiceMQTT == 1)
      {
        MQTT.setServer(BROKER_MQTT, BROKER_PORT);
        indiceMQTT = 2;
      }
      else if(indiceMQTT == 2)
      {
    
        while(!MQTT.connected())
        {
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(0,0);
          
          display.println("Conectando:");
          display.println(BROKER_MQTT);
          if(MQTT.connect(ID_MQTT))
          {
            display.println("Conectado!");

          }
          else
          {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
          }
          display.display();
        }
        
        indiceMQTT = 3;
        acao = true;
      }
    }
    else
    {
       indiceMQTT = 3;
       acao = true;
    }
      
      if(indiceMQTT == 3)
      {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(30,0);
        display.println("PUBLICAR");
        display.println("Protocolo:");
        display.print(Protocoll[navMQTT]);
        display.display();

        Serial.println(navMQTT);

        if(navMQTT > 1)
        {
          navMQTT = 0;
        }
        else if(navMQTT < 0)
        {
          navMQTT = 1;
        }

        if(esquerda)
        {
          navMQTT--;
        }
        else if(direita)
        {
          navMQTT++;
        }
        else if(centro && navMQTT != 1)
        {
          
        }
        else if(centro && navMQTT == 1)
        {
          indiceMQTT = 0;
          acao = false;
          centro = false;
          delay(1000);
        }
      }
   }
   else
   {
    indiceMQTT == 0;
   }

}


//PROGRAMA ESP32 OBD2 SCAN//

#include <CAN.h>
#include <WiFi.h>
#include <WiFiClient.h>

unsigned char DistTravelled[8] =  {0x02, 0x01, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char Throttle[8] =       {0x02, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char EngineSpeed[8] =    {0x02, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char VehicleSpeed[8] =   {0x02, 0x01, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char CoolantTemp[8] =    {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char EngRunTime[8] =     {0x02, 0x01, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char IAT[8] =            {0x02, 0x01, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char MAF[8] =            {0x02, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char MAP[8] =            {0x02, 0x01, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char FuelTankLevel[8] =  {0x02, 0x01, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x00};

double temperature;
double velocidade;
double velocidade_app;
double rpm;
double rpm_app;
double distancia;
double acelerador;
double iat;
double combustivel;
unsigned int obd_msg=0;

//// Dados da rede WiFi
const char* ssid = "TCC MATHEUS";
const char* password = "12345678";

WiFiServer server(80);
///////////////////////////////////////////////////////////////////////////

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 90);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 100);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional


void setup() {
  Serial.begin(115200);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  
  Serial.print("\nConectando a rede: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);                   // Conecta na rede


  int i = 0;
  while (WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
  if(++i > 30) ESP.restart();                 // Reinicia ESP após 30 segundos
  }
  Serial.println("\nWiFi Conectado!");
  server.begin();                               // Inicia Server
  Serial.print("Use essa URL para conectar: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
  Serial.println("CAN OBD-II");
  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    ESP.restart(); 
  }
  CAN.onReceive(onReceive);


  // Adiciona filtros para receber apenas a mensagem CAN com ID apresentado.
    CAN.filter(0x7E8);
}

void loop() {

          /*Solicitação dos dados*/
            switch(obd_msg){            
            
            case 0://Temperatura do Liquido de Arrefecimento(°C)
                   CAN.beginPacket(0x7DF, 8);
                   CAN.write(CoolantTemp,sizeof(CoolantTemp));
                   CAN.endPacket();

                   break;
            case 1://Velocidade do Veiculo(km/h)
                   CAN.beginPacket(0x7DF, 8);
                   CAN.write(VehicleSpeed,sizeof(VehicleSpeed));
                   CAN.endPacket();

                   break;
            case 2://POSIÇÃO DO ACELERADOR(%)
                   CAN.beginPacket(0x7DF, 8);
                   CAN.write(Throttle,sizeof(Throttle));
                   CAN.endPacket();

                   break;
            case 3://TEMPERATURA DO AR DE ENTRADA(°C)
                   CAN.beginPacket(0x7DF, 8);
                   CAN.write(IAT,sizeof(IAT));
                   CAN.endPacket();
                 
                   break;
            case 4://NIVEL DO COMBUSTIVEL(%)
                   CAN.beginPacket(0x7DF, 8);
                   CAN.write(FuelTankLevel,sizeof(FuelTankLevel));
                   CAN.endPacket();

                   break;
            case 5://RPM DO MOTOR(RPM)
                   CAN.beginPacket(0x7DF, 8);
                   CAN.write(EngineSpeed,sizeof(EngineSpeed));
                   CAN.endPacket();

                   break;
            case 6://DISTANCIA PERCORRIDA(KM)
                   CAN.beginPacket(0x7DF, 8);
                   CAN.write(DistTravelled,sizeof(DistTravelled));
                   CAN.endPacket();
                   break;                                                                                                         
           default:
                   obd_msg=0;
                   break;
            }           

                WiFiClient client = server.available();   // listen for incoming clients
    
              if (client) {                             // if you get a client,
                Serial.println("New Client.");           // print a message out the serial port
                String currentLine = "";                // make a String to hold incoming data from the client
                while (client.connected()) {            // loop while the client's connected
                  if (client.available()) {             // if there's bytes to read from the client,
                    char c = client.read();             // read a byte, then
                    Serial.write(c);                    // print it out the serial monitor
                    if (c == '\n') {                    // if the byte is a newline character
            
                      // if the current line is blank, you got two newline characters in a row.
                      // that's the end of the client HTTP request, so send a response:
                      if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();
            
                        // the content of the HTTP response follows the header:
                        client.print(temperature);
                        client.print(",");
                        client.print(velocidade_app);
                        client.print(",");
                        client.print(rpm_app);
                        client.print(",");
                        client.print(distancia);
                        client.print(",");
                        client.print(acelerador);
                        client.print(",");
                        client.print(iat);
                        client.print(",");
                        client.print(combustivel);
            
                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                      } else {    // if you got a newline, then clear currentLine:
                        currentLine = "";
                      }
                    } else if (c != '\r') {  // if you got anything else but a carriage return character,
                      currentLine += c;      // add it to the end of the currentLine
                    }
            
                    // Check to see if the client request was "GET /H" or "GET /L":
                    if (currentLine.endsWith("GET /E")) {
                              Serial.println("Client Teste.");
                    }
                  }
                }
                // close the connection:
                client.stop();
                Serial.println("Client Disconnected.");
              }
        delay(10);
}

void onReceive(int packetSize) {

      int length_data=CAN.read();
      int service_respose=CAN.read();
      
      if(length_data == 0x03 && service_respose == 0x41){
      
            switch(CAN.read()){            
            
            case 0x05://Temperatura do Liquido de Arrefecimento(°C)
                      temperature = CAN.read() - 40;
                      Serial.print("Temperatura °C = ");
                      Serial.println(temperature);
                      obd_msg=1;
                      break;
            case 0x0D://Velocidade do Veiculo(km/h)
                      velocidade = CAN.read();
                      Serial.print("Velocidade Km/h = ");
                      Serial.println(velocidade);
                      if(velocidade<=0){
                        velocidade_app=135;
                      }
                      else if(velocidade>0 && velocidade<=120){
                        velocidade_app =(135-(velocidade*1.125));
                      }else if(velocidade>120 && velocidade<=240){
                        velocidade_app =(135-(velocidade*1.125));
                      }else{ 
                        velocidade_app=-135;
                      }
                      obd_msg=2;
                      break;
            case 0x11://POSIÇÃO DO ACELERADOR(%)
                      acelerador = (0.39215686274*CAN.read());
                      Serial.print("Posição do Acelerador(%) = ");
                      Serial.println(acelerador);
                      obd_msg=3;
                      break;
            case 0x0F://TEMPERATURA DO AR DE ENTRADA(°C)
                      iat = (CAN.read()-40);
                      Serial.print("Temperatura do Ar de entrada(°C) = ");
                      Serial.println(iat);
                      obd_msg=4;
                      break;
            case 0x2F://NIVEL DO COMBUSTIVEL(%)
                      combustivel = (0.39215686274*CAN.read());
                      Serial.print("Nivel do Combustivel(%) = ");
                      Serial.println(combustivel);
                      obd_msg=5;
                      break; 
                                                                 
             default: 
                      break;
            }
      }
      else if(length_data == 0x04 && service_respose == 0x41){
      
            switch(CAN.read()){            
            
            case 0x0C://RPM DO MOTOR(RPM)
                      rpm = ((CAN.read() * 256.0) + CAN.read()) / 4.0;
                      Serial.print("Engine RPM = ");
                      Serial.println(rpm);
                      if(rpm<=0){
                        rpm_app=135;
                      }
                      else if(rpm>0 && rpm<=6000){
                        rpm_app =(135-(rpm*0.0225));
                      }else if(rpm>6000 && rpm<=12000){
                        rpm_app =(135-(rpm*0.0225));
                      }else{ 
                        rpm_app=-135;
                      }
                      obd_msg=6;
                      break;
            case 0x31://DISTANCIA PERCORRIDA(KM)
                      distancia = ((CAN.read() * 256.0) + CAN.read());
                      Serial.print("Distancia(KM) = ");
                      Serial.println(distancia);
                      obd_msg=0;
                      break;                                                                     
             default: 
                      break;
            }
      }
}

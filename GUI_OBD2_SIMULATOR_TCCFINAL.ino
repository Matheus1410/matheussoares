
//PROGRAMA ESP32 OBD2 SIMULATOR//

#include <CAN.h>


#define REC_B_SIZE 4
#define TOTAL_PARAM 10

int msg=0;

unsigned char DistTravelled[8] = {0x04, 0x41, 0x31, 0x50, 0x00, 0x00, 0x00, 0x00};
unsigned char Throttle[8] = {0x03, 0x41, 0x11, 0x02, 0x00, 0x00, 0x00, 0x00};
unsigned char EngineSpeed[8] =  {0x04, 0x41, 0x0C, 0x4e, 0x20, 0x00, 0x00, 0x00};
unsigned char VehicleSpeed[8] = {0x03, 0x41, 0x0D, 0x50, 0x00, 0x00, 0x00, 0x00};
unsigned char CoolantTemp[8] = {0x03, 0x41, 0x05, 0x46, 0x00, 0x00, 0x00, 0x00};
unsigned char EngRunTime[8] = {0x04, 0x41, 0x1F, 0x00, 0x78, 0x00, 0x00, 0x00};
unsigned char IAT[8] = {0x03, 0x41, 0x0F, 0x60, 0x00, 0x00, 0x00, 0x00};
unsigned char MAF[8] = {0x04, 0x41, 0x10, 0x00, 0xE5, 0x00, 0x00, 0x00};
unsigned char MAP[8] = {0x03, 0x41, 0x0B, 0x22, 0x00, 0x00, 0x00, 0x00};
unsigned char FuelTankLevel[8] = {0x03, 0x41, 0x2f, 0x22, 0x00, 0x00, 0x00, 0x00};


void setup()
{
  Serial.begin(115200);

  Serial.println("CAN Receiver");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    ESP.restart();  
  }
    CAN.onReceive(onReceive);
  // Adiciona filtros para receber apenas a mensagem CAN com ID apresentado.
    CAN.filter(0x7DF);

}

void loop(){

      if(Serial.available()){
              int ind = 0;
              unsigned char databyte[REC_B_SIZE * TOTAL_PARAM] = {0};
              delay(10);
              while (Serial.available() && (ind < (REC_B_SIZE * TOTAL_PARAM))) {
                databyte[ind] = Serial.read();
                ind++;
              }
              if (ind == (REC_B_SIZE * TOTAL_PARAM)){
        
                DistTravelled[4] = databyte[3];
                DistTravelled[3] = databyte[2];
                VehicleSpeed[3] = databyte[7];
                CoolantTemp[3] = databyte[11];
                EngRunTime[4] = databyte[15];
                EngRunTime[3] = databyte[14];
                Throttle[3] =  databyte[19];
                IAT[3] = databyte[23];
                MAF[4] = databyte[27];
                MAF[3] = databyte[26];
                MAP[3] = databyte[31];
                FuelTankLevel[3] = databyte[35];
                EngineSpeed[4] = databyte[39];
                EngineSpeed[3] = databyte[38];
                }
      }
            
          switch(msg){
           
           case 0://Temperatura do Liquido de Arrefecimento(°C)
                      CAN.beginPacket(0x7E8, 8);
                      CAN.write(CoolantTemp,sizeof(CoolantTemp));
                      CAN.endPacket();
                      msg=10;
                      break;
           case 1://Velocidade do Veiculo(km/h)
                      CAN.beginPacket(0x7E8, 8);
                      CAN.write(VehicleSpeed,sizeof(VehicleSpeed));
                      CAN.endPacket();
                      msg=10;
                      break;
           case 2://RPM DO MOTOR(RPM)
                      CAN.beginPacket(0x7E8, 8);
                      CAN.write(EngineSpeed,sizeof(EngineSpeed));
                      CAN.endPacket();
                      msg=10;
                      break;
           case 3://DISTANCIA PERCORRIDA(KM)
                      CAN.beginPacket(0x7E8, 8);
                      CAN.write(DistTravelled,sizeof(DistTravelled));
                      CAN.endPacket();
                      msg=10;
                      break;
           case 4://POSIÇÃO DO ACELERADOR(%)
                      CAN.beginPacket(0x7E8, 8);
                      CAN.write(Throttle,sizeof(Throttle));
                      CAN.endPacket();
                      msg=10;
                      break;
           case 5://TEMPERATURA DO AR DE ENTRADA(°C)
                      CAN.beginPacket(0x7E8, 8);
                      CAN.write(IAT,sizeof(IAT));
                      CAN.endPacket();
                      msg=10;                            
                      break;
           case 6://NIVEL DO COMBUSTIVEL(%)
                      CAN.beginPacket(0x7E8, 8);
                      CAN.write(FuelTankLevel,sizeof(FuelTankLevel));
                      CAN.endPacket();
                      msg=10;               
                      break;
            default:
                      break;
          }
}


void onReceive(int packetSize) {

      if(CAN.read()==0x02 && CAN.read()==0x01){
          
           switch(CAN.read()){
           
           case 0x05://Temperatura do Liquido de Arrefecimento(°C)
                      msg=0;
                      break;
           case 0x0D://Velocidade do Veiculo(km/h)
                      msg=1;
                      break;
           case 0x0C://RPM DO MOTOR(RPM)
                      msg=2;
                      break;
           case 0x31://DISTANCIA PERCORRIDA(KM)
                      msg=3;
                      break;  
           case 0x11://POSIÇÃO DO ACELERADOR(%)
                      msg=4;
                      break;
           case 0x0F://TEMPERATURA DO AR DE ENTRADA(°C)
                      msg=5;
                      break;
           case 0x2F://NIVEL DO COMBUSTIVEL(%)
                      msg=6;
                      break;
            default:
                      break;
          }
      }
}
%teste

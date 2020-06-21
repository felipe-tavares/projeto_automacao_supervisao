#include <Arduino.h>
#include <stdint.h>

/* Estrutura da mensagem */
#define READ_COIL   0X01
#define READ_INPUT  0X02
#define WRITE_COIL  0X05
#define READ_ANALOG 0x06
#define WRITE_ANALOG 0X07

/* lógica negada para saídas */
#define ON  0
#define OFF 1

/* Parametros */
#define SLAVE_ADR       "03"

/* mapeamento das saídas */
//enum {OUT1=8, OUT2, OUT3, OUT4} OUTPUTS;
#define OUTPUT_OFFSET 15
#define OUT1 16
#define OUT2 17
#define OUT3 18
#define OUT4 19

/* mapeamento das entradas */
#define INPUT_OFFSET 1
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

/* mapeamento das entradas analógicas */
#define ANALOG_INPUT_OFFSET -1
#define AIN1 A0
#define AIN2 A1
#define AIN3 A2
#define AIN4 A3

/* mapeamento da saída analógica */
#define ANALOG_OUTPUT_OFFSET 11
#define AON1 12

#define LM35  A10
#define TAM  100

extern float temperatura;

extern void Le_temperatura();

float temperatura;

void Le_temperatura()
{
  int i;
  float vet_temperatura[TAM], aux;

  aux = 0;
  for (i = 0; i < TAM; i++)
  {
    vet_temperatura[i] = (float(analogRead(LM35)) * 5 / (1023)) / 0.01;
    aux += vet_temperatura[i];
  }
  aux = (aux / TAM);
  temperatura = aux;
   Serial.println(aux);

}

void msgHandler();
//uint8_t calculaLRC (uint8_t *dado, uint8_t t);

void setup()
{
  Serial.begin(9600);
  Serial.println("Sistema Inciado...");

  // configura saídas
  pinMode(OUT1,OUTPUT);
  pinMode(OUT2,OUTPUT);
  pinMode(OUT3,OUTPUT);
  pinMode(OUT4,OUTPUT);

  // configura entradas
  pinMode(IN1,INPUT);
  pinMode(IN2,INPUT);
  pinMode(IN3,INPUT);
  pinMode(IN4,INPUT);

  // inicia com todas desligadas
  digitalWrite(OUT1, OFF);
  digitalWrite(OUT2, OFF);
  digitalWrite(OUT3, OFF);
  digitalWrite(OUT4, OFF);
  pinMode( AON1,OUTPUT);

Le_temperatura();
}

void loop()
{
  msgHandler();
}


void msgHandler()
{
if (Serial.available() > 0) 
   {
    while(Serial.available() > 0 )
    {
      int tamanho;
      // ler toda a mensagem até encontrar LF
      String msg = Serial.readStringUntil('\n');
      String aux;
      aux = msg;

      while(msg[tamanho] != '\0') {  //até acabar de ler msg
        ++tamanho;
      }

      if(tamanho == 13) tamanho-=1;
      else if(tamanho == 14) tamanho-=2;

      // início é valido?
      if(msg[0] == ':')
      {
        Serial.println("Caracter de inicio encontrado...");

   

        // mensagem é para mim?
        if(msg[1]==SLAVE_ADR[0] && msg[2]==SLAVE_ADR[1] )
        {
          Serial.println("Endereço de escravo correto...");

          // processa a mensagem
          int cmd = (msg[3]-'0')*10 + (msg[4]-'0');

          /*Serial.print("Comando: ");
          Serial.println(cmd);*/

          switch (cmd)
          {
            case READ_COIL:
              {
              int coil = ((msg[5]-'0')*10 + (msg[6]-'0')) + OUTPUT_OFFSET;
              // debug
              Serial.print("Leitura na Saida ");
              Serial.println(coil-OUTPUT_OFFSET);

              // Ligada ou desligada?
              if(!digitalRead(coil))
              {
                  msg[7]='F';
                  msg[8]='F';
                  msg[9]='0';
                  msg[10]='0';
              }
              else
              {
                msg[7]='0';
                msg[8]='0';
                msg[9]='0';
                msg[10]='0';
              }
              
              // Resposta com o valor atual da entrada..f
              Serial.print("Resposta do Escravo: ");
              Serial.println(msg);
              break;
              }
            case READ_INPUT:
              {
              // numero da entrada
              int contact = ((msg[5]-'0')*10 + (msg[6]-'0')) + INPUT_OFFSET;

              // debug
              Serial.print("Leitura na Entrada ");
              Serial.println(contact-INPUT_OFFSET);

              // Ligada ou desligada?
              if(digitalRead(contact))
              {
                  msg[7]='F';
                  msg[8]='F';
                  msg[9]='0';
                  msg[10]='0';
              }
              else
              {
                msg[7]='0';
                msg[8]='0';
                msg[9]='0';
                msg[10]='0';
              }
              
              // Resposta com o valor atual da entrada..
              Serial.print("Resposta do Escravo: ");
              Serial.println(msg);

              break;
              }
            // Escrever em uma saída
            case WRITE_COIL:
            {            
              // string para inteiro aplicando offset, saida 1 no Arduino está mapeada para o pino 8, saída 2 para o pino 9 ....
              int coil = ((msg[5]-'0')*10 + (msg[6]-'0')) + OUTPUT_OFFSET;

              // padrao é desligar
              int value = OFF;

              // debug
              Serial.print("Escrita na Saida ");
              Serial.println(coil-OUTPUT_OFFSET);

              // ligar ou desligar?
              if(msg[7]=='F' && msg[8]=='F' && msg[9]=='0' && msg[10]=='0')
              {
                value = ON;
                Serial.println(" -> Mudar para Ligado");
              }
              else if(msg[7]=='0' && msg[8]=='0' && msg[9]=='0' && msg[10]=='0')
              {
                value = OFF;
                Serial.println(" Mudar para Desligado");
              }
              // executao comando
              digitalWrite(coil, value);
              
              // Para esse caso, a resposta é um simples echo da mensagem original
              Serial.print("Resposta do Escravo: ");
              Serial.println(msg);
            //default:
              break;
            }

            case READ_ANALOG:
            { 
              // decodifica entrada a ser lida
              int ain = ((msg[5]-'0')*10 + (msg[6]-'0')) + ANALOG_INPUT_OFFSET;

              // debug
              Serial.print("Leitura na Entrada Analogica ");
              Serial.println(ain+1);

              // executao comando
              uint16_t value = analogRead(ain);
              
              // int para string
              char buf[5];
              sprintf(buf,"%04d", value);

              // monta valor de retonro
              msg[7]=buf[0];
              msg[8]=buf[1];
              msg[9]=buf[2];
              msg[10]=buf[3];
              
              // Responde para o mestre
              Serial.print("Resposta do Escravo: ");
              Serial.println(msg);
            //default:
              break;
            }

            case WRITE_ANALOG:
            {
              // decodifica entrada a ser lida
              int aon = ((msg[5]-'0')*10 + (msg[6]-'0')) + ANALOG_OUTPUT_OFFSET;
              // de string para inteito
              int value = (msg[7]-'0')*1000 + (msg[8]-'0')*100 + (msg[9]-'0')*10 +(msg[10]-'0');

              // debug
              Serial.print("Escrita na Saida Analogica ");
              Serial.println(aon-ANALOG_OUTPUT_OFFSET);
              Serial.print("Valor: ");
              Serial.println(value);

              // escreve na saída
              analogWrite(aon, value);

              // Para esse caso, a resposta é um simples echo da mensagem original
              Serial.print("Resposta do Escravo: ");
              Serial.println(msg);

              break;
            }
          }

        }
        else
        {
          Serial.println("Mensagem para outro escravo, Aleluia!!!..."); 
        }
      } 
      else
      {
        Serial.println("Formato de mensagem desconhecido...");    
      }
    }
  }
}

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
#define OUT0 8
#define OUT1 16
#define OUT2 17 //ligar solenoide
#define OUT3 18 // ligar sirene
#define OUT4 19 //aquecedor ligar
#define OUT5 20 // motorAdir
#define OUT6 21// motorAesq
#define OUT7 22// motorBdir
#define OUT8 23// motorBesq

/* mapeamento das entradas */
#define INPUT_OFFSET 1
#define IN1 2
#define IN2 3
#define IN3 4 //alerta vento
#define IN4 5   //sensor porta

/* mapeamento das entradas analógicas */
#define ANALOG_INPUT_OFFSET -1
#define AIN1 A0
#define AIN2 A1
#define AIN3 A2 //temperatura
#define AIN4 A3//anemometro
#define AIN5 A4//alerta vento
#define AIN6 A5
#define AIN7 A6
#define AIN8 A8



/* mapeamento da saída analógica */
#define ANALOG_OUTPUT_OFFSET 8
#define AON1 9
#define AON2 10
#define AON3 11
#define AON4 12
#define AON5 13
#define AON6 14
#define AON7 15



#define TAM  5

int tempac = 0;
int banda = 17;
int manualac = 0;
int retorno = 0;
int flag = 0;
int vento = 0;

int cortinaquarto = 0;
int cortinasala = 0;
int histerese =0;
extern float temperatura;
 float temperatura2;

extern void Le_temperatura();

float temperatura;

int prim=0;






int funcaoTeste(int value, int sensor, int bandeira)
{
  int mot, hora[2] = {OUT5, OUT7}, anti[2] = {OUT6, OUT8};
  if (bandeira == 1) {
    mot = 0;
  }

  else {
    mot = 1;
  }

  int lado = (analogRead(sensor));
  if (lado > 819)  lado = 819;
  else if (lado < 205)   lado = 205;


  /** ?
    ! 4V = 818.4
    ! 1V = 204.6
  */

  value = map(value, 0, 1023, 205, 819);

 //Serial.println(bandeira);
  Serial.println(mot);
  Serial.println("mot");

  Serial.println(value);
   Serial.println("value");
  
  Serial.println(lado);
 Serial.println("lado");

  if (value > lado)
  {
    digitalWrite(hora[mot],  ON);
    digitalWrite(anti[mot], OFF);

  }

  else if (value < lado)
  {
    digitalWrite(anti[mot],  ON);
    digitalWrite(hora[mot], OFF);

  }
  else
  {
    digitalWrite(hora[mot], OFF);
    digitalWrite(anti[mot], OFF);
  }
  return lado;
}
void Le_temperatura()
{
  int i;
  float vet_temperatura[TAM],vet_temperatura2[TAM], aux,aux2 ;

  aux = 0;
   aux2 = 0;
  for (i = 0; i < TAM; i++)
  {
    vet_temperatura[i] = (float(analogRead(AIN4)) * 5 / (1023)) / 0.01;
    aux += vet_temperatura[i];
  }

  for (i = 0; i < TAM; i++)
  {
    vet_temperatura2[i] = (float(analogRead(AIN3)) * 5 / (1023)) / 0.01;
    aux2 += vet_temperatura2[i];
  }
  
  aux = (aux / TAM);
  temperatura = aux;
  aux2 = (aux2 / TAM);
  temperatura2 = aux2;


}


void msgHandler();
//uint8_t calculaLRC (uint8_t *dado, uint8_t t);

void setup()
{
  Serial.begin(9600);
  Serial.println("Sistema Inciado...");

  // configura saídas
  pinMode(OUT0, OUTPUT);
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);
  pinMode(OUT5, OUTPUT);
  pinMode(OUT6, OUTPUT);
  pinMode(OUT7, OUTPUT);
  pinMode(OUT8, OUTPUT);

  // configura entradas
  pinMode(IN1, INPUT);
  pinMode(IN2, INPUT);
  pinMode(IN3, INPUT);
  pinMode(IN4, INPUT);


  // inicia com todas desligadas
  digitalWrite(OUT1, OFF);
  digitalWrite(OUT2, OFF);
  digitalWrite(OUT3, OFF);
  digitalWrite(OUT4, OFF);
  digitalWrite(OUT5, OFF);
  digitalWrite(OUT6, OFF);
  digitalWrite(OUT7, OFF);
  digitalWrite(OUT8, OFF);




  pinMode( AON1, OUTPUT);
  pinMode( AON2, OUTPUT);
  pinMode( AON3, OUTPUT);
  pinMode( AON4, OUTPUT);
  pinMode( AON5, OUTPUT);
  pinMode( AON6, OUTPUT);
    pinMode( AON7, OUTPUT);


  pinMode(  AIN1, INPUT);
  pinMode( AIN2, INPUT);
  pinMode( AIN3, INPUT);
  pinMode( AIN4, INPUT);
  pinMode( AIN5, INPUT);
  pinMode( AIN6, INPUT);
  pinMode( AIN7, INPUT);
  pinMode( AIN8, INPUT);
  

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
    while (Serial.available() > 0 )
    {
      int tamanho;
      // ler toda a mensagem até encontrar LF
      String msg = Serial.readStringUntil('\n');
      String aux;
      aux = msg;

      while (msg[tamanho] != '\0') { //até acabar de ler msg
        ++tamanho;
      }

      if (tamanho == 13) tamanho -= 1;
      else if (tamanho == 14) tamanho -= 2;

      // início é valido?
      if (msg[0] == ':')
      {
        Serial.println("Caracter de inicio encontrado...");



        // mensagem é para mim?
        if (msg[1] == SLAVE_ADR[0] && msg[2] == SLAVE_ADR[1] )
        {
          Serial.println("Endereço de escravo correto...");

          // processa a mensagem
          int cmd = (msg[3] - '0') * 10 + (msg[4] - '0');

          /*Serial.print("Comando: ");
            Serial.println(cmd);*/ 
            
                  
                  Le_temperatura();
                 

                  if (temperatura > (banda + histerese))
                  {
                    
                     digitalWrite(OUT4, OFF);
                   
                    
                  }


                  if (temperatura < (banda - histerese))
                  
                  {
                    
                  
                    digitalWrite(OUT4, ON);
                    
                  }
                if (prim==1){
                  flag=1;

                  funcaoTeste(cortinaquarto, AIN7, flag);

                  flag=2;
                  funcaoTeste(cortinasala, AIN8, flag);
          
                 

                }

                 

          switch (cmd)
          {
            case READ_COIL:
              {
                int coil = ((msg[5] - '0') * 10 + (msg[6] - '0')) + OUTPUT_OFFSET;
                // debug
                Serial.print("Leitura na Saida ");
                Serial.println(coil - OUTPUT_OFFSET);

                // Ligada ou desligada?

                if (!digitalRead(coil))
                {
                  msg[7] = 'F';
                  msg[8] = 'F';
                  msg[9] = '0';
                  msg[10] = '0';
                }
                else
                {
                  msg[7] = '0';
                  msg[8] = '0';
                  msg[9] = '0';
                  msg[10] = '0';
                }

                // Resposta com o valor atual da entrada..f
                Serial.print("Resposta do Escravo: ");
                Serial.println(msg);

                break;
              }
            case READ_INPUT:
              {
                // numero da entrada
                int contact = ((msg[5] - '0') * 10 + (msg[6] - '0')) + INPUT_OFFSET;

                // debug
                Serial.print("Leitura na Entrada ");
                Serial.println(contact - INPUT_OFFSET);

                // Ligada ou desligada?
                if (digitalRead(contact))
                {
                  msg[7] = 'F';
                  msg[8] = 'F';
                  msg[9] = '0';
                  msg[10] = '0';
                }
                else
                {
                  msg[7] = '0';
                  msg[8] = '0';
                  msg[9] = '0';
                  msg[10] = '0';
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
                int coil = ((msg[5] - '0') * 10 + (msg[6] - '0')) + OUTPUT_OFFSET;

                // padrao é desligar
                int value = OFF;

                // debug
                Serial.print("Escrita na Saida ");
                Serial.println(coil - OUTPUT_OFFSET);

                // ligar ou desligar?
                if (msg[7] == 'F' && msg[8] == 'F' && msg[9] == '0' && msg[10] == '0')
                {
                  value = ON;
                  Serial.println(" -> Mudar para Ligado");
                }
                else if (msg[7] == '0' && msg[8] == '0' && msg[9] == '0' && msg[10] == '0')
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
                int ain = ((msg[5] - '0') * 10 + (msg[6] - '0')) + ANALOG_INPUT_OFFSET;

                // debug
                Serial.print("Leitura na Entrada Analogica ");
                Serial.println(ain + 1);

                // executao comando
                if (ain == 3) {
                  Le_temperatura(); // funcao da temperaturaaaaa

                  uint16_t value = temperatura;

                  // int para string
                  char buf[5];
                  sprintf(buf, "%04d", value);

                  // monta valor de retonro
                  msg[7] = buf[0];
                  msg[8] = buf[1];
                  msg[9] = buf[2];
                  msg[10] = buf[3];

                  // Responde para o mestre
                  Serial.print("Resposta do Escravo: ");
                  Serial.println(msg);
                  //default:

                }
                else if (ain == 4) { // VENTOOOOOO
                  uint16_t  value = analogRead(ain);
                  Serial.println(value);
                  value  = map(value, 100, 924, 5, 150);
                  vento = value;

                  if (value > 50) {

                    vento = 1;
                  }
                  else {
                    vento = 0;
                  }




                  // int para string
                  char buf[5];
                  sprintf(buf, "%04d", value);

                  // monta valor de retonro
                  msg[7] = buf[0];
                  msg[8] = buf[1];
                  msg[9] = buf[2];
                  msg[10] = buf[3];

                  // Responde para o mestre
                  Serial.print("Resposta do Escravo: ");
                  Serial.println(msg);
                  //default:

                }
                else if (ain == 5) { // VENTOOOOOO alerta



                  if (vento == 1)
                  {
                    msg[7] = 'F';
                    msg[8] = 'F';
                    msg[9] = '0';
                    msg[10] = '0';
                  }
                  else
                  {
                    msg[7] = '0';
                    msg[8] = '0';
                    msg[9] = '0';
                    msg[10] = '0';
                  }



                  // Responde para o mestre
                  Serial.print("Resposta do Escravo: ");
                  Serial.println(msg);
                  //default:

                }


                else {
                  uint16_t value = analogRead(ain);

                  // int para string
                  char buf[5];
                  sprintf(buf, "%04d", value);

                  // monta valor de retonro
                  msg[7] = buf[0];
                  msg[8] = buf[1];
                  msg[9] = buf[2];
                  msg[10] = buf[3];

                  // Responde para o mestre
                  Serial.print("Resposta do Escravo: ");
                  Serial.println(msg);
                  //default:

                }
                break;
              }






            case WRITE_ANALOG:
              {
                // decodifica entrada a ser lida
                int aon = ((msg[5] - '0') * 10 + (msg[6] - '0')) + ANALOG_OUTPUT_OFFSET;
                // de string para inteito
                int value = (msg[7] - '0') * 1000 + (msg[8] - '0') * 100 + (msg[9] - '0') * 10 + (msg[10] - '0');

                // debug
                Serial.print("Escrita na Saida Analogica ");
                Serial.println(aon - ANALOG_OUTPUT_OFFSET);
                Serial.print("Valor: ");

                 if (aon - ANALOG_OUTPUT_OFFSET == 3) {
                
                  Le_temperatura();
                  
                   if (temperatura2 > value)
                  {
                   Serial.println("teste entrou maior");
                     digitalWrite(OUT1, OFF);   //ligar e desligar AR CONDICIONADO
                    
                  }


                  if (temperatura2 <  value)
                  
                  {
                
                      Serial.println("teste entrou menor");
                    digitalWrite(OUT1, OFF);
                  }

  

               

                }

                if (aon - ANALOG_OUTPUT_OFFSET == 3) {
                  manualac = value;
                 


                  value = (51 * value) / 10;

                  Serial.println(manualac);

                }
                if (aon - ANALOG_OUTPUT_OFFSET == 4) {

                  Le_temperatura();
                  histerese=value;
                  if (temperatura > (banda + value))
                  {
                 
                     digitalWrite(OUT4, OFF);   //ligar e desligar aquecedorrrr
                    
                  }


                  if (temperatura < (banda - value))
                  
                  {
                
                  
                    digitalWrite(OUT4, ON);
                  }
                  

                }
                if (aon - ANALOG_OUTPUT_OFFSET == 5) {
                  tempac = value;
                  
                }
                if (aon - ANALOG_OUTPUT_OFFSET == 6) {
                  cortinaquarto = value;
                  flag = 1;
                  retorno = funcaoTeste(cortinaquarto, AIN7, flag);
                  prim=1;
                  

                }
                if (aon - ANALOG_OUTPUT_OFFSET == 7) {
                  cortinasala = value;
                  flag = 2;
                  retorno = funcaoTeste(cortinasala, AIN8, flag);

                  prim=1;

                }




                // escreve na saída
                analogWrite(aon, value);
                Serial.println(value);
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

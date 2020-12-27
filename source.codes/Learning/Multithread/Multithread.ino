int count  = 0;
int blinked = 0;

String statusButton = "DESATIVADO";

//pinos usados
const uint8_t pin_led = 2;
const uint8_t pin_btn = 4;

//variaveis que indicam o núcleo
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne  = 1;

void setup() {
  pinMode(pin_led, OUTPUT);
  pinMode(pin_btn, INPUT);

  //inicializa o LCD com os pinos SDA e SCL
  Serial.begin(115200);
  delay(500);


  //cria uma tarefa que será executada na função coreTaskZero, com prioridade 1 e execução no núcleo 0
  //coreTaskZero: piscar LED e contar quantas vezes
  xTaskCreatePinnedToCore(
                    coreTaskZero,   /* função que implementa a tarefa */
                    "coreTaskZero", /* nome da tarefa */
                    10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                    NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                    1,          /* prioridade da tarefa (0 a N) */
                    NULL,       /* referência para a tarefa (pode ser NULL) */
                    taskCoreZero);         /* Núcleo que executará a tarefa */
                    
  delay(500); //tempo para a tarefa iniciar

  //cria uma tarefa que será executada na função coreTaskOne, com prioridade 2 e execução no núcleo 1
  //coreTaskOne: atualizar as informações do display
  xTaskCreatePinnedToCore(
                    coreTaskOne,   /* função que implementa a tarefa */
                    "coreTaskOne", /* nome da tarefa */
                    10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                    NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                    2,          /* prioridade da tarefa (0 a N) */
                    NULL,       /* referência para a tarefa (pode ser NULL) */
                    taskCoreOne);         /* Núcleo que executará a tarefa */

    delay(500); //tempo para a tarefa iniciar

   //cria uma tarefa que será executada na função coreTaskTwo, com prioridade 2 e execução no núcleo 0
   //coreTaskTwo: vigiar o botão para detectar quando pressioná-lo
   xTaskCreatePinnedToCore(
                    coreTaskTwo,   /* função que implementa a tarefa */
                    "coreTaskTwo", /* nome da tarefa */
                    10000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                    NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                    2,          /* prioridade da tarefa (0 a N) */
                    NULL,       /* referência para a tarefa (pode ser NULL) */
                    taskCoreZero);         /* Núcleo que executará a tarefa */
   
}

void loop() {
}

//essa função ficará mudando o estado do led a cada 1 segundo
//e a cada piscada (ciclo acender e apagar) incrementará nossa variável blinked
void coreTaskZero( void * pvParameters ){
 
    String taskMessage = "Task running on core ";
    taskMessage = taskMessage + xPortGetCoreID();
 
    while(true){
      digitalWrite(pin_led, !digitalRead(pin_led));
      if (++count % 2 == 0 )
        blinked++;

      delay(1000);
    } 
}

//essa função será responsável apenas por atualizar as informações no 
//display a cada 100ms
void coreTaskOne( void * pvParameters ){
     while(true){
        Serial.print("LED: ");
        Serial.println(blinked);
        Serial.print("Botão: ");
        Serial.println(statusButton);
        delay(100);
    } 
}

//essa função será responsável por ler o estado do botão
//e atualizar a variavel de controle.
void coreTaskTwo( void * pvParameters ){
     while(true){
      if(digitalRead(pin_btn)){
        statusButton = "Ativado   ";  
      }
      else statusButton = "Desativado";
      delay(10);
    } 
}

// C++ code
#include <SoftwareSerial.h>
#include <stdint.h>
#define imprimir(a) Serial.println(a); 
#define imprimirj(a) Serial.print(a);

SoftwareSerial arduino2Serial(3, 2); // RX, TX del puerto virtual
char message;
int count=0;
bool control=false;
bool controlInterfaz=0;
bool impInterfaz=1;
int option=0;
#define HEADER 0x03         //Bytes fijos de la trama, es decir no cuentan como payload
#define RX_16BITS 0X81       // Trama de recepcion de 16bits
//uint8_t frame_rx[61]={};

class factory_frame{

    public:
    // Trama de recepcion de 16bits que simula una trama real
        uint8_t frame_rx [61] = {};
    //Estructura de datos para almacenar la informacion que retorna los metodos
            struct 
            {
                uint16_t MAC = 0;       //Se almacena la direccion MAC de 16 bits, ejemplos: 0x1111
                uint8_t LENGHT = 0;     //Longitud total del mensaje
                uint8_t FRAME_TYPE = 0; //Tipo de mensaje
                uint8_t RSSI = 0;       //RSSI Received Signal Strength Indicator, indica potencia de recepcion
                String PAYLOAD = "";    //Mensaje comunicatica de importancia
                bool CHECKSUM = false;  //Indica si la trama esta completa es decirm que no esta corrupta
            }data;                       //Instancia u objeto de la estructura
            

        public: 
        uint16_t GET_MAC (void);        //funcion que retorna la direccion mac de 16 bits
        uint8_t GET_LENGHT (void);      //funcion que retorna la longitud de la trama
        uint8_t FRAME_TYPE (void);      //funcion que retorna el tipo de trama
        uint8_t GET_RSSI (void);        //funcion que retorna la calidad de la señal del mensaje
        String GET_PAYLOAD (void);      //funcion que retorna el mensaje de la trama (stringe)
        bool GET_CHECKSUM (void);       //funcion que retorna esta corrupta o no (booleana)




};

factory_frame Frame;

void setup() {
  Serial.begin(9600);
  arduino2Serial.begin(2400);
}

void loop() {
	while (arduino2Serial.available() > 0) {
    message = arduino2Serial.parseInt();
    Frame.frame_rx[count]= message;
    //Serial.print((char)message);  
    count++;
    //Serial.print(count);
    control=true;   
  }	  
  //delay(200);
  count=0;
  imprimirInterfaz();
  interfaz();
  control=false;
}

void imprimirInterfaz(){
  if(impInterfaz){
    	imprimir(F("Ha recibido un paquete, ¿Que desea hacer con el?:"));
      	imprimir(F("1.- Mostrar direccion mac"));
      	imprimir(F("2.- Mostrar la longitud del mensaje"));
      	imprimir(F("3.- Mostrar el tipo de trama"));
      	imprimir(F("4.- Mostrar intesidad de la senal del mensaje de recepcion"));
      	imprimir(F("5.- Mostrar mensaje oculto:"));
      	imprimir(F("6.- Mostrar si la trama esta corrupta"));
  		impInterfaz=0;
  }
}

void interfaz(){
    if(control){
      //imprimir(F("Se recibio un nuevo mensaje"));
      while(controlInterfaz==0){
        if(Serial.available()){
        	option = Serial.parseInt();
          	controlInterfaz=1;
        }
      }
      
      switch (option){   
        
        case 1:
            Frame.data.MAC = Frame.GET_MAC();
            imprimirj(F("La direccion Mac es: "));
        	Serial.println(Frame.data.MAC, HEX);
        	option=0;
        	controlInterfaz=0;
            break;
        
        case 2:
            Frame.data.LENGHT = Frame.GET_LENGHT();
            imprimirj(F("La longitud del mensaje es de: "));
        	imprimirj(Frame.data.LENGHT);
        	imprimir(F(" bytes"));
        	option=0;
        	controlInterfaz=0;
            break;
        
        case 3: 
            Frame.data.FRAME_TYPE = Frame.FRAME_TYPE();
        	if(Frame.FRAME_TYPE()== RX_16BITS){
            	imprimir(F("La trama es un mensaje de recepcion de 16 bits"));
            }
        	option=0;
        	controlInterfaz=0;
            break;
        
        case 4:
            Frame.data.RSSI = Frame.GET_RSSI();
        	imprimirj(F("La intesidad de la señal con la que llega el mensaje fue: "));
        	imprimirj(F("-"));
        	imprimirj(Frame.data.RSSI);
        	imprimir(F(" dbm"));
        	option=0;
        	controlInterfaz=0;
        break;
        
        case 5:
            Frame.data.PAYLOAD = Frame.GET_PAYLOAD();
        	imprimirj(F("El mensaje oculto es: "));
            imprimir(Frame.data.PAYLOAD);
        	option=0;
        	controlInterfaz=0;
        break;
        
        case 6:
        Frame.data.CHECKSUM = Frame.GET_CHECKSUM();
        if (!Frame.GET_CHECKSUM())
        {
          imprimir(F("La trama esta corrupta"));
        }else{
          imprimir(F("La trama no esta corrupta"));
        }
        option=0;
        controlInterfaz=0;
        break;

        default:
        	//imprimir("Selecciona una opcion valida");
        	option=0;
        	controlInterfaz=0;
            break;
        }
      
  }
}

        uint16_t factory_frame::GET_MAC(){

            return (frame_rx[4]*256 + frame_rx[5]);
        };
        uint8_t factory_frame::GET_LENGHT (void){
            return (frame_rx[1]*256+frame_rx[2]);
        };
        uint8_t factory_frame::FRAME_TYPE (void){
            return frame_rx[3];
        };
        uint8_t factory_frame::GET_RSSI (void){
            return (frame_rx[6]);
        };

        String factory_frame::GET_PAYLOAD (void){

            String temp_frame="";

            for (uint16_t i = 8; i <GET_LENGHT()+HEADER; i++){
            temp_frame+= (char)frame_rx[i];
            }
            return temp_frame;
        };


        bool factory_frame::GET_CHECKSUM (void){
            uint8_t suma = 0;
            
            for (uint8_t i = HEADER; i <GET_LENGHT()+HEADER; i++)
            suma += frame_rx[i];

            if ((0xff-suma) == frame_rx[GET_LENGHT()+HEADER])
            {
                return true;
            }else{
                return false;
            }

        };

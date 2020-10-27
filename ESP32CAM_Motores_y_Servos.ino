/*     Basado/Copiado/Ampliado(poco) desde este proyecto
 *      
 *      https://github.com/tomasmacek/ESP32CAM_RCTANK
 *      
 *     Que a su vez es una modificacion de este otro
 *     
 *     https://github.com/PepeTheFroggie/ESP32CAM_RCTANK
 *     
 *     
 * Mis añadidos son pocos pero importantes:
 * 
 *    1º IP Fija, tal como lo he dejado es imprescidible.
 *    
 *    2º Para añadir un segundo servo (y de paso un tercero) uso uso los pines del puerto serie GPIO1 y 3
 *         Por lo que he comentado todas las lineas que sacaban informacion al puerto serie.
 *         
 *    3º Mientras probaba las distintas salidas descubri que con el GPIO16 podia mover un SG90, asi que ye tengo un cuarto
 *          Este GPIO produce un retardo notable, tanto al "arrancar" (sobre todo) como durante la ejecucion.
 *          Y solo funciona con los SG90, creo que tiene que ver con que mis MG90 son de rotacion continua y al inicio solo dan vueltas y vueltas.
 *          
 *          Es facil de "eliminar" Busca AZUL y comenta las lineas, y si te vale solo con dos servos, pues quita SERVO3 tambien.
 *    
 *    4º Para intentar cambiar la interface he "tabulado" el archivo app_httpd.cpp tanto la parte HTML como la del SCRIPT 
 *       La parte del Script no estoy seguro que este bien tabulada, es demasiado compleja para mi.
 *        
 *        
 * Mi idea de cambiar el interface de manejo era sustitui los botones por un "Joystick" he buscado bastantes, pero ya no se seguir...
 * Aqui van unos posibles por si alguno sabe y se anima
 * 
 *         https://github.com/bobboteck/JoyStick      El que mas me gusta, se podrian poner dos, uno para los motores y otro para PAN/TILT de los servos
 *         
 *         https://www.kirupa.com/html5/drag.htm      Otra opcion, que parece mas facil, pero no se si permitiria poner dos.
 *         
 *         https://www.esploradores.com/practica-12-bis-joystick-wifi-mejorado-para-el-control-de-robots/   Y otra mas, esta española.
 *         
 *         https://www.instructables.com/Making-a-Joystick-With-HTML-pure-JavaScript/         Una utima, pero seguro que hay mas.
 *   
 *  
 *          
 *         GPIO 0 Carga pero no hace stream 
 *         GPIO 1 (SERIALES COMENTADOS) SI   
 *         GPIO 3 (SERIALES COMENTADOS) SI 
 *         GPIO116 PARA SERVO SG90 (AZULES) FUNCIONA PARA SERVOS MG90 (MARRONES) NO 
 *          
ESP32-CAM Remote Control 
*/
// Compilation&Instalation notes
// Make sure that you have either selected ESP32 Wrover Module or another board which has PSRAM enabled ( in Arduino IDE)
// Rename tempate_secrates.h. to secrates.h and fill in your MY_SSID and MY_PASSWORD of your wifi
// If you want to be able to refer to your ESP32CAM by symbolic name , register at https://www.duckdns.org service and select name of the service
//  fill in API key to MY_DUCKDNS_TOKEN and name to MY_DUCKDNS_NAME in secrates.h
// LED blinks in two bursts where count of blinks in burst indicates a state
//   first burst: 1 blink= conected to wifi, 2 blinks - switched to soft access point mode (user can still connect to this softAP)

#include "secrets.h"
#include "esp_wifi.h"
#include "esp_camera.h"
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <HTTPClient.h>


// Set your Static IP address
IPAddress local_IP(192, 168, 1, 252);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_M5STACK_PSRAM
#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_WROVER_KIT)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27

#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       32
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#else
#error "Camera model not selected"
#endif

const char* ssid = MY_SSID;
const char* password = MY_PASSWORD;

void startCameraServer();

const int MotPin0 = 12;  
const int MotPin1 = 13;  
const int MotPin2 = 14;  
const int MotPin3 = 15;  

void initMotors() 
{
  ledcSetup(3, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcSetup(4, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcSetup(5, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcSetup(6, 2000, 8); // 2000 hz PWM, 8-bit resolution
  ledcAttachPin(MotPin0, 3); 
  ledcAttachPin(MotPin1, 4); 
  ledcAttachPin(MotPin2, 5); 
  ledcAttachPin(MotPin3, 6); 
}

const int ServoTILT = 2; 
const int ServoPAN = 1; 
const int Servo3   = 3;
const int ServoAZUL   = 16;                                                             //AZUL

void initServo() 
{
  ledcSetup(8, 50, 16); // 50 hz PWM, 16-bit resolution, range from 3250 to 6500.
  ledcAttachPin(ServoTILT, 8); 
  ledcSetup(9, 50, 16); // 50 hz PWM, 16-bit resolution, range from 3250 to 6500.
  ledcAttachPin(ServoPAN, 9);
  ledcSetup(10, 50, 16); // 50 hz PWM, 16-bit resolution, range from 3250 to 6500.
  ledcAttachPin(Servo3, 10);
  ledcSetup(11, 50, 16); // 50 hz PWM, 16-bit resolution, range from 3250 to 6500.      //AZUL
  ledcAttachPin(ServoAZUL, 11);                                                         //AZUL
}

void Blink( int n ){
  for (int i = 0 ; i < n ; i++) 
  {
    ledcWrite(7,10);  // flash led
    delay(100);
    ledcWrite(7,0);
    delay(100);    
  }   
}

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // prevent brownouts by silencing them
  
  //Serial.begin(115200);
  //Serial.setDebugOutput(true);
  //Serial.println();

    // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    //Serial.println("STA Failed to configure");
  }
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    //Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  s->set_vflip(s, 0);
  s->set_hmirror(s, 1);

  // Remote Control Car
  initMotors();
  initServo();
  
  ledcSetup(7, 5000, 8);
  ledcAttachPin(4, 7);  //pin4 is LED
  
  //Serial.println("ssid: " + (String)ssid);
  //Serial.println("password: " + (String)password);
  
  WiFi.begin(ssid, password);
  delay(500);

  long int StartTime=millis();
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      if ((StartTime+10000) < millis()) break;
  } 

  /*
  int8_t power;
  esp_wifi_set_max_tx_power(20);
  esp_wifi_get_max_tx_power(&power);
  Serial.printf("wifi power: %d \n",power); 
  */
  
  startCameraServer();

  if (WiFi.status() == WL_CONNECTED) 
  {
    //Serial.println("");
    //Serial.println("WiFi connected");    
    //Serial.print("Use 'http://");
    //Serial.print(WiFi.localIP());
    //Serial.println("' to connect");
    Blink( 1 );

  } else {
    //Serial.println("");
    //Serial.println("WiFi disconnected. Creating own access point. Use SSID:ESP32-CAM and PASSWORD:12345678 to connect");      
    //Serial.print(WiFi.softAPIP());
    //Serial.println("' to connect");
    char* apssid = "ESP32-CAM";
    char* appassword = "12345678";         //AP password require at least 8 characters.
    WiFi.softAP((WiFi.softAPIP().toString()+"_"+(String)apssid).c_str(), appassword);
    Blink( 2 );
  }

  delay(1000);

#ifdef MY_DUCKDNS_TOKEN
  // Send IP to DuckDNS
  HTTPClient http;
  String sIP;
  sIP=WiFi.localIP().toString();
  String sURI= String("https://www.duckdns.org/update?domains=") + String(MY_DUCKDNS_NAME) + String("&token=") + String(MY_DUCKDNS_TOKEN) + String("&ip=") + sIP ;
  //Serial.println(sURI);
  http.begin(sURI);
  int httpCode = http.GET();  
  if (httpCode > 0) { //Check for the returning code
    String payload = http.getString();
    //Serial.println(httpCode);
    //Serial.println(payload);
    Blink( 1 );
  }else {
    //Serial.println("Error on HTTP request");
    Blink( 3 );
  } 
  http.end(); //Free the resources       
#else
  //Serial.println("Not using DuckDNS.");
  Blink( 2 );
#endif
}

void loop() {
  int count=0;
  delay(100);
  if ( count > 100 ){
    //Serial.printf("RSSi: %ld dBm\n",WiFi.RSSI());
    count=0;
  } 
  count++;
}

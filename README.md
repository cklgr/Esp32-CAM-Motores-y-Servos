# Esp32-CAM-Motores-y-Servos


     Basado/Copiado/Ampliado(poco) desde este proyecto
      
      https://github.com/tomasmacek/ESP32CAM_RCTANK
       
      Que a su vez es una modificacion de este otro
      
      https://github.com/PepeTheFroggie/ESP32CAM_RCTANK
      
      
  Mis añadidos son pocos pero importantes:
  
     1º IP Fija, tal como lo he dejado es imprescidible.
     
     2º Para añadir un segundo servo (y de paso un tercero) uso uso los pines del puerto serie GPIO1 y 3
          Por lo que he comentado todas las lineas que sacaban informacion al puerto serie.
          
     3º Mientras probaba las distintas salidas descubri que con el GPIO16 podia mover un SG90, asi que ye tengo un cuarto
           Este GPIO produce un retardo notable, tanto al "arrancar" (sobre todo) como durante la ejecucion.
           Y solo funciona con los SG90, creo que tiene que ver con que mis MG90 son de rotacion continua y al inicio solo dan vueltas y vueltas.
          
           Es facil de "eliminar" Busca AZUL y comenta las lineas, y si te vale solo con dos servos, pues quita SERVO3 tambien.
     
     4º Para intentar cambiar la interface he "tabulado" el archivo app_httpd.cpp tanto la parte HTML como la del SCRIPT 
        La parte del Script no estoy seguro que este bien tabulada, es demasiado compleja para mi.
         
         
  Mi idea de cambiar el interface de manejo era sustituir los botones por un "Joystick" he buscado bastantes, pero ya no se seguir...
  Aqui van unos posibles por si alguno sabe y se anima
  
          https://github.com/bobboteck/JoyStick      El que mas me gusta, se podrian poner dos, uno para los motores y otro para PAN/TILT de los servos
          
          https://www.kirupa.com/html5/drag.htm      Otra opcion, que parece mas facil, pero no se si permitiria poner dos.
          
          https://www.esploradores.com/practica-12-bis-joystick-wifi-mejorado-para-el-control-de-robots/   Y otra mas, esta española.
          
          https://www.instructables.com/Making-a-Joystick-With-HTML-pure-JavaScript/         Una utima, pero seguro que hay mas.
    
   
           
          GPIO 0 Carga pero no hace stream 
          GPIO 1 (SERIALES COMENTADOS) SI   
          GPIO 3 (SERIALES COMENTADOS) SI 
          GPIO116 PARA SERVO SG90 (AZULES) FUNCIONA PARA SERVOS MG90 (MARRONES) NO 
           


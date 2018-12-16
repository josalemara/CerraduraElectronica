#include <Keypad.h> // Controla el teclado
#include <LiquidCrystal_I2C.h>
#include <Servo.h> //Control del servomotor
#include <EEPROM.h> //COntrola E/S EEPROM 

Servo seguro; //servomotor 
const byte filas=4;
const byte columnas=4;
char teclas[filas][columnas]={
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinsFilas[filas]={5,4,3,2};
byte pinsColumnas[columnas]={9,8,7,6};
Keypad teclado = Keypad(makeKeymap(teclas), pinsFilas,pinsColumnas, filas,columnas);
LiquidCrystal_I2C lcd(0x27, 16, 2); 


char co[5] = {'1','2','3','4'}; //almacena la contraseña en eeprom
char cl[5]; //almacena la contraseña nueva
char cn[5]; //confirmacion de la contraseña
char ca[7]= {'3','7','1','9'}; //clave admin establecida
char cal[7]; // Clave admin leida
int contador = 0; //Lleva la posicion del array
int cursorr = 6; //Lleva la posicion del cursor
int comp; // comparacion entre 2 arrays
int buzzer=11;
int a; //aux 
int conta=0;


void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  // put your setup code here, to run once:
   pinMode(12, OUTPUT); // LEDS QUE INDICAN ABIERTO(6) O CERRADO(10)
   pinMode(13,OUTPUT);
   digitalWrite(13, HIGH); //enciende el led de cerrado
   pinMode (buzzer, OUTPUT);
   Serial.begin(9600); 
  /* for(int i =0;i<=3;i++){
    //LEER CONTRASEÑA DE LA EEPROM
    co[i]= EEPROM.read(i); //almacenarla en co[]
 }*/
 seguro.attach(10); // Pin del servomotor
 lcd.setCursor(6,1); // La escritura de numero siempre sera en el sgundo renglon a partir dle 6to cuadrito
 lcd.print("____"); // Espacios a escribir
 //seguro.write(90); //Cerrar puerta
 lcd.setCursor(0,0);
 lcd.print("  **PASSWORD** ");

}//fin del setup  


void loop(){
 cl[contador] = teclado.getKey(); // almacenar en la posicion contador la tecla pulsada
 if (cl[contador] != NO_KEY){ // Si se pulsó alguna tecla entonces

 pulsacion();
 switch (cl[contador]){ //condiciones segun la tecla pulsada

 //*****************************************//
 //Comprobar si la contraseña es correcta
 //******************************************//
 case 'A':
 if (comprobacion(cl,co)){ //si los 4 digitos son los mismos entonces
 lcd.clear();
 Serial.print(comprobacion(cl,co));
 lcdtextoizq(" **Correcto**");
 seguro.write(15); //abrir puerta
 ledverde();
 tonoClaveCorrecta();
 delay(600);
 lcdtextoizq("'B' para cerrar");

 } //fin del if de comp >=4
 else if (comprobacion(cl,ca)){  //////////cambiar pass de usuario por medio de admin
  limpiar();
  lcd.setCursor(0,0);
  lcd.print("Bienvenido admin");
  delay(2000);
  nuevapassmaster();
  vuelve();
  if (comprobacion(cn,cl)){
 lcd.clear();
 lcdtextoizq("Correcto");
 delay(500);
 lcdtextoizq("Nueva pass");
 delay(500);
 lcd.clear();
 lcdtextoizq("pass guardada");
 delay(500);
 for(int i =0;i<=4;i++){
 EEPROM.write(i,cl[i]);}
 for(int i =0;i<=4;i++){
 co[i]= EEPROM.read(i);}
 limpiar();
 lcdnumero();

 lcdtitulo(" **PASSWORD** ");
 }  
 }
 
 else{ //si al pulsar 'A' no son iguales
 ledrojoblink();
 Serial.print(cl);
 lcd.clear();
 lcdtextocentrado("Error");
 tonoClaveIncorrecta(); 
 delay(600);

 lcd.setCursor(0,1);
 delay(500);
 lcdnumero();
 printpassword();
 } // fin del else al if comp>=4
 break; //break al caso 'a'

 //*************************************//
 // ***Cierra la puerta si se pulsa B //
 //*************************************//
 case 'B':
 
 lcd.clear();
 lcdtextocentrado("Cerrado");
 delay(500);
 seguro.write(90); //cierra el seguro
 ledrojo();
 borrarCodigoIntroducido(cl);
 lcdnumero();
 printpassword();
 break; //fin del caso B

 case 'C':
 conta++;
 lcd.clear();
 repetirpass();//debe validar la contraseña
 Serial.print(cl);
 if (comprobacion(co,cl)){  
 lcd.clear();//si la contraseña concide entonce
 lcdtextoizq("Pass correcta");
 delay(500);
 limpiar();
 nuevapass();
 vuelve(); //volver a introducir
 } //funcion cambio de password
 else{
 lcd.setCursor(0,1);
 lcd.clear();
 lcd.print("No pudo cambiar");
 delay(2000);
 limpiar();
 printpassword();
 lcdnumero();
 break;

 }

 if (comprobacion(cn,cl)){
 lcd.clear();
 lcdtextoizq("Correcto");
 delay(500);
 lcdtextoizq("Nueva pass");
 delay(500);
 lcd.clear();
 lcdtextoizq("pass guardada");
 delay(500);
 for(int i =0;i<=4;i++){
 EEPROM.write(i,cl[i]);}
 for(int i =0;i<=4;i++){
 co[i]= EEPROM.read(i);}
 limpiar();
 lcdnumero();

 lcdtitulo(" **PASSWORD** ");
 } 
 break;//fin caso c


 default:
 escribirn(cl);


 } //fin del switch

 } //fin del if si se pulso alguna tecla

 if (contador>4){ //Vuelve a hacer que el cursor comience a partir de cuadrito 6
 contador=0;
 cursorr=6;
 }
}//fin del loop
//*********************************
//*****Seccion de funciones********
//*********************************
void nuevapassmaster(){
 limpiar();
 lcd.setCursor(0,0);
 lcd.print("Intro nueva pass");
 delay(2000);
 limpiar();
 lcd.setCursor(0,0);
 lcd.print("Finalice con 'D'");
 delay(2000);
 limpiar();
 lcd.setCursor(0,0);
 lcd.print("SOLO NUMEROS");
 delay(1500);
 lcdnumero();
 contador=0;
 while(contador<=4){ //cuatro veces el ciclo
 cl[contador] = teclado.getKey();
 if (cl[contador]!=NO_KEY){ //si escribi algo
 pulsacion();
 switch (cl[contador]){

 case 'D':
 if (contador=4){ // si ya escribio 4 numeros
 lcdtextoizq("Vuelve a ingresarla");
 delay(1000);
 contador++;
 }//fin del if contador==$
 else { // si no escribio 4 numeros
 lcdtextoizq("Error de pass");
 delay(500);
 lcd.setCursor(0,1);
 lcdtextoizq(" ");
 lcd.print(contador);
 contador++;
 }//Fin del else
 break;

 default:
 if ((contador==4)&& (cl[contador]!='D')){
 lcdtextoizq("Finnalice con D");
 borrarCodigoIntroducido(cl);
 nuevapass();

 } 
 else{
 escribirn(cl);
 }

 }//fin del switch

 //fin del if
 }//fn del if no key

}//fin de while
}//finde nueva pass
void pulsacion(){ // tono de la pulsacion
 tone(buzzer,350);
 delay(200);
 noTone(buzzer);
}
void tonoClaveCorrecta(){ // tono de la clave correcta
 delay(200);
 tone(buzzer,500);
 delay(100);
 noTone(buzzer);
 tone(buzzer,600);
 delay(100);
 noTone(buzzer);
 tone(buzzer,800);
 delay(100);
 noTone(buzzer);
}
void tonoClaveIncorrecta(){ 
 tone(buzzer,70,500); // para generar
 delay(250); // tono de error
 noTone(buzzer);
}
void borrarCodigoIntroducido(char *array){
 for(int i = 0 ; i<5;i++){ // borra el codigo temporal guardado
 array[i]='0';
 }//fin del for
}
void borrarCodigoAdmi(char *array){
 for(int i = 0 ; i<7;i++){ // borra el codigo temporal guardado
 array[i]='0';
 }//fin del for
}
void lcdnumero(){ //reseta los valores del cursor del lcd asi como del contador
 lcd.setCursor(0,1); //segundo renglon
 lcd.print(" ");
 lcd.setCursor(6,1);
 lcd.print("____   ");
 cursorr=6;
 contador=0;
} //fin de lcdresetparanumero
void lcdtextocentrado(String A){ //escribe el mensaje segundo renglon 1er cuadro
 lcd.setCursor(0,1);
 lcd.print(" ");
 lcd.setCursor(6,1);
 lcd.print(A); //imprime el valor introducido
} //fin de lcdresetparatexto

void lcdtextoizq(String A){ //escribe el mensaje segundo renglon 6tocuadro
 lcd.setCursor(0,1);
 lcd.print(" ");
 lcd.setCursor(0,1);
 lcd.print(A); //imprime el valor introducido
} //fin de lcdresetparatexto

void ledrojo(){ //enciende led rojo
 digitalWrite(13, HIGH);
 digitalWrite(12,LOW);
} 
void ledrojoblink(){ //enciende led rojo parpadeando
  for(int i=0; i<5;i++){
 digitalWrite(13, HIGH);
 delay(50);
 digitalWrite(13,LOW);
 digitalWrite(12, LOW);
 delay(50);
 digitalWrite(12, HIGH);

}
 digitalWrite(13, HIGH);
 digitalWrite(12,LOW);}

void ledverde(){ //enciende led verde
 digitalWrite(13, LOW);
 digitalWrite(12, HIGH);
}
void escribirn(char *v){ //escribe el valor de un array en la posicion actual del contador
 lcd.setCursor(cursorr,1);
 lcd.print(v[contador]);
 v[contador+1]= (' ');
 delay(250);
 lcd.setCursor(cursorr,1);
 lcd.print('*');
 cursorr++;
 contador++;
}
boolean comprobacion(char *v1,char *v2){
 int cont=0;
 for(int i = 0; i<=3; i++){
 if(v1[i]==v2[i])
 cont++;
 }
 return (cont==4);
}
boolean comprobarClaveAdmi(char *v1,char *v2){
 int cont=0;
 for(int i = 0; i<=3; i++){
 if(v1[i]=v2[i])
 cont++;
 }
 return (cont==4);
}
void lcdtitulo(String a){ 
 lcd.setCursor(0,0);
 lcd.print(" ");
 lcd.setCursor(0,0);
 lcd.print(a); //imprime el valor introducido
} //fin de lcdresetparatexto
void repetirpass(){
 int i =0;
 lcd.clear();
 lcdtitulo("Introduzca pass");
 delay(800);
 lcd.clear();
 lcdtitulo("pass anterior");
 delay(1000);
 lcd.clear();
 delay(2000);
 lcd.print(" Finalice con *");
 lcdnumero();
 contador=0;
 while (i<=4){ //hará esto 4 vces

 cl[contador] = teclado.getKey(); //obtiene valor
 if (cl[contador]!=NO_KEY){ //si detecta una tecla entonces
 pulsacion();
 switch (cl[contador]){

 case '*': //comprueba si la pass escrita es igual a la anterior

 if (comprobacion(cl,co)){ //si es igual
  lcd.clear();
 lcdtextocentrado("Correcto");
 ledverde();
 delay(300);
 ledrojo();
 i++;} //fin if comp<=4
 else{
  lcd.clear();
 lcdtextocentrado("Error");
 ledrojoblink();
 i++;
 }//end else
 break; //fin del caso *

 default:
 if (comprobacion(cl,co) && (contador==4)&& (cl[contador]!= '*')){
 borrarCodigoIntroducido(cl);
 i++;


 }
 else{
 escribirn(cl); 
 i++;
 }
 }//fin del switch
 }//fin del if no key
 }//fin del if contador==4
 //fin del while
}//fin de la funcion repetirpass
void nuevapass(){
 limpiar();
 lcd.setCursor(0,0);
 lcd.print("Intro nueva pass");
 delay(2000);
 limpiar();
 lcd.setCursor(0,0);
 lcd.print("Finalice con 'D'");
 delay(2000);
 limpiar();
 lcd.setCursor(0,0);
 lcd.print("SOLO NUMEROS");
 delay(1500);
 lcdnumero();
 contador=0;
 while(contador<=4){ //cuatro veces el ciclo
 cl[contador] = teclado.getKey();
 if (cl[contador]!=NO_KEY){ //si escribi algo
 pulsacion();
 switch (cl[contador]){

 case 'D':
 if (contador=4){ // si ya escribio 4 numeros
 lcdtextoizq("Vuelve a ingresarla");
 delay(1000);
 contador++;
 }//fin del if contador==$
 else { // si no escribio 4 numeros
 lcdtextoizq("Error de pass");
 delay(500);
 lcd.setCursor(0,1);
 lcdtextoizq(" ");
 lcd.print(contador);
 contador++;
 }//Fin del else
 break;

 default:
 if ((contador==4)&& (cl[contador]!='D')){
 lcdtextoizq("Finnalice con D");
 borrarCodigoIntroducido(cl);
 nuevapass();

 } 
 else{
 escribirn(cl);
 }

 }//fin del switch

 //fin del if
 }//fn del if no key

}//fin de while
}//finde nueva pass
void vuelve(){
  lcd.clear();
 lcdtitulo("Escribe otra vez");
 lcd.setCursor(0,0);
 lcd.print("Finalice con '#'");
 delay(1500);
 lcdnumero();
 contador=0;
 while(contador<=4){ //cuatro veces el ciclo
 cn[contador] = teclado.getKey();
 if (cn[contador]!=NO_KEY){ //si escribi algo
 pulsacion();
 switch (cn[contador]){

 case '#':
 if (contador==4){ // si ya escribio 4 numeros
 lcdtextoizq("Contraseña leida");
 contador++;
 }//fin del if contador==$
 else { // si no escribio 4 numeros
  lcd.clear();
 lcdtextoizq("Error de pass");
 delay(500);
 lcd.setCursor(0,1);
 lcdtextoizq(" ");
 lcd.print(contador);
 }//Fin del else
 break;

 default:
 if ((contador==4)&& (cn[contador]!='#')){
 borrarCodigoIntroducido(cn);
 vuelve();
 
 }
 else{
 escribirn(cn);
 }
 }//fin del switch
 //fin del if
 }//fn del if no key
}//fin de while
}
//finde volver pass


void printpassword(){
   lcd.setCursor(0,0);
 lcd.print("  **PASSWORD** ");}


void limpiar(){
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}
 

//finde volver pas
 

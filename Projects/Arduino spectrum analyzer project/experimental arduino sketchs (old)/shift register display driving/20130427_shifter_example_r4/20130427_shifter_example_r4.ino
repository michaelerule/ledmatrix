#define LOG_OUT 1 // use the log output function
#define FHT_N 256 // set to 256 point fht

#include <FHT.h> // include the library

#define SER_Pin 2 //SER_IN
#define RCLK_Pin 3 //L_CLOCK
#define SRCLK_Pin 4 //CLOCK

#define D1_CLK 5 //
#define D1_RES 6 //
#define D2_CLK 7 //
#define D2_RES 8//

#define D1CLK_low()  PORTD&=~(1<<5)
#define D1CLK_high() PORTD|=(1<<5)

#define D1RES_low()  PORTD&=~(1<<6)
#define D1RES_high() PORTD|=(1<<6)

#define D2CLK_low()  PORTD&=~(1<<7)
#define D2CLK_high() PORTD|=(1<<7)

#define D2RES_low()  PORTD&=~(1<<8)
#define D2RES_high() PORTD|=(1<<8)

#define NUM_REGISTERS 3 //how many registers are in the chain

#define COL_DELAY 1

const int numColumns = 18;
const int numRows = 20;

int pixels[18][20];

int sensorPin = A2; 


void setup(){
  // DDRD = 0xFF;
  // PORTD = 0x00;
  pinMode(SER_Pin, OUTPUT);
  pinMode(RCLK_Pin, OUTPUT);
  pinMode(SRCLK_Pin, OUTPUT);

  pinMode(D1_CLK, OUTPUT);
  pinMode(D1_RES, OUTPUT);
  pinMode(D2_CLK, OUTPUT);
  pinMode(D2_RES, OUTPUT);

  digitalWrite(D1_RES, LOW);
  digitalWrite(D2_RES, LOW);

  CheckLEDs();

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void loop(){
  //shifter.clear(); //set all pins on the shift register chain to LOW
  /* shifter.setAll(HIGH); //Set
   shifter.write(); //send changes to the chain and display them
   
   delay(1000);
   
  /*  shifter.setPin(1, HIGH); //set pin 1 in the chain(second pin) HIGH
   shifter.setPin(3, HIGH);
   shifter.setPin(5, HIGH);
   shifter.setPin(7, HIGH);
   
   shifter.write(); //send changes to the chain and display them
   //notice how you only call write after you make all the changes you want to make
   
   delay(1000);*/


  //  shifter.clear();//Set all pins on the chain high
  //shifter.write(); //send changes to the chain and display them


  /*

   int sensorValue = analogRead(sensorPin); 
   
   int Level_0 = map(sensorValue, 0,1023,0,20);
   
   for (int x = 0; x < numRows; x++) {
   
   if(x<=Level_0-1){
   pixels[0][x] = HIGH;
   pixels[1][x] = HIGH;
   }
   else{
   pixels[0][x] = LOW;
   pixels[1][x] = LOW;
   }
   }
   */
  //refreshSmart();
  // displayTest();


  //displayGrid();
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
    //Serial.write(255); // send a start byte
    //Serial.write(fht_log_out, FHT_N/2); // send out the data

    

    setLevels();
    //delay(100);

    refreshSmart();

  }




}

void refreshSmart(){
  /*
  // digitalWrite(D1_RES, LOW);
  for (int thisCol=0; thisCol < 10; thisCol++) {
    digitalWrite(D1_CLK, HIGH);
    for (int thisRow; thisRow < numRows; thisRow++)
        shifter.setPin(thisRow, pixels[thisCol][thisRow]);
    shifter.write(); 
    delayMicroseconds(COL_DELAY);
    digitalWrite(D1_CLK, LOW);
  }
  //digitalWrite(D1_RES, HIGH);
  */
  
  for (int thisCol=0; thisCol < 10; thisCol++) {
    D2CLK_low();
    delay(1);
    D2CLK_high();
    delay(1);
  }
  for (int thisCol=0; thisCol < 10; thisCol++) {
    //#define SER_Pin 2   //SER_IN
    //#define RCLK_Pin 3  //L_CLOCK
    //#define SRCLK_Pin 4 //CLOCK
    for (int thisRow=0; thisRow < numRows; thisRow++)
    {
        digitalWrite(SER_Pin,1-pixels[thisCol][thisRow]);
        digitalWrite(SRCLK_Pin,0);
        digitalWrite(SRCLK_Pin,1);
    }
    // show the data
    digitalWrite(RCLK_Pin,LOW);
    digitalWrite(RCLK_Pin,HIGH);
    //delayMicroseconds(1500);
    // advance decade counter ( rise triggered )

    digitalWrite(SER_Pin,HIGH);  
    for (int thisRow=0; thisRow < numRows; thisRow++)
    {
        digitalWrite(SRCLK_Pin,LOW);
        digitalWrite(SRCLK_Pin,HIGH);
    }
    digitalWrite(RCLK_Pin,LOW);
    digitalWrite(RCLK_Pin,HIGH);

    D1CLK_low();
    D1CLK_high();
  }
  
    D2CLK_low();
    D2CLK_high();
    D2CLK_low();
    D2CLK_high();
  // iterate over the second decade counter
  for (int thisCol=10; thisCol<18; thisCol++) {
    //#define SER_Pin 2   //SER_IN
    //#define RCLK_Pin 3  //L_CLOCK
    //#define SRCLK_Pin 4 //CLOCK
    for (int thisRow=0; thisRow < numRows; thisRow++)
    {
        digitalWrite(SER_Pin,1-pixels[thisCol][thisRow]);
        digitalWrite(SRCLK_Pin,0);
        digitalWrite(SRCLK_Pin,1);
    }
    // show the data
    digitalWrite(RCLK_Pin,0);
    digitalWrite(RCLK_Pin,1);
    //delayMicroseconds(1500);

    digitalWrite(SER_Pin,HIGH);  
    for (int thisRow=0; thisRow < numRows; thisRow++)
    {
        digitalWrite(SRCLK_Pin,LOW);
        digitalWrite(SRCLK_Pin,HIGH);
    }
    digitalWrite(RCLK_Pin,LOW);
    digitalWrite(RCLK_Pin,HIGH);

    // advance decade counter ( rise triggered )
    D2CLK_low();
    D2CLK_high();
  }
}

void CheckLEDs(){
  int Level_0 = 2;
  int Level_1 = 3;
  int Level_2 = 4;
  int Level_3 = 5;


  int Level_4 = 6;
  int Level_5 = 7;
  int Level_6 = 8;
  int Level_7 = 9;

  int Level_8 = 10;
  int Level_9 = 11;
  int Level_10 = 12;
  int Level_11 = 13;

  int Level_12 = 14;
  int Level_13 = 15;
  int Level_14 = 14;
  int Level_15 = 13;

  int Level_16 = 12;
  int Level_17 = 11;


  for (int x = 0; x < numRows; x++) {

    /*   if(x<=Level_0-1){
     pixels[0][x] = HIGH;
     }
     else{
     pixels[0][x] = LOW;
     }*/

    if(x<=Level_1-1){
      pixels[1][x] = HIGH;
    }
    else{
      pixels[1][x] = LOW;
    }

    if(x<=Level_2-1){
      pixels[2][x] = HIGH;
    }
    else{
      pixels[2][x] = LOW;
    }

    if(x<=Level_3-1){
      pixels[3][x] = HIGH;
    }
    else{
      pixels[3][x] = LOW;
    }

    if(x<=Level_4-1){
      pixels[4][x] = HIGH;
    }
    else{
      pixels[4][x] = LOW;
    }

    if(x<=Level_5-1){
      pixels[5][x] = HIGH;
    }
    else{
      pixels[5][x] = LOW;
    }

    if(x<=Level_6-1){
      pixels[6][x] = HIGH;
    }
    else{
      pixels[6][x] = LOW;
    }

    if(x<=Level_7-1){
      pixels[7][x] = HIGH;
    }
    else{
      pixels[7][x] = LOW;
    }

    if(x<=Level_8-1){
      pixels[8][x] = HIGH;
    }
    else{
      pixels[8][x] = LOW;
    } 

    if(x<=Level_9-1){
      pixels[9][x] = HIGH;
    }
    else{
      pixels[9][x] = LOW;
    } 

    if(x<=Level_10-1){
      pixels[10][x] = HIGH;
    }
    else{
      pixels[10][x] = LOW;
    } 

    if(x<=Level_11-1){
      pixels[11][x] = HIGH;
    }
    else{
      pixels[11][x] = LOW;
    }

    if(x<=Level_12-1){
      pixels[12][x] = HIGH;
    }
    else{
      pixels[12][x] = LOW;
    }

    if(x<=Level_13-1){
      pixels[13][x] = HIGH;
    }
    else{
      pixels[13][x] = LOW;
    }

    if(x<=Level_14-1){
      pixels[14][x] = HIGH;
    }
    else{
      pixels[14][x] = LOW;
    }

    if(x<=Level_15-1){
      pixels[15][x] = HIGH;
    }
    else{
      pixels[15][x] = LOW;
    }

    if(x<=Level_16-1){
      pixels[16][x] = HIGH;
    }
    else{
      pixels[16][x] = LOW;
    }

    if(x<=Level_17-1){
      pixels[17][x] = HIGH;
    }
    else{
      pixels[17][x] = LOW;
    }

  }
}

void randomLED(){

  pixels[random(0,18)][random(0,20)] = HIGH;
  pixels[random(0,18)][random(0,20)] = LOW;

}

void setLevels(){

  /*  int Level_0 = map(fht_log_out[2], 0,256,0,20);
   int Level_1 = map(fht_log_out[16], 0,256,0,20);
   int Level_2 = map(fht_log_out[32], 0,256,0,20);
   int Level_3 = map(fht_log_out[64], 0,256,0,20);
   int Level_4 = map(fht_log_out[96], 0,256,0,20);
   int Level_5 = map(fht_log_out[124], 0,256,0,20);&*/

  int Level_0 = map(fht_log_out[2], 0,256,0,20);
  int Level_1 = map(fht_log_out[8], 0,256,0,20);
  int Level_2 = map(fht_log_out[16], 0,256,0,20);
  int Level_3 = map(fht_log_out[24], 0,256,0,20);

  int Level_4 = map(fht_log_out[32], 0,256,0,20);
  int Level_5 = map(fht_log_out[40], 0,256,0,20);
  int Level_6 = map(fht_log_out[48], 0,256,0,20);
  int Level_7 = map(fht_log_out[56], 0,256,0,20);

  int Level_8 = map(fht_log_out[64], 0,256,0,20);
  int Level_9 = map(fht_log_out[72], 0,256,0,20);
  int Level_10 = map(fht_log_out[80], 0,256,0,20);
  int Level_11 = map(fht_log_out[88], 0,256,0,20);

  int Level_12 = map(fht_log_out[96], 0,256,0,20);
  int Level_13 = map(fht_log_out[104], 0,256,0,20);
  int Level_14 = map(fht_log_out[112], 0,256,0,20);
  int Level_15 = map(fht_log_out[126], 0,256,0,20);

  int Level_16 = Level_14+3;
  int Level_17 = Level_15+3;


  for (int x = 0; x < numRows; x++) {

    if(x<=Level_0-1){
      pixels[0][x] = HIGH;
    }
    else{
      pixels[0][x] = LOW;
    }

    if(x<=Level_1-1){
      pixels[1][x] = HIGH;
    }
    else{
      pixels[1][x] = LOW;
    }

    if(x<=Level_2-1){
      pixels[2][x] = HIGH;
    }
    else{
      pixels[2][x] = LOW;
    }

    if(x<=Level_3-1){
      pixels[3][x] = HIGH;
    }
    else{
      pixels[3][x] = LOW;
    }

    if(x<=Level_4-1){
      pixels[4][x] = HIGH;
    }
    else{
      pixels[4][x] = LOW;
    }

    if(x<=Level_5-1){
      pixels[5][x] = HIGH;
    }
    else{
      pixels[5][x] = LOW;
    }

    if(x<=Level_6-1){
      pixels[6][x] = HIGH;
    }
    else{
      pixels[6][x] = LOW;
    }

    if(x<=Level_7-1){
      pixels[7][x] = HIGH;
    }
    else{
      pixels[7][x] = LOW;
    }

    if(x<=Level_8-1){
      pixels[8][x] = HIGH;
    }
    else{
      pixels[8][x] = LOW;
    } 

    if(x<=Level_9-1){
      pixels[9][x] = HIGH;
    }
    else{
      pixels[9][x] = LOW;
    } 

    if(x<=Level_10-1){
      pixels[10][x] = HIGH;
    }
    else{
      pixels[10][x] = LOW;
    } 

    if(x<=Level_11-1){
      pixels[11][x] = HIGH;
    }
    else{
      pixels[11][x] = LOW;
    }

    if(x<=Level_12-1){
      pixels[12][x] = HIGH;
    }
    else{
      pixels[12][x] = LOW;
    }

    if(x<=Level_13-1){
      pixels[13][x] = HIGH;
    }
    else{
      pixels[13][x] = LOW;
    }

    if(x<=Level_14-1){
      pixels[14][x] = HIGH;
    }
    else{
      pixels[14][x] = LOW;
    }

    if(x<=Level_15-1){
      pixels[15][x] = HIGH;
    }
    else{
      pixels[15][x] = LOW;
    }


    if(x<=Level_16-1){
      pixels[16][x] = HIGH;
    }
    else{
      pixels[16][x] = LOW;
    }
    if(x<=Level_17-1){
      pixels[17][x] = HIGH;
    }
    else{
      pixels[17][x] = LOW;
    }

  }
}



/*void shift(int SRCLK_Pin, int RCLK_Pin, int SER_Pin, unsigned long data){
 PORTD &= ~(1 << RCLK_Pin); 				// Set the register-clock pin low
 
 for (int i = 0; i < 13; i++){	// Now we are entering the loop to shift out 8+ bits
 
 PORTD &= ~(1 << SRCLK_Pin); 			// Set the serial-clock pin low
 
 PORTD |= (((data&(0x01<<i))>>i) << SER_Pin ); 	// Go through each bit of data and output it
 
 PORTD |= (1 << SRCLK_Pin); 			// Set the serial-clock pin high
 
 PORTD &= ~(((data&(0x01<<i))>>i) << SER_Pin );	// Set the datapin low again	
 }
 
 PORTD |= (1 << RCLK_Pin);				// Set the register-clock pin high to update the output of the shift-register
 }
 
 
 */





















































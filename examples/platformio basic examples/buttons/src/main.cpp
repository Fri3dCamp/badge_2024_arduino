#include <Arduino.h>
#include <JC_Button.h>          // https://github.com/JChristensen/JC_Button

Button button_A(39);
Button button_B(40);
Button button_X(38);
Button button_Y(41);
Button button_MENU(45);
Button button_START(0);

void setup()
{  
  Serial.begin(115200);
  delay(3000); 
  Serial.println("Fri3dbadge Button test");

  button_A.begin();              
  button_B.begin();              
  button_X.begin();              
  button_Y.begin();              
  button_MENU.begin();              
  button_START.begin();              
}

void checkButton(Button *b,const char *buttonname) {

    b->read();

    if (b->wasPressed())
    {
      Serial.print(buttonname);
      Serial.println(" pressed");
    }
    if (b->wasReleased())
    {
      Serial.print(buttonname);
      Serial.println(" released");
    }
}

void loop()
{
  checkButton(&button_A,"A");
  checkButton(&button_B,"B");
  checkButton(&button_X,"X");
  checkButton(&button_Y,"Y");
  checkButton(&button_MENU,"MENU");
  checkButton(&button_START,"START");
}

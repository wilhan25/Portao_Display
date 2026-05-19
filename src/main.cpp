/**********************************************************************
  Filename    : Automacao_automatica
  Description : controle de casa inteligente
  Auther      : Eng.Wilhan Almeida
  Criado: 15/05/2026
**********************************************************************/
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>

// Servo
Servo portao;
int servoPin = 13;

// lcd
#define SDA 18
#define SCL 5
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool i2CAddrTest(uint8_t addr);

// teclado
char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte linhaPins[4] = {27, 26, 25, 33};
byte colPins[4] = {19, 21, 22, 23};
Keypad myKeypad = Keypad(makeKeymap(keys), linhaPins, colPins, 4, 4);

// senha da portao
char senhaPortao[] = {"505"};

void setup()
{
  Wire.begin(SDA, SCL); // attach the IIC pin
  if (!i2CAddrTest(0x27))
  {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();          // LCD driver initialization
  lcd.backlight();     // Open the backlight
  lcd.setCursor(0, 0); // Move the cursor to row 0, column 0
  lcd.print("Qual a senha ?");

  portao.setPeriodHertz(50);
  portao.attach(servoPin, 500, 2500);
  portao.write(0);
}

void loop()
{

  static char senhaDigitada[4] = {};
  static byte senhaPos = 0; // posicao do caracter digitado no vetor da senha
  // preciso ler os numeros digitados
  char keyPressed = myKeypad.getKey();

  // escrever em tempo real na tela do lcd
  lcd.setCursor(0, 1);
  if (keyPressed)
  {
    senhaDigitada[senhaPos++] = keyPressed;
    lcd.print(senhaDigitada);
    if (senhaPos > 2)
    {
      if (senhaPortao[0] == senhaDigitada[0] && senhaPortao[1] == senhaDigitada[1] && senhaPortao[2] == senhaDigitada[2])
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bem-Vindo");
        portao.write(180);
        senhaPos = 0;
        unsigned long tempoInicial = millis();

        while (millis() - tempoInicial < 5000)
        {

          char tecla = myKeypad.getKey();

          if (tecla == 'A')
          {

            senhaPos = 0;

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("NOVA SENHA:");

            while (senhaPos < 3)
            {

              char novaTecla = myKeypad.getKey();

              if (novaTecla)
              {

                senhaPortao[senhaPos] = novaTecla;

                lcd.setCursor(senhaPos, 1);
                lcd.print(novaTecla);

                senhaPos++;
              }
            }

            senhaPortao[3] = '\0';

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Senha alterada");

            delay(2000);

            break;
          }
        }
        portao.write(0);

        senhaDigitada[0] = '-';
        senhaDigitada[1] = '-';
        senhaDigitada[2] = '-';

        lcd.clear();
        lcd.setCursor(0, 0); // Move the cursor to row 0, column 0
        lcd.print("Qual a senha ?");
        senhaPos = 0;
      }
      else
      {
        lcd.clear();
        senhaDigitada[0] = '-';
        senhaDigitada[1] = '-';
        senhaDigitada[2] = '-';
        lcd.print("Vaza daqui !");
        senhaPos = 0;

        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0); // Move the cursor to row 0, column 0
        lcd.print("Qual a senha ?");
      }
    }
  }
}

bool i2CAddrTest(uint8_t addr)
{
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0)
  {
    return true;
  }
  return false;
}
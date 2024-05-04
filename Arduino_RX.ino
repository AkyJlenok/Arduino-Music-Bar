/*
   Данный алгоритм позволяет получить через Serial пачку значений, и раскидать
   их в целочисленный массив. Использовать можно банально для управления
   ЧЕМ УГОДНО через bluetooth, так как bluetooth модули есть UART интерфейс связи.
   Либо управлять через Serial с какой-то программы с ПК
   Как использовать:
   1) В PARSE_AMOUNT указывается, какое количество значений мы хотим принять.
   От этого значения напрямую зависит размер массива принятых данных, всё просто
   2) Пакет данных на приём должен иметь вид:
   Начало - символ $
   Разделитель - @
   Завершающий символ - ;
   Пример пакета: $110 25 600 920;  будет раскидан в массив intData согласно порядку слева направо
   Что делает данный скетч:
   Принимает пакет данных указанного выше вида, раскидывает его в массив intData, затем выводит обратно в порт.
   Отличие от предыдущего примера: написан мной, не используя никаких хитрых функций. Предельно просто и понятно работает
*/

#include <LCD-I2C.h>

LCD_I2C lcd(0x27, 16, 2);

#define PARSE_AMOUNT 2         // число значений в массиве, который хотим получить
String Data[PARSE_AMOUNT];     // массив численных значений после парсинга
boolean recievedFlag;
boolean getStarted;
byte index;
String string_convert = "";

unsigned long timer_slice[2] = {0, 0};
uint8_t slice[2] = {0, 0};
bool flag_slice[2] = {false, false};

uint8_t x_offset[2] = {16, 16};

void parsing() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != '@' && incomingByte != ';') {   // если это не @ И не ;
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        Data[index] = string_convert;  // преобразуем строку в int и кладём в массив
        string_convert = "";                  // очищаем строку
        index++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == '$') {                // если это $
      getStarted = true;                      // поднимаем флаг, что можно парсить
      index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ';') {                // если таки приняли ; - конец парсинга
      getStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
  }
}
void setup() {
  Serial.begin(9600);

  lcd.begin();
  lcd.display();
  lcd.backlight();

  while (!recievedFlag) {
    parsing();
    lcd.setCursor(0, 0);
    lcd.print("Connection...");
  }

}
void loop() {
  parsing();       // функция парсинга
  if (recievedFlag) {
    lcd.clear();
    recievedFlag = !recievedFlag;
    }
  for (byte i = 0; i < PARSE_AMOUNT; i++) {                                                         // выводим элементы массива

    if (Data[i].length() > 16) {                                                                    //если строка больше 16
      if (millis() - timer_slice[i] > 500) {
        if (x_offset[i] != 0) {
          lcd.setCursor(x_offset[i], i);
          lcd.print(Data[i]);
          x_offset[i]--;
        }
        else {
        slice[i]++;                                                                                 //увеличиваем срез
        lcd.setCursor(0, i);
        lcd.print(Data[i].substring(slice[i], Data[i].length()) + "                ");              //выводим на дисплей                                                                                            
        timer_slice[i] = millis();

        if (slice[i] >= Data[i].length()) {                                                       //если дошли до конца строки
          slice[i] = 0;                                                                           //сбросить слайдер
          x_offset[i] = 16;                                                                          //сбросить сдвиг
        }
      }
        timer_slice[i] = millis();
      }
    }

    else {                                                                                          //если меньше 16 то просто выводим и не ебём мозги
      lcd.setCursor(0, i);
      lcd.print(Data[i] + "                ");                                                      //выводим и очищаем артtфакты
    }
  }
      /*
      if (Data[i].length() <= 16) {
        lcd.setCursor(0, i);
        lcd.print(Data[i]);
      }

      
      else {
        for (int j = 0; j <= Data[i].length(); j++) {
          lcd.setCursor(0, i);
          lcd.print(Data[i].substring(j, Data[i].length()) + "                ");
          delay(1000);
        }
      }
    } 

*/
  }

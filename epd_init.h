#define PIN_SPI_SCK  13
#define PIN_SPI_DIN  14
#define PIN_SPI_CS   15
#define PIN_SPI_BUSY 25//19
#define PIN_SPI_RST  26//21
#define PIN_SPI_DC   27//22

/* Pin level definition ------------------------------------------------------*/
#define LOW             0
#define HIGH            1

#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

void EPD_initSPI_1()
{
    pinMode(PIN_SPI_BUSY,  INPUT);
    pinMode(PIN_SPI_RST , OUTPUT);
    pinMode(PIN_SPI_DC  , OUTPUT);
    
    pinMode(PIN_SPI_SCK, OUTPUT);
    pinMode(PIN_SPI_DIN, OUTPUT);
    pinMode(PIN_SPI_CS , OUTPUT);

    digitalWrite(PIN_SPI_CS , HIGH);
    digitalWrite(PIN_SPI_SCK, LOW);
}
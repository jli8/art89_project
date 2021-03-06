/*
 *    MAX7456 overlay code combined with the single mic code
 *
 *    This test code creates a vertical white bar on the screen
 *    in the direction of where the user hears a sound.
 */

#define F_CPU 16000000
#define SAMPLES 1000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define RESET 0 // PB0
#define CS 2 // PB2
#define MOSI 3 // PB3
#define MISO 4 // PB4
#define SCK 5 // PB5

// ========== Define functions ============
void ioinit(void); // Initializes IO
void welcome();
void complete();
void tinaSmilkstein();
void tinaSmillksteinCascade();
void tinaShow();
void verticalBarShow();
void enableAndClear();
void vertical_bar(int upper, int lower);
void SPI_write(char address, char byte);
char SPI_read(char address);
void Initialize_ADC0(void);
void Initialize_ADC1(void);
int GetSound(int mic, int samples);
void reset();
// ========== End of Define ============

int main (void) {
    char x;
    ioinit(); // Setup IO pins and defaults
    _delay_ms(1000);

    // === Reset ===
    PORTB &= ~(1<<RESET);
    _delay_ms(1000);
    PORTB |= (1<<RESET);
    _delay_ms(1000);
    // === End of Reset ===

    SPI_write(0, 0x08);
    // Bit 6: PAL Signal, Bit 3: Enable Display of OSD image
    _delay_ms(1);

    // Automatic black level control:
    // have to read, augment and rewrite
    // The data sheet is rather specific about this
    // ============================================
    x = SPI_read(0xEC);
    _delay_ms(1);
    x &= 0xEF;
    SPI_write(0x6C, x);
    _delay_ms(1);
    // ============================================

    // Adjust the screen to fit nicely onto the display 
    // ============================================
    SPI_write(0x04, 0); // DMM set to 0
    SPI_write(0x02, 0x1E); // Horizontal Offset Left by -30 pixels 
    SPI_write(0x03, 0x14); // Vertical Offset Down by -4 pixels 
    // ============================================

    /*
    Initialize_ADC0();
    int base_noise = GetSound(0, 1000);
    int threshold = base_noise + 30;
    */
    Initialize_ADC0();
    Initialize_ADC1();
    // Average
    int base_noise = ((GetSound(0, 1000) + GetSound(1, 1000)) / 2);
    int threshold = base_noise + 15, mic0 = 0, mic1 = 0;

    welcome();
    while(1) {
        // Tina Smilkstein
        // tinaShow();

        // Single Vertical Bar Testing
        /*
        vertical_bar(12, 26);
        SPI_write(0, 0x08); // Enable Display
        _delay_ms(200);
        SPI_write(0x04, 0x06); // Clear Display on /VSync
        _delay_ms(50);
        vertical_bar(25, 9);
        SPI_write(0, 0x08); // Enable Display
        _delay_ms(200);
        SPI_write(0x04, 0x06); // Clear Display on /VSync
        _delay_ms(50);
        */
        
        // Single Mic Testing
        /* int mic0 = GetSound(0, 1000);
        if (mic0 > threshold) {
            vertical_bar(14, 28); // Middle
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        */

        // Testing using two mics concurrently
        mic0 = GetSound(0, 1000);
        mic1 = GetSound(1, 1000);

        if ((mic0 > threshold) && (mic0 > (mic1 + 30))) {
            vertical_bar(0, 14); // Far Left 
            SPI_write(0, 0x08); // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display 
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 30))) { 
            vertical_bar(29, 13); // Far Right 
            SPI_write(0, 0x08); // Enable Display of OSD image 
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 28))) {
            vertical_bar(1, 15); // Left Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 28))) {
            vertical_bar(28, 12); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 26))) {
            vertical_bar(2, 16); // Left Side
            SPI_write(0, 0x08); // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 26))) {
            vertical_bar(27, 11); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
             _delay_ms(1000);
             SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 24))) {
            vertical_bar(3, 17); // Left Side
            SPI_write(0, 0x08); // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 24))) {
            vertical_bar(26, 10); // Right Side
            SPI_write(0, 0x08); // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 22))) {
            vertical_bar(4, 18); // Left Side
            SPI_write(0, 0x08); // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 22))) {
            vertical_bar(25, 9); // Right Side
            SPI_write(0, 0x08); // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 20))) {
            vertical_bar(5, 19); // Left Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 20))) {
            vertical_bar(24, 8); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 18))) {
            vertical_bar(6, 20); // Left Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }      
        else if ((mic1 > threshold) && (mic1 > (mic0 + 18))) {
              vertical_bar(23, 7); // Right Side 
              SPI_write(0, 0x08); // Enable Display of OSD image 
              _delay_ms(1000);
              SPI_write(0x04, 0x06); // Clear Display 
              _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 16))) {
            vertical_bar(7, 21); // Left Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 16))) {
            vertical_bar(22, 6); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 14))) {
            vertical_bar(8, 22); // Left Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 14))) {
            vertical_bar(21, 5); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 12))) {
            vertical_bar(9, 23); // Left Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 12))) {
            vertical_bar(20, 4); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 10))) {
            vertical_bar(10, 24); // Left Side 
            SPI_write(0, 0x08); // Enable Display of OSD image 
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display 
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 10))) {
            vertical_bar(19, 3); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 8))) {
            vertical_bar(11, 25); // Left Side 
            SPI_write(0, 0x08); // Enable Display of OSD image 
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display 
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 8))) {
            vertical_bar(18, 2); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 6))) { 
            vertical_bar(12, 26); // Left Side 
            SPI_write(0, 0x08); // Enable Display of OSD image 
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 6))) {
            vertical_bar(17, 1); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 4))) {
            vertical_bar(13, 27); // Left Side 
            SPI_write(0, 0x08); // Enable Display of OSD image 
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display 
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 4))) {
            vertical_bar(16, 0); // Right Side
            SPI_write(0, 0x08);  // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
        else if ((mic0 > threshold) && (mic0 > (mic1 + 2))) {
            vertical_bar(14, 28);  // Left Side 
            SPI_write(0, 0x08);    // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display 
            _delay_ms(500);
        }
        else if ((mic1 > threshold) && (mic1 > (mic0 + 2))) {
            vertical_bar(15, 29);  // Right Side
            SPI_write(0, 0x08);    // Enable Display of OSD image
            _delay_ms(1000);
            SPI_write(0x04, 0x06); // Clear Display
            _delay_ms(500);
        }
    }
}

void vertical_bar(int upper, int lower) {
    SPI_write(0x05, 0x00); // DMAH Top Half of Screen
    SPI_write(0x06, upper); // DMAL 1
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, upper+30); // DMAL 2
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, upper+60); // DMAL 3
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, upper+90); // DMAL 4
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, upper+120); // DMAL 5
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, upper+150); // DMAL 6
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, upper+180); // DMAL 7
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, upper+210); // DMAL 8
    SPI_write(0x07, 0xFF);

    if (upper <= 15) {
        SPI_write(0x06, upper+240); // DMAL 9
        SPI_write(0x07, 0xFF);
    }

    SPI_write(0x05, 0x01); // DMAH Bottom Half of Screen
    SPI_write(0x06, lower); // DMAL 9
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, lower+30);  // DMAL 10
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, lower+60);  // DMAL 11
    SPI_write(0x07, 0xFF);
    SPI_write(0x06, lower+90);  // DMAL 12
    SPI_write(0x07, 0xFF);

    if (lower <= 13 || lower == 255) {
        SPI_write(0x06, lower+120);  // DMAL 13
        SPI_write(0x07, 0xFF);
    }
}

void ioinit (void) {
    UCSR0B = 0x00; // Disable Tx and Rx
    PORTB = 0xFF;
    DDRB = ((1<<CS) | (1<<MOSI) | (1<<SCK) | (1<<RESET));
    TCCR2B = (1<<CS21); // Set Prescaler to 8. CS21=1 
}

void welcome() {
    SPI_write(0x05, 0x00); // DMAH Top Half of Screen
    
    // WELCOME
    SPI_write(0x06, 161); // DMAL
    SPI_write(0x07, 0x21); // W
    SPI_write(0x06, 162); // DMAL
    SPI_write(0x07, 0x0F); // E
    SPI_write(0x06, 163); // DMAL
    SPI_write(0x07, 0x16); // L
    SPI_write(0x06, 164); // DMAL
    SPI_write(0x07, 0x0D); // C
    SPI_write(0x06, 165); // DMAL
    SPI_write(0x07, 0x19); // O
    SPI_write(0x06, 166); // DMAL
    SPI_write(0x07, 0x17); // M
    SPI_write(0x06, 167); // DMAL
    SPI_write(0x07, 0x0F); // E
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(2000);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(50);

    // INITIALIZING...
    SPI_write(0x06, 156); // DMAL
    SPI_write(0x07, 0x13); // I
    SPI_write(0x06, 157); // DMAL
    SPI_write(0x07, 0x18); // N
    SPI_write(0x06, 158); // DMAL
    SPI_write(0x07, 0x13); // I
    SPI_write(0x06, 159); // DMAL
    SPI_write(0x07, 0x1E); // T
    SPI_write(0x06, 160); // DMAL
    SPI_write(0x07, 0x13); // I
    SPI_write(0x06, 161); // DMAL
    SPI_write(0x07, 0x0B); // A
    SPI_write(0x06, 162); // DMAL
    SPI_write(0x07, 0x16); // L
    SPI_write(0x06, 163); // DMAL
    SPI_write(0x07, 0x13); // I
    SPI_write(0x06, 164); // DMAL
    SPI_write(0x07, 0x24); // Z
    SPI_write(0x06, 165); // DMAL
    SPI_write(0x07, 0x13); // I
    SPI_write(0x06, 166); // DMAL
    SPI_write(0x07, 0x18); // N
    SPI_write(0x06, 167); // DMAL
    SPI_write(0x07, 0x11); // G
    SPI_write(0x06, 168); // DMAL
    SPI_write(0x07, 0x41); // .
    SPI_write(0x06, 169); // DMAL
    SPI_write(0x07, 0x41); // .
    SPI_write(0x06, 170); // DMAL
    SPI_write(0x07, 0x41); // .
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(1000);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(500);
    verticalBarShow();
    _delay_ms(500);
    complete();
}

void complete() {
    SPI_write(0x05, 0x00); // DMAH Top Half of Screen
    // COMPLETE
    SPI_write(0x06, 160); // DMAL
    SPI_write(0x07, 0x0D); // C
    SPI_write(0x06, 161); // DMAL
    SPI_write(0x07, 0x19); // O
    SPI_write(0x06, 162); // DMAL
    SPI_write(0x07, 0x17); // M
    SPI_write(0x06, 163); // DMAL
    SPI_write(0x07, 0x1A); // P
    SPI_write(0x06, 164); // DMAL
    SPI_write(0x07, 0x16); // L
    SPI_write(0x06, 165); // DMAL
    SPI_write(0x07, 0x0F); // E
    SPI_write(0x06, 166); // DMAL
    SPI_write(0x07, 0x1E); // T
    SPI_write(0x06, 167); // DMAL
    SPI_write(0x07, 0x0F); // E
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(1500);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(500);

    // READY FOR USE
    SPI_write(0x06, 158); // DMAL
    SPI_write(0x07, 0x1C); // R
    SPI_write(0x06, 159); // DMAL
    SPI_write(0x07, 0x0F); // E
    SPI_write(0x06, 160); // DMAL
    SPI_write(0x07, 0x0B); // A
    SPI_write(0x06, 161); // DMAL
    SPI_write(0x07, 0x0E); // D
    SPI_write(0x06, 162); // DMAL
    SPI_write(0x07, 0x23); // Y
    SPI_write(0x06, 164); // DMAL
    SPI_write(0x07, 0x10); // F
    SPI_write(0x06, 165); // DMAL
    SPI_write(0x07, 0x19); // O
    SPI_write(0x06, 166); // DMAL
    SPI_write(0x07, 0x1C); // R
    SPI_write(0x06, 168); // DMAL
    SPI_write(0x07, 0x1F); // U
    SPI_write(0x06, 169); // DMAL
    SPI_write(0x07, 0x1D); // S
    SPI_write(0x06, 170); // DMAL
    SPI_write(0x07, 0x0F); // E
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(1500);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(500);
}

void tinaSmilkstein() {
    SPI_write(0x06, 37); // DMAL 1
    SPI_write(0x07, 0x1E); // T
    SPI_write(0x06, 38); // DMAL 2
    SPI_write(0x07, 0x2D); // i
    SPI_write(0x06, 39); // DMAL 3
    SPI_write(0x07, 0x32); // n
    SPI_write(0x06, 40); // DMAL 4
    SPI_write(0x07, 0x25); // a
    SPI_write(0x06, 42); // DMAL 5
    SPI_write(0x07, 0x1D); // S
    SPI_write(0x06, 43); // DMAL 6
    SPI_write(0x07, 0x31); // m
    SPI_write(0x06, 44); // DMAL 7
    SPI_write(0x07, 0x2D); // i
    SPI_write(0x06, 45); // DMAL 7
    SPI_write(0x07, 0x30); // l
    SPI_write(0x06, 46); // DMAL 7
    SPI_write(0x07, 0x2F); // k
    SPI_write(0x06, 47); // DMAL 8
    SPI_write(0x07, 0x37); // s
    SPI_write(0x06, 48); // DMAL 9
    SPI_write(0x07, 0x38); // t
    SPI_write(0x06, 49); // DMAL 10
    SPI_write(0x07, 0x29); // e
    SPI_write(0x06, 50); // DMAL 11
    SPI_write(0x07, 0x2D); // i
    SPI_write(0x06, 51); // DMAL 12
    SPI_write(0x07, 0x32); // n
}

void tinaSmillksteinCascade() {
    SPI_write(0x06, 37); // DMAL 1
    SPI_write(0x07, 0x1E); // T
    enableAndClear();
    SPI_write(0x06, 38); // DMAL 2
    SPI_write(0x07, 0x2D); // i
    enableAndClear();
    SPI_write(0x06, 39); // DMAL 3
    SPI_write(0x07, 0x32); // n
    enableAndClear();
    SPI_write(0x06, 40); // DMAL 4
    SPI_write(0x07, 0x25); // a
    enableAndClear();
    SPI_write(0x06, 42); // DMAL 5
    SPI_write(0x07, 0x1D); // S
    enableAndClear();
    SPI_write(0x06, 43); // DMAL 6
    SPI_write(0x07, 0x31); // m
    enableAndClear();
    SPI_write(0x06, 44); // DMAL 7
    SPI_write(0x07, 0x2D); // i
    enableAndClear();
    SPI_write(0x06, 45); // DMAL 7
    SPI_write(0x07, 0x30); // l
    enableAndClear();
    SPI_write(0x06, 46); // DMAL 7
    SPI_write(0x07, 0x2F); // k
    enableAndClear();
    SPI_write(0x06, 47); // DMAL 8
    SPI_write(0x07, 0x37); // s
    enableAndClear();
    SPI_write(0x06, 48); // DMAL 9
    SPI_write(0x07, 0x38); // t
    enableAndClear();
    SPI_write(0x06, 49); // DMAL 10
    SPI_write(0x07, 0x29); // e
    enableAndClear();
    SPI_write(0x06, 50); // DMAL 11
    SPI_write(0x07, 0x2D); // i
    enableAndClear();
    SPI_write(0x06, 51); // DMAL 12
    SPI_write(0x07, 0x32); // n
    enableAndClear();
}

void tinaShow() {
    tinaSmillksteinCascade(); // Flash letters
    tinaSmilkstein(); // Load entire name into max7456 memory
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(3000);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(500);
    tinaSmilkstein();
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(500);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(500);
    tinaSmilkstein();
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(500);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(500);
}

void verticalBarShow()
{
    // Range of Vertical Bars from Left to Right:
    vertical_bar(0, 14); // Far Left
    enableAndClear();
    vertical_bar(1, 15);
    enableAndClear();
    vertical_bar(2, 16);
    enableAndClear();
    vertical_bar(3, 17);
    enableAndClear();
    vertical_bar(4, 18);
    enableAndClear();
    vertical_bar(5, 19);
    enableAndClear();
    vertical_bar(6, 20);
    enableAndClear();
    vertical_bar(7, 21);
    enableAndClear();
    vertical_bar(8, 22);
    enableAndClear();
    vertical_bar(9, 23);
    enableAndClear();
    vertical_bar(10, 24);
    enableAndClear();
    vertical_bar(11, 25);
    enableAndClear();
    vertical_bar(12, 26);
    enableAndClear();
    vertical_bar(13, 27); // Left
    enableAndClear();
    vertical_bar(14, 28); // Middle
    enableAndClear();
    vertical_bar(15, 29); // Right
    enableAndClear();
    vertical_bar(16, 0);
    enableAndClear();
    vertical_bar(17, 1);
    enableAndClear();
    vertical_bar(18, 2);
    enableAndClear();
    vertical_bar(19, 3);
    enableAndClear();
    vertical_bar(20, 4);
    enableAndClear();
    vertical_bar(21, 5);
    enableAndClear();
    vertical_bar(22, 6);
    enableAndClear();
    vertical_bar(23, 7);
    enableAndClear();
    vertical_bar(24, 8);
    enableAndClear();
    vertical_bar(25, 9);
    enableAndClear();
    vertical_bar(26, 10);
    enableAndClear();
    vertical_bar(27, 11);
    enableAndClear();
    vertical_bar(28, 12);
    enableAndClear();
    vertical_bar(29, 13); // Far Right
    enableAndClear();
    _delay_ms(250);

    // Range of vertical bars from right to left:
    vertical_bar(29, 13); // Far Right
    enableAndClear();
    vertical_bar(28, 12);
    enableAndClear();
    vertical_bar(27, 11);
    enableAndClear();
    vertical_bar(26, 10);
    enableAndClear();
    vertical_bar(25, 9);
    enableAndClear();
    vertical_bar(24, 8);
    enableAndClear();
    vertical_bar(23, 7);
    enableAndClear();
    vertical_bar(22, 6);
    enableAndClear();
    vertical_bar(21, 5);
    enableAndClear();
    vertical_bar(20, 4);
    enableAndClear();
    vertical_bar(19, 3);
    enableAndClear();
    vertical_bar(18, 2);
    enableAndClear();
    vertical_bar(17, 1);
    enableAndClear();
    vertical_bar(16, 0);
    enableAndClear();
    vertical_bar(15, 29); // Right
    enableAndClear();
    vertical_bar(14, 28); // Middle
    enableAndClear();
    vertical_bar(13, 27); // Left
    enableAndClear();
    vertical_bar(12, 26);
    enableAndClear();
    vertical_bar(11, 25);
    enableAndClear();
    vertical_bar(10, 24);
    enableAndClear();
    vertical_bar(9, 23);
    enableAndClear();
    vertical_bar(8, 22);
    enableAndClear();
    vertical_bar(7, 21);
    enableAndClear();
    vertical_bar(6, 20);
    enableAndClear();
    vertical_bar(5, 19);
    enableAndClear();
    vertical_bar(4, 18);
    enableAndClear();
    vertical_bar(3, 17);
    enableAndClear();
    vertical_bar(2, 16);
    enableAndClear();
    vertical_bar(1, 15);
    enableAndClear();
    vertical_bar(0, 14); // Far Left
    enableAndClear();
}

void enableAndClear() {
    SPI_write(0, 0x08); // Enable Display
    _delay_ms(100);
    SPI_write(0x04, 0x06); // Clear Display on /VSync
    _delay_ms(30);
}

void SPI_write(char address, char byte) {
    unsigned char SPICount; // Counter used to clock out the data
    unsigned char SPIData; // Define a data structure for the SPI data
    PORTB |= (1<<CS); // Make sure we start with active-low CS high
    PORTB &= ~(1<<SCK); // and CK low
    SPIData = address; // Preload the data to be sent with Address
    PORTB &= ~(1<<CS); // Set active-low CS low to start the SPI cycle

    // Prepare to clock out address 
    for (SPICount = 0; SPICount < 8; SPICount++) {
        if (SPIData & 0x80) PORTB |= (1<<MOSI); // Check 1 and set MOSI line
        else PORTB &= ~(1<<MOSI);
        PORTB |= (1<<SCK); // Toggle the clock line
        PORTB &= ~(1<<SCK);
        SPIData <<= 1; // Rotate to get the next bit
                       // and loop back to send the next bit
    }

    // Repeat for the Data byte
    SPIData = byte; // Preload the data to be sent with Data
    for (SPICount = 0; SPICount < 8; SPICount++) {
        if (SPIData & 0x80) PORTB |= (1<<MOSI);
        else PORTB &= ~(1<<MOSI);
        
        PORTB |= (1<<SCK);
        PORTB &= ~(1<<SCK);
        SPIData <<= 1;
    }

    PORTB |= (1<<CS);
    PORTB &= ~(1<<MOSI);
}

char SPI_read(char address) {
    unsigned char SPICount; // Counter used to clock out the data
    char SPIData;
    char temp;
    PORTB |= (1<<CS); // Make sure we start with active-low CS high
    PORTB &= ~(1<<SCK); // and CK low
    SPIData = address; // Preload the data to be sent with Address
    PORTB &= ~(1<<CS); // Set active-low CS low to start the SPI cycle

    // Prepare to clock out address 
    for (SPICount = 0; SPICount < 8; SPICount++) {
        if (SPIData & 0x80) PORTB |= (1<<MOSI); // Check 1 and set MOSI line
        else PORTB &= ~(1<<MOSI);
        PORTB |= (1<<SCK); // Toggle the clock line
        PORTB &= ~(1<<SCK); // Rotate to get the next bit
        SPIData <<= 1; // and loop back to send the next bit
    }

    PORTB &= ~(1<<MOSI);
    SPIData = 0;

    // Prepare to clock in data
    for (SPICount = 0; SPICount < 8; SPICount++) {
        SPIData <<=1; // Rotate the data
        PORTB |= (1<<SCK); // Raise clock to clock data out of the MAX7456
        temp = PINB;
        if (temp & (1<<MISO)) SPIData |= 1; // Read the data bit

        PORTB &= ~(1<<SCK);
    }

    PORTB |= (1<<CS);

    return SPIData;
}

void Initialize_ADC0(void) {
    ADCSRA = 0x87; // 10000111: pre-scaler set to CLK/128 = 125kHz
    ADCSRB = 0x00; // Turn gain and auto-trigger off
    ADMUX = 0x00; // Set ADC channel ADC1 with 1x gain
}

void Initialize_ADC1(void) {
    ADCSRA = 0x87; // 10000111: pre-scaler set to CLK/128 = 125kHz
    ADCSRB = 0x00; // Turn gain and auto-trigger off
    ADMUX = 0x01; // Set ADC channel ADC1 with 1x gain
}

int GetSound(int mic, int samples) {
    int i = 0;
    long sum = 0;
    for (i = 0; i < samples; i++) { // Take 1000 samples of the sound
        if (mic == 1) // Select analog input channel
            ADMUX = 0x01;
        else
            ADMUX = 0x00;
        ADCSRA = 0xC7; // 11000111: pre-scalar set to CLK/128 = 125kHZ
        _delay_us(260);
        sum += ADC & 0x3FF; // Sum of the noise is turned into a long value 
    }

    return (int)(sum/i); // Return the average of 100 samples
}




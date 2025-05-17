#include "mbed.h"

// Define shield connections (NUCLEO-F401RE pins)
DigitalOut latch(PB_5);    // D4 -> LCHCLK (Latch)
DigitalOut clk(PA_8);      // D7 -> SFTCLK (Clock)
DigitalOut data(PA_9);     // D8 -> SDI (Data)
DigitalIn  button1(PA_1);  // S1 -> A1 (Reset button, active low)
DigitalIn  button3(PB_0);  // S3 -> A3 (Mode button, active low)
AnalogIn   pot(PA_0);      // Potentiometer -> A0

// 7-seg encoding for digits 0-9 (active-low segments, common anode)
const uint8_t SEGMENT_MAP[10] = { 
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 
    0x92, 0x82, 0xF8, 0x80, 0x90 
};
// Digit select bytes for 4 digits (active low to enable one digit)
const uint8_t SEGMENT_SELECT[4] = { 0xF1, 0xF2, 0xF4, 0xF8 };

// Shared variables for time
volatile int total_seconds = 0;
volatile bool updateDisplay = true;
 
// Ticker to increment time every 1 second
Ticker second_tick;
void tick() {
    total_seconds++;
    if(total_seconds >= 6000) { // wrap at 99:59 for safety
        total_seconds = 0;
    }
}

// Function to output two bytes to the shift registers (segments + digit)
void outputToDisplay(uint8_t segments, uint8_t digitSelect) {
    latch = 0;
    // Shift out 8 bits of segment data (MSB first)
    for(int i = 7; i >= 0; --i) {
        data = (segments >> i) & 0x1;
        clk = 0;
        clk = 1;
    }
    // Shift out 8 bits of digit select data
    for(int i = 7; i >= 0; --i) {
        data = (digitSelect >> i) & 0x1;
        clk = 0;
        clk = 1;
    }
    latch = 1;
}

// ISR ticker for display multiplex (called e.g. every 2 ms)
Ticker refresh_tick;
volatile int currDigit = 0;
void refreshISR() {
    updateDisplay = true;  // flag main loop to update (or do work here if non-RTOS)
}
 
int main() {
    // Initializations
    button1.mode(PullUp);
    button3.mode(PullUp);
    second_tick.attach(&tick, 1.0);            // tick every 1 second
    refresh_tick.attach(&refreshISR, 0.002);   // 2 ms refresh interrupt

    // Pre-calculate blank pattern for safety
    const uint8_t BLANK = 0xFF; // (all segments off)
 
    bool modeVoltage = false;
    int prev_b1 = 1, prev_b3 = 1;
 
    while (true) {
        // Check S1 (reset button)
        int b1 = button1.read();
        if(b1 == 0 && prev_b1 == 1) {  // falling edge
            total_seconds = 0;  // reset time
        }
        prev_b1 = b1;
 
        // Check S3 (mode switch)
        int b3 = button3.read();
        // If pressed, enter voltage display mode; if released, go back to time
        modeVoltage = (b3 == 0);
        prev_b3 = b3;
 
        // When refresh tick triggers, update the display
      if(updateDisplay) {
    updateDisplay = false;

    uint8_t segByte = 0xFF, selByte = 0xFF;

    if (!modeVoltage) {
        // MM:SS mode
        int seconds = total_seconds % 60;
        int minutes = total_seconds / 60;

        switch(currDigit) {
            case 0: segByte = SEGMENT_MAP[minutes / 10]; selByte = SEGMENT_SELECT[0]; break;
            case 1: segByte = SEGMENT_MAP[minutes % 10] & 0x7F; selByte = SEGMENT_SELECT[1]; break;
            case 2: segByte = SEGMENT_MAP[seconds / 10]; selByte = SEGMENT_SELECT[2]; break;
            case 3: segByte = SEGMENT_MAP[seconds % 10]; selByte = SEGMENT_SELECT[3]; break;
        }
    } else {
        // Voltage mode
        float volts = pot.read() * 3.3f;
        int centivolts = (int)(volts * 100.0f);
        if(centivolts > 999) centivolts = 999;
        int intPart = centivolts / 100;
        int fracPart = centivolts % 100;

        switch(currDigit) {
            case 0: segByte = SEGMENT_MAP[intPart] & 0x7F; selByte = SEGMENT_SELECT[0]; break;
            case 1: segByte = SEGMENT_MAP[fracPart / 10]; selByte = SEGMENT_SELECT[1]; break;
            case 2: segByte = SEGMENT_MAP[fracPart % 10]; selByte = SEGMENT_SELECT[2]; break;
            case 3: segByte = 0xFF; selByte = SEGMENT_SELECT[3]; break; // blank
        }
    }

    outputToDisplay(segByte, selByte);
    currDigit = (currDigit + 1) % 4;  // Cycle to next digit
}
    // (In a low-power or RTOS scenario, consider sleeping the thread here briefly)
    }
}
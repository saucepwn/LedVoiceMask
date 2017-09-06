#include <Adafruit_NeoMatrix.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            2

// How many NeoPixels are attached to the Arduino?
#define ROWS           8
#define COLS           8
#define BRIGHTNESS     25

// Values range from [0 - 255]
#define COLOR_RED      255
#define COLOR_BLUE     0
#define COLOR_GREEN    0

// Tweak the algorithm
#define STAY_PROBABILITY  0.2  // Must be betweeen 0.0 and 1.0
#define DELAY             85   // The delay, in MS, between scren refreshes

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  ROWS, COLS, PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

uint8_t pixelState[COLS];
const uint16_t color = matrix.Color(COLOR_RED, COLOR_GREEN, COLOR_BLUE);

void setup() {
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  Serial.begin(9600);
  Serial.println("Ready for business!");

  clearPixels();
}

void loop() {
  for (uint8_t i = 0; i < COLS; i++) {
    pixelState[i] = calcLitPixels(pixelState[i]);
    setLitPixelsByRow(i, pixelState[i]);
  }
  
  matrix.show();
  delay(DELAY);
}

void setLitPixelsByRow(uint8_t row, uint8_t num_pixels) {
  for (uint8_t i = 0; i < COLS; i++) {
    if (i < num_pixels) {
      // Turn pixel on
      matrix.drawPixel(row, i, color);
    } else {
      // Turn pixel off
      matrix.drawPixel(row, i, 0);
    }
  }
}

void clearPixels() {
  for (uint8_t i = 0; i < COLS; i++) {
    pixelState[i] = 0;
  }

  matrix.fillScreen(0);
  matrix.show();
}

uint8_t calcLitPixels(uint8_t leds_lit) {
  // Calculate down probability
  float down_probability = (1 - STAY_PROBABILITY) * leds_lit * 0.125;

  // Calculate up probablitly
  float up_probability = 1 - STAY_PROBABILITY - down_probability;

  int lower_bound_stay = 0;
  int upper_bound_stay = STAY_PROBABILITY * 100;

  int lower_bound_up = upper_bound_stay;
  int upper_bound_up = upper_bound_stay + (up_probability * 100);

  int lower_bound_down = upper_bound_up;
  int upper_bound_down = upper_bound_up + (down_probability * 100);
  
  // Get a random number
  int random_upper_bound = (down_probability + up_probability + STAY_PROBABILITY) * 100;
  int r = random(random_upper_bound);
  
  if (r >= lower_bound_stay && r < upper_bound_stay) {
    return leds_lit;
  } else if (r >= lower_bound_up && r < upper_bound_up) {
    return leds_lit + 1;
  } else if (r >= lower_bound_down && r < upper_bound_down) {
    return leds_lit - 1;
  } else {
    return leds_lit;
  }
}

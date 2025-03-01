/* Edge Impulse inference code with optimized LED effects */
#include <newDancingshoe_inferencing.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <Adafruit_NeoPixel.h>

#define EI_CLASSIFIER_SENSOR EI_CLASSIFIER_SENSOR_ACCELEROMETER
#define CONFIDENCE_THRESHOLD 0.8

/* Constants ---------------------------------------------------------------- */
#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))
#define ACC_RANGE           1
#define CONVERT_G_TO_MS2    (9.81/(16384.0/(1.+ACC_RANGE)))

/* LED Settings ------------------------------------------------------------ */
#define LED_PIN         9
#define LED_COUNT       91
#define FRAME_RATE      30      // Hz
#define FRAME_DELAY     (1000 / FRAME_RATE)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/* IMU Setup --------------------------------------------------------------- */
MPU6050 imu;
int16_t ax, ay, az;

/* Effect Variables -------------------------------------------------------- */
unsigned long lastFrame = 0;
String currentEffect = "";
float cometPosition = 0.0;
uint16_t hueOffset = 0;
uint8_t breathBrightness = 0;
int8_t breathDirection = 1;
float pulseBrightness = 0;
bool anomalyState = false;

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // LED Initialization
    strip.begin();
    strip.show();
    strip.setBrightness(30);

    // IMU Initialization
    Wire.begin();
    imu.initialize();
    delay(10);
    
    // IMU Calibration
    imu.setXAccelOffset(-652);
    imu.setYAccelOffset(1274);
    imu.setZAccelOffset(2512);
    imu.setXGyroOffset(31);
    imu.setYGyroOffset(-31);
    imu.setZGyroOffset(5);
    imu.setFullScaleAccelRange(ACC_RANGE);
    
    if (EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME != 3) {
        ei_printf("ERR: EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME should be 3\n");
    }
}

/* Optimized LED Effects --------------------------------------------------- */
void rainbowWave() {
    const float speed = 0.8;
    hueOffset += speed * 256;
    for(int i=0; i<LED_COUNT; i++) {
        uint16_t hue = (i * 65536L / LED_COUNT) + hueOffset;
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, 128));
    }
}

void walkingComet() {
    const float speed = 1.2;
    const int cometLength = 12;
    
    // Fade all pixels
    for(int i=0; i<LED_COUNT; i++) {
        uint32_t color = strip.getPixelColor(i);
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        strip.setPixelColor(i, r * 0.92, g * 0.92, b * 0.92);
    }
    
    // Add new comet head
    int pos = (int)cometPosition;
    for(int i=0; i<cometLength; i++) {
        float intensity = 1.0 - (float)i/cometLength;
        int led = (pos - i + LED_COUNT) % LED_COUNT;
        strip.setPixelColor(led, strip.Color(
            0, 
            intensity * 255, 
            intensity * 255
        ));
    }
    
    cometPosition += speed;
    if(cometPosition >= LED_COUNT) cometPosition = 0;
}

void gentlePulse() {
    const float decay = 0.85;
    pulseBrightness = (pulseBrightness < 10) ? 255 : pulseBrightness * decay;
    uint8_t bright = (uint8_t)pulseBrightness;
    strip.fill(strip.Color(bright, bright, bright));
}

void smoothSwipe() {
    static float position = 0;
    const float speed = 1.5;
    
    position += speed;
    if(position > LED_COUNT * 2) position = 0;
    
    for(int i=0; i<LED_COUNT; i++) {
        float dist = fabs(i - position);
        if(dist > LED_COUNT) dist = LED_COUNT * 2 - dist;
        
        uint8_t green = constrain(255 - (dist * 4), 0, 255);
        uint8_t blue = constrain(255 - ((dist - LED_COUNT/2) * 4), 0, 255);
        strip.setPixelColor(i, strip.Color(0, green, blue));
    }
}

void rotatingGradient() {
    const uint16_t speed = 200;
    hueOffset += speed;
    for(int i=0; i<LED_COUNT; i++) {
        uint16_t hue = (i * 65536L / LED_COUNT) + hueOffset;
        strip.setPixelColor(i, strip.ColorHSV(hue, 255, 128));
    }
}

void breathingGlow() {
    breathBrightness += breathDirection * 2;
    if(breathBrightness >= 100 || breathBrightness <= 0) {
        breathDirection = -breathDirection;
    }
    strip.fill(strip.Color(breathBrightness + 155, breathBrightness + 155, 0));
}

void dualComets() {
    const float speed = 1.5;
    static float pos1 = 0, pos2 = LED_COUNT/2;
    
    // Fade all pixels
    for(int i=0; i<LED_COUNT; i++) {
        strip.setPixelColor(i, strip.Color(
            (strip.getPixelColor(i) >> 16) & 0xFF,
            ((strip.getPixelColor(i) >> 8) & 0xFF) * 0.9,
            (strip.getPixelColor(i) & 0xFF) * 0.9
        ));
    }
    
    // Update comet positions
    pos1 += speed;
    pos2 += speed;
    if(pos1 >= LED_COUNT) pos1 = 0;
    if(pos2 >= LED_COUNT) pos2 = 0;
    
    // Draw comets
    for(int i=0; i<8; i++) {
        float intensity = 1.0 - (float)i/8;
        strip.setPixelColor((int)pos1 + i, strip.Color(0, intensity * 255, intensity * 255));
        strip.setPixelColor((int)pos2 + i, strip.Color(0, intensity * 255, intensity * 255));
    }
}

void anomalyPulse() {
    static uint8_t brightness = 0;
    static int8_t direction = 1;
    
    brightness += direction * 5;
    if(brightness >= 255 || brightness <= 0) direction = -direction;
    strip.fill(strip.Color(brightness, 0, 0));
}

/* Inference Handling ------------------------------------------------------ */
void runInference() {
    static unsigned long lastInference = 0;
    if(millis() - lastInference < 800) return;
    lastInference = millis();

    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {0};
    
    for(size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 3) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);
        imu.getAcceleration(&ax, &ay, &az);
        
        buffer[ix] = ax * CONVERT_G_TO_MS2;
        buffer[ix + 1] = ay * CONVERT_G_TO_MS2;
        buffer[ix + 2] = az * CONVERT_G_TO_MS2;
        
        delayMicroseconds(next_tick - micros());
    }

    signal_t signal;
    numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    
    ei_impulse_result_t result = {0};
    run_classifier(&signal, &result);

    // Handle results
    bool anomaly = result.anomaly > 0.8;
    if(anomaly) {
        currentEffect = "anomaly";
    } else {
        float maxVal = 0;
        int maxIdx = -1;
        for(int i=0; i<EI_CLASSIFIER_LABEL_COUNT; i++) {
            if(result.classification[i].value > maxVal) {
                maxVal = result.classification[i].value;
                maxIdx = i;
            }
        }
        
        if(maxVal > CONFIDENCE_THRESHOLD) {
            currentEffect = result.classification[maxIdx].label;
        } else {
            currentEffect = "idle";
        }
    }
}

void loop() {
    // Frame timing
    if(millis() - lastFrame < FRAME_DELAY) return;
    lastFrame = millis();
    
    runInference();
    
    // Update LEDs based on current effect
    if(currentEffect == "hinchipinichi") {
        rainbowWave();
    } 
    else if(currentEffect == "walk") {
        walkingComet();
    }
    else if(currentEffect == "jump") {
        gentlePulse();
    }
    else if(currentEffect == "left-right") {
        smoothSwipe();
    }
    else if(currentEffect == "round-r") {
        rotatingGradient();
    }
    else if(currentEffect == "stand") {
        breathingGlow();
    }
    else if(currentEffect == "up-down") {
        dualComets();
    }
    else if(currentEffect == "anomaly") {
        anomalyPulse();
    }
    else {
        strip.fill(0);
    }
    
    strip.show();
}
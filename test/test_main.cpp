#include <Arduino.h>
#include <unity.h>
#include "calculate.h"


Calculate calculate;

void test_mqtt0(void){
    TEST_ASSERT_EQUAL(0,DD0); // check digital input dd0
}

void test_mqtt_state_high(void){
    digitalWrite(DD0, HIGH);
    TEST_ASSERT_EQUAL(HIGH, digitalRead(DD0));
}

void test_mqtt_state_low(void){
    digitalWrite(DD0, LOW);
    TEST_ASSERT_EQUAL(HIGH, digitalRead(DD0));
}

void test_led_builtin_pin_number(void) {
    TEST_ASSERT_EQUAL(13, LED_BUILTIN);
}

void test_led_state_high(void) {
    digitalWrite(LED_BUILTIN, HIGH);
    TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_BUILTIN));
}

void test_led_state_low(void) {
    digitalWrite(LED_BUILTIN, LOW);
    TEST_ASSERT_EQUAL(LOW, digitalRead(LED_BUILTIN));
}

void test_function_calculate_addition(void) {
    TEST_ASSERT_EQUAL(32, calculate.add(25, 7));
}

void test_function_calculate_subtraction(void) {
    TEST_ASSERT_EQUAL(20, calculate.sub(23, 3));
}

void test_function_calculate_multiplication(void) {
    TEST_ASSERT_EQUAL(50, calculate.mul(25, 2));
}

void test_function_calculate_division(void) {
    TEST_ASSERT_EQUAL(32, calculate.div(96, 3));
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_mqtt0);
    RUN_TEST(test_led_builtin_pin_number);
    

    pinMode(LED_BUILTIN, OUTPUT);
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop() {
    if (i < max_blinks)
    {
        RUN_TEST(test_led_state_high);
        delay(500);
        RUN_TEST(test_led_state_low);
        delay(500);
        RUN_TEST(test_mqtt_state_high);
        delay(500);
        RUN_TEST(test_mqtt_state_low);
        delay(500);

        RUN_TEST(test_function_calculate_addition);
        delay(500);
        RUN_TEST(test_function_calculate_subtraction);
        delay(500);
        RUN_TEST(test_function_calculate_multiplication);
        delay(500);
        RUN_TEST(test_function_calculate_division);
        delay(500);
        i++;
    }
    else if (i == max_blinks) {
      UNITY_END(); // stop unit testing
    }
}
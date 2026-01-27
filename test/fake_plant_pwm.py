import RPi.GPIO as GPIO
import time

PWM_PIN = 18      # GPIO18 (pin 12)
PWM_FREQ = 50     # 50 Hz (20 ms period)

GPIO.setmode(GPIO.BCM)
GPIO.setup(PWM_PIN, GPIO.OUT)

pwm = GPIO.PWM(PWM_PIN, PWM_FREQ)

# Start at 1500 µs pulse width
# Duty cycle = (pulse / period) * 100
duty_1500 = (1.5 / 20.0) * 100
duty_4000 = (4.0 / 20.0) * 100

print("Starting PWM at 1500 us")
pwm.start(duty_1500)

time.sleep(5)

print("Changing PWM to 4000 us")
pwm.ChangeDutyCycle(duty_4000)

print("Holding PWM steady...")
while True:
    time.sleep(1)


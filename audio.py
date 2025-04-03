import serial
import pygame

# Setup serial connection
ser = serial.Serial("COM6", 9600, timeout=1)

# Initialize pygame.
pygame.init()
pygame.mixer.init()

# Load audio files
pygame.mixer.music.load("fever.wav")
alert_sound = pygame.mixer.Sound("spo2-bpm.wav")

while True:
    # Check serial input
    if ser.in_waiting:
        message = ser.readline().decode().strip()
        if message == "ALERT":
            pygame.mixer.music.play()
        elif message == "ALERT1":
            alert_sound.play()
# Inclinometer

- Digitaler 0-360° Neigungsmesser/Winkelmesser
- Laufzeit: ca. 15h
- Ladezeit: 2,5h (Über USB)

# Hardware



## USB-Kommunikation
USB --> (VCP) VirtualComPort

# Software

## Hardwaretreiber

### Bosch Sensor

2 x BMI160

### OLED Display

SSD1306

# Kalibrierung

## Drehraten
- Winkelmesser auf den Tisch legen, ohne das er bewegt oder gedreht wird.
- Die MEsswerte über VCP auslesen
- Diese Werte sind gleichzeitig Bias

## Beschleunigung

- Winkelmesser einfach für jede Achse ruhig auf den Tisch legen (Sowohl für pos. auls auch für neg. Richtung)
- Werte über VCP auslesen
- Der Wer der jeweiligen Achse zeigt den BIAS-Wert

- Bedingt durch ein nciht ebenes Gehäuse, nicht genaue positionierung beim Lötprozess des Sensors und einbau der Platine im Gehäuse ergibt sich eine leichte Abweichung. In der Regel ist diese Abweichung konstant.



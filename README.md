# STM32 Multi-Sensor Datenlogger

## Overview

Ein einfacher Datenlogger mit einem STM32 .  
Das System misst Temperatur, Luftfeuchtigkeit und Luftdruck, speichert die Daten mit Zeitstempel und legt sie auf einer SD-Karte ab.

Über die serielle Schnittstelle kann man Einstellungen während des Betriebs ändern.

## Technologies

* STM32F103C8T6 (Blue Pill)
* BME280
* DS3231 (RTC)
* MicroSD Card (SPI)
* C / STM32 HAL
* FATFS

## Features

* Misst Temperatur, Luftfeuchtigkeit und Druck  
* Zeitstempel über RTC  
* Speichert Daten als CSV auf SD-Karte  
* Steuerung über UART (Terminal)  
* Logging per Timer (Interrupt)  
* Einfach aufgebaute, modulare Struktur  

## Setup

```text
STM32    | BME280/DS3231 | SD Card     | UART
---------|---------------|-------------|--------
PB6 (SCL)| SCL           | -           | -
PB7 (SDA)| SDA           | -           | -
PA4      | -             | CS          | -
PA5      | -             | SCK         | -
PA6      | -             | MISO        | -
PA7      | -             | MOSI        | -
PA9      | -             | -           | TX
PA10     | -             | -           | RX





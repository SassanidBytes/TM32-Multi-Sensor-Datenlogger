# STM32 Multi-Sensor Datenlogger

## Overview

Entwicklung eines modularen Embedded-Datenloggers auf Basis eines STM32-Mikrocontrollers zur kontinuierlichen Erfassung und Speicherung von Umweltdaten (Temperatur, Luftfeuchtigkeit, Luftdruck).
Das System integriert mehrere Sensoren, ein Echtzeitmodul sowie ein Dateisystem und ermöglicht eine flexible Konfiguration zur Laufzeit über eine serielle Schnittstelle.

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

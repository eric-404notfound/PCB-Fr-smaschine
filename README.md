# PCB-Fräsmaschine

Eine kompakte, selbstgebaute CNC-Fräsmaschine zur Herstellung von Leiterplatten (PCBs).

## Merkmale

- Steuerung mit Raspberry Pi Pico (RP2040)
- G-Code-Interpretation (nur lineare Befehle)
- Steppertreiber (TMC2209)
- Ziel: präzises Isolationsfräsen von Platinen
- Eigenes Steuerprotokoll und einfache Befehlsschnittstelle

## Hardware

- Raspberry Pi Pico  
- 3x NEMA17 Schrittmotoren  
- 3x A4988 / TMC-Treiber  
- 12–24 V Netzteil  
- Linearführungen, Spindelhalter, Grundplatte (z. B. Alu/Multiplex)

## Software

- Steuerlogik in C++ (Pico SDK)
- Feedrate-, Schrittweitensteuerung
- Unterstützung für einfache Fräs-Commands

## Status

🚧 In Entwicklung – Software und Mechanik werden laufend verbessert.

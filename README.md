# USB-TTL Auto Baud Rate Detection Sniffer

ESP32-C6 projekt pre automatickú detekciu baud rate a monitorovanie USB-TTL komunikácie s SD kartou a displejom.

## Funkcie

- ✅ Automatická detekcia baud rate (9600-115200 bps)
- ✅ Sledovanie RX/TX komunikácie v reálnom čase
- ✅ Ovládanie cez TFT displej
- ✅ Logovanie všetkých dát na SD kartu
- ✅ Testovanie komunikácie s cieľovým zariadením
- ✅ Multi-state machine pre správne správanie

## Hardvérové požadavky

- ESP32-C6 DevKit
- TFT display (ILI9341)
- SD kartová slot
- RX/TX pieskovisko

## Konfigurácia

### Hardvérové pripojenie

```
ESP32-C6 Pin   →   Periféria
-----------------------------------
GPIO4          →   RX (do cieľového zariadenia)
GPIO5          →   TX (do cieľového zariadenia)
GPIO8          →   TFT DC
GPIO9          →   SD Card CS
GPIO10         →   TFT CS
GPIO11         →   TFT MOSI
GPIO12         →   TFT SCK
GPIO13         →   TFT MISO
```

### Software konfigurácia

Použite PlatformIO:

```bash
# Nainštalujte závislosti
pio lib install

# Zostavte projekt
pio run

# Nahrajte na ESP32-C6
pio run --target upload
```

## Stavy systému

1. **BOOTING** - Systém sa inicializuje
2. **WAITING** - Čaká na komunikáciu z cieľového zariadenia
3. **ANALYZING** - Detekuje baud rate
4. **FOUND_SPEED** - Našiel baud rate
5. **TESTING** - Testuje komunikáciu s cieľovým zariadením
6. **RESTART_NEEDED** - Vyžaduje reštart cieľového zariadenia
7. **RUNNING** - Monitruje aktívnu komunikáciu

## Použitie

1. Zapnite ESP32-C6
2. Na displeji uvidíte aktuálny stav
3. Pripojte cieľové zariadenie na RX/TX
4. Systém automaticky deteguje baud rate
5. Všetky dáta sa ukladajú na SD kartu
6. Na displeji vidíte priebežný stav

## Logovanie

Všetky správy a dáta sa ukladajú na SD kartu v súbore:
`LOG_[timestamp].txt`

Formát logu:
```
[timestamp] [counter] MESSAGE
[timestamp] [counter] RX DATA: ...
[timestamp] [counter] BAUD DETECTED: 115200 bps
```

## Konfigurácia v config.h

```cpp
#define RX_PIN 4
#define TX_PIN 5
#define SD_CS 9
#define TFT_CS 10
// ... viac konfigurácie
```

## Príklady použitia

### Testovanie komunikácie

Systém pošle testovacie hlásenie "AT\r\n" a očakáva odpoveď.

### Monitorovanie dát

Všetky RX dáta sa zobrazujú na displeji a ukladajú na SD kartu.

## Podpora

Pre otázky a problémy, pozrite sa na dokumentáciu v PDF súbore.

## Licencia

MIT License


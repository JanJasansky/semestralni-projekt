# BAC Calculator

Tento projekt je semestrální projekt vytvořený v C++ s využitím knihovny Qt. Jedná se o aplikaci pro výpočet hladiny alkoholu v krvi (BAC - Blood Alcohol Content). Aplikace poskytuje uživatelsky přívětivé rozhraní pro zadávání údajů a zobrazení výsledků.

## ✨ Hlavní vlastnosti

- **Grafické uživatelské rozhraní** vytvořené pomocí Qt
- **Výpočet BAC** na základě zadaných údajů (hmotnost, pohlaví, množství alkoholu, čas od posledního pití)
- **Vizualizace dat** pomocí Python skriptu (grafy)
- **Podpora multiplatformnosti** (macOS, Linux)
- **Uživatelsky přívětivé rozhraní** s moderním designem

## 📦 Instalace a spuštění

### Požadavky:

- **CMake**: Verze 3.10 nebo novější
- **Qt**: Verze 5.15 nebo novější
- **Python**: Verze 3.8 nebo novější (pro generování grafů)
- **Knihovny Pythonu**: Matplotlib

### Rychlá kompilace (macOS/Linux):

1. Stáhněte si tento repozitář a rozbalte ho do požadované složky.
2. Otevřete terminál a přejděte do složky projektu:
   ```bash
   cd /cesta/k/semestralni-projekt
   ```
3. Vytvořte složku pro build a přejděte do ní:
   ```bash
   mkdir build && cd build
   ```
4. Spusťte CMake pro vygenerování build souborů:
   ```bash
   cmake ..
   ```
5. Sestavte projekt:
   ```bash
   cmake --build .
   ```
6. Spusťte aplikaci:
   ```bash
   ./BACCalculator
   ```

### Alternativní způsob spuštění (Python grafy):

1. Spusťte Python skript pro generování grafů:
   ```bash
   python python/plot_bac.py
   ```

## 📚 Použité knihovny

Projekt využívá následující knihovny a závislosti:

- **Qt**: Pro grafické uživatelské rozhraní
- **Matplotlib**: Pro vizualizaci dat (Python)

## 🚧 Plánované rozšíření

- Přidání dalších vizualizací a statistik
- Podpora dalších operačních systémů
- Rozšíření funkcionality o další výpočty

## 🏗️ Struktura projektu

- **src/**: Zdrojové kódy aplikace (hlavní logika, GUI)
- **python/**: Python skripty pro vizualizaci dat
- **icons/**: Ikony používané v aplikaci
- **build/**: Složka pro build soubory
- **CMakeLists.txt**: Konfigurační soubor pro CMake

Užijte si používání aplikace BAC Calculator!
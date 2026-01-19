# AlcoholCalc (C++)

Konzolova aplikace pro odhad hladiny alkoholu v krvi (BAC, promile) v case.
Uzivatel zada vahu/vysku, pohlavi a konkretni piti vcetne casu od-do. Program:
- spocita orientacni krivku promile po minutach
- najde maximum (peak)
- odhadne cas poklesu pod 0.5 ‰, 0.2 ‰ a 0.0 ‰
- vygeneruje CSV soubor pro vykresleni grafu

## Build (CMake)
```bash
mkdir build
cd build
cmake ..
cmake --build .
./AlcoholCalc

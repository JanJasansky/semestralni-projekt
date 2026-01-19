# AlcoholCalc (C++)

Konzolová aplikace pro odhad hladiny alkoholu v krvi (BAC, promile) v čase. 
Uživatel zadává své osobní údaje (váhu, výšku, pohlaví) a informace o vypitých nápojích, včetně času konzumace. Program následně:

- Spočítá orientační křivku promile alkoholu v krvi po minutách.
- Najde maximální hodnotu promile (peak) a čas, kdy k ní došlo.
- Odhadne časy, kdy hladina alkoholu klesne pod 0.5 ‰, 0.2 ‰ a 0.0 ‰.
- Vygeneruje CSV soubor, který lze použít pro vykreslení grafu v externích nástrojích (např. Excel).

## Funkce programu

1. **Zadání údajů uživatele:**
   - Pohlaví (muž/žena).
   - Váha v kilogramech.
   - Výška v centimetrech.

2. **Zadání nápojů:**
   - Typ nápoje (např. pivo, víno, rum nebo vlastní nápoj).
   - Objem nápoje v mililitrech.
   - Obsah alkoholu v procentech.
   - Čas, od kdy do kdy byl nápoj konzumován.

3. **Výpočet a výstup:**
   - Zobrazení křivky hladiny alkoholu v krvi v čase (ASCII graf).
   - Výpis klíčových informací, jako je maximální promile a časy vystřízlivění.
   - Možnost exportu výsledků do CSV souboru.

## Sestavení a spuštění (CMake)

Pro sestavení a spuštění aplikace postupujte podle následujících kroků:

```bash
# Vytvoření adresáře pro build
mkdir build
cd build

# Generování build souborů pomocí CMake
cmake ..

# Sestavení projektu
cmake --build .

# Spuštění aplikace
./AlcoholCalc
```

## Požadavky
- C++ kompilátor s podporou standardu C++17.
- CMake (minimální verze 3.16).

## Poznámky
- Výsledky výpočtů jsou pouze orientační a neměly by být používány jako právně závazné informace.
- Program je určen pro vzdělávací účely a demonstraci výpočtů hladiny alkoholu v krvi.

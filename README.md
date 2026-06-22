# Sachy

Konzolová (textová) šachová hra napísaná v jazyku C pre dvoch hráčov, hraná v jednom termináli na striedačku. Projekt je nastavený ako CMake projekt (vygenerovaný Visual Studiom).

## Funkcie

- Klasická šachovnica 8×8 s počiatočným rozostavením figúrok
- Hra pre dvoch hráčov – zadanie mena (nickname) a výber farby (biely/čierny)
- Zápis ťahov v šachovej notácii polí (napr. `E2` → `E4`)
- Zvýraznenie možných ťahov vybranej figúrky priamo na šachovnici (zelenou farbou)
- Farebné rozlíšenie bielych a čiernych figúrok v konzole
- Kontrola platnosti ťahu pre každý typ figúrky (pešiak, vežia, strelec, jazdec, dáma, kráľ)
- Rošáda (kráľovská aj dámska) vrátane kontroly, či nie je kráľ v šachu alebo neprechádza cez ohrozené pole
- Detekcia šachu a šachmatu
- Uloženie a načítanie hry do/z textového súboru (`sachova_hra.txt`) – počas hry sa stačí zadať `save`

## Požiadavky

- Kompilátor s podporou C/C++ (testované s MSVC vo Visual Studiu)
- CMake 3.8 alebo novší
- Generátor Ninja (podľa priložených CMake presetov) alebo bežné Visual Studio riešenie

## Build a spustenie

### Visual Studio
1. Otvor priečinok `sachy` vo Visual Studiu (File → Open → Folder).
2. Visual Studio automaticky rozpozná CMake projekt a presety (`x64-debug`, `x64-release`, `x86-debug`, `x86-release`).
3. Skompiluj a spusti (F5 alebo Ctrl+F5).

### CMake z príkazového riadku
```bash
cd sachy
cmake --preset x64-release
cmake --build out/build/x64-release
```
Výsledný executable nájdeš v priečinku `out/build/x64-release`.

> Poznámka: program používa `system("cls")`/`system("clear")` na čistenie konzoly a farebné ANSI kódy, takže najlepšie funguje vo Windows Terminal, modernom `cmd`, alebo v termináloch na Linuxe/macOS s podporou ANSI escape sekvencií.

## Ako sa hrá

1. Pri spustení sa program spýta, či chceš načítať uloženú hru (`y`/`n`).
2. Ak hráš novú hru, zadáš nickname pre oboch hráčov a hráč 1 si vyberie farbu (hráč 2 dostane opačnú).
3. Na ťah zadávaš najprv pozíciu figúrky, ktorou chceš ťahať (napr. `E2`), program zobrazí všetky jej dostupné ťahy zvýraznené na šachovnici.
4. Následne zadáš cieľovú pozíciu (napr. `E4`).
5. Hra striedavo prepína hráčov, kontroluje šach a šachmat a v prípade šachmatu sa skončí.
6. Kedykoľvek počas zadávania ťahu môžeš napísať `save` – hra sa uloží do súboru `sachova_hra.txt` a program sa skončí.

## Štruktúra projektu

```
sachy/
├── CMakeLists.txt          # hlavný (top-level) CMake súbor
├── CMakePresets.json       # presety pre x64/x86 debug a release
└── sachy/
    ├── CMakeLists.txt      # CMake súbor pre executable
    ├── sachy.h             # hlavičkový súbor
    └── sachy.cpp           # hlavná logika hry
```

## Známe obmedzenia

- Chýba detekcia premeny pešiaka (promotion), braní mimochodom (en passant) a remízových pravidiel (napr. patová situácia, 50-ťahové pravidlo).
- Vstup je citlivý na formát – pozícia sa musí zadať ako písmeno + číslo (napr. `A1`–`H8`).

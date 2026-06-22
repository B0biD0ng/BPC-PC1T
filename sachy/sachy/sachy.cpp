// sachy.cpp : Defines the entry point for the application.
//

#include "sachy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>



#define BOARD_SIZE 8
#define RESET_COLOR "\033[0m"
#define WHITE_PIECE_COLOR "\033[97m"
#define BLACK_PIECE_COLOR "\033[90m"

#define MAX_NICKNAME_LENGTH 20

// Struktúra pre hráča
typedef struct {
    char meno[MAX_NICKNAME_LENGTH];
    int farba; // 1 - biely, 2 - čierny
} Hrac;

// Prototypy funkcií
void inicializujSachovnicu(char sachovnica[BOARD_SIZE][BOARD_SIZE]);
void zobrazSachovnicu(char sachovnica[BOARD_SIZE][BOARD_SIZE]);
void vycistiKonzolu();
int jeBielaFigurka(char figura);
int jeCiernaFigurka(char figura);
int jeValidnyPohyb(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS, int hrac);
int jeCestaPrazdna(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS);
void vykonajPohyb(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS);
void zadajNickname(Hrac* hrac, int cisloHrac);
void vyberFarbu(Hrac* hrac);
int nacitajHru(char sachovnica[BOARD_SIZE][BOARD_SIZE], int* naRade, Hrac hraci[2]);
void ulozHru(char sachovnica[BOARD_SIZE][BOARD_SIZE], int naRade,  Hrac hraci[2]);
bool jeSach(char sachovnica[BOARD_SIZE][BOARD_SIZE], int hrac);
bool jeSachmat(char sachovnica[BOARD_SIZE][BOARD_SIZE], int hrac);
int jeRosada(char sachovnica[BOARD_SIZE][BOARD_SIZE], int hrac, int startR, int startS, int cielR, int cielS);
int jeCestaRosadyVolna(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS);
void zobrazMoznePozicie(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int hrac);
int jeSachPozicia(char sachovnica[BOARD_SIZE][BOARD_SIZE], int r, int s, int hrac);
int maDostupnePohyby(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int hrac);
void zobrazSachovnicuSoZvyraznenim(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int hrac);

int main() {
    char sachovnica[BOARD_SIZE][BOARD_SIZE];
    Hrac hraci[2];
    int hrac = 0;  // Hráč 1 (index 0) začína
    setlocale(LC_ALL, "");
    int startR, startS, cielR, cielS;
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Vyčistenie zvyškového vstupu pred začiatkom hry
    }
    printf("Chcete načítať uloženu hru? (y/n): ");
    char vyber;
    scanf(" %c", &vyber);

    if (vyber == 'y' || vyber == 'Y') {
        if (!nacitajHru(sachovnica, &hrac, hraci)) {
            printf("Načítanie hry zlyhalo. Spúštam novú hru.\n");
            inicializujSachovnicu(sachovnica);

            // Zadať mená a farbu pre hráčov
            zadajNickname(&hraci[0], 1);
            zadajNickname(&hraci[1], 2);
            vyberFarbu(&hraci[0]); // Prvý hráč si vyberie farbu
        }
    }
    else {
        inicializujSachovnicu(sachovnica);

        // Zadať mená a farbu pre hráčov
        zadajNickname(&hraci[0], 1);
        zadajNickname(&hraci[1], 2);
        vyberFarbu(&hraci[0]); // Prvý hráč si vyberie farbu
        vycistiKonzolu();
        hraci[1].farba = (hraci[0].farba == 1) ? 2 : 1; // Druhý hráč dostane opačnú farbu
        
    }

    

    vycistiKonzolu();
    while (1) {
        // Zobrazenie šachovnice
        zobrazSachovnicu(sachovnica);
        printf("Na rade je hráč: %s (%s).\n", hraci[hrac].meno, hrac == 0 ? "biely" : "čierny");

        // Výber počiatočnej pozície figúrky
        while (1) {
            char vyberFigurku[10];
            printf("Zadajte pozíciu figurky (napr. E2): ");
            fgets(vyberFigurku, sizeof(vyberFigurku), stdin);

            if (strncmp(vyberFigurku, "save", 4) == 0) {
                ulozHru(sachovnica, hrac, hraci);
                printf("Hra bola uložená. Koniec programu.\n");
                return 0; 
            }

            if (strlen(vyberFigurku) >= 2 && isalpha(vyberFigurku[0]) && isdigit(vyberFigurku[1])) {
                startS = toupper(vyberFigurku[0]) - 'A';
                startR = 8 - (vyberFigurku[1] - '0');

                if (startR >= 0 && startR < BOARD_SIZE && startS >= 0 && startS < BOARD_SIZE &&
                    ((hrac == 0 && jeBielaFigurka(sachovnica[startR][startS])) ||
                        (hrac == 1 && jeCiernaFigurka(sachovnica[startR][startS])))) {
                    // Overenie, či má figúrka dostupné pohyby
                    if (!maDostupnePohyby(sachovnica, startR, startS, hrac + 1)) {
                        printf("Táto figúrka nemá žiadne dostupné pohyby! Zvoľte inú figúrku.\n");
                        continue; // Vráťte sa do výberu figúrky
                    }
                    break; // Figúrka má dostupné pohyby, pokračujte ďalej
                }
            }
            printf("Neplatná pozícia! Skúste znova.\n");
        }

        // Zobrazenie šachovnice so zvýraznenými možnými ťahmi
        vycistiKonzolu();
        zobrazSachovnicuSoZvyraznenim(sachovnica, startR, startS, hrac + 1);

        // Výber cieľovej pozície
        while (1) {
            char vyberCiel[10];
            printf("Zadajte cieľovú pozíciu (napr. E4): ");
            fgets(vyberCiel, sizeof(vyberCiel), stdin);

            if (strlen(vyberCiel) >= 2 && isalpha(vyberCiel[0]) && isdigit(vyberCiel[1])) {
                cielS = toupper(vyberCiel[0]) - 'A';
                cielR = 8 - (vyberCiel[1] - '0');
                // Overenie rošády
                if (jeRosada(sachovnica, hrac + 1, startR, startS, cielR, cielS)) {
                    printf("Hráč %s vykonal rošádu!\n", hraci[hrac].meno);
                    // Presun kráľa a veže pri rošáde
                    if (cielS == 6) { // Kráľovská rošáda
                        sachovnica[startR][startS] = '.';
                        sachovnica[startR][cielS] = (hrac == 0 ? 'K' : 'k');
                        sachovnica[startR][7] = '.';
                        sachovnica[startR][5] = (hrac == 0 ? 'R' : 'r');
                    }
                    else if (cielS == 2) { // Dámska rošáda
                        sachovnica[startR][startS] = '.';
                        sachovnica[startR][cielS] = (hrac == 0 ? 'K' : 'k');
                        sachovnica[startR][0] = '.';
                        sachovnica[startR][3] = (hrac == 0 ? 'R' : 'r');
                    }
                    hrac = 1 - hrac; // Zmena hráča
                    vycistiKonzolu();
                    break; // Prechod na ďalší ťah
                }
                // Normálny pohyb
                if (jeValidnyPohyb(sachovnica, startR, startS, cielR, cielS, hrac + 1)) {
                    vykonajPohyb(sachovnica, startR, startS, cielR, cielS);

                   
                    hrac = 1 - hrac;

                    // Kontrola, či je nový hráč v šachu
                    if (jeSach(sachovnica, hrac + 1)) {
                        printf("Hráč %d je v šachu!\n", hrac + 1);

                        // Kontrola, či je hráč v šachmate
                        if (jeSachmat(sachovnica, hrac + 1)) {
                            printf("Šachmat! Hráč %d prehral.\n", hrac + 1);
                            return 0; // Ukončenie programu po šachmate
                        }
                    }
                    vycistiKonzolu();
                    break; // Prechod na ďalší ťah
                }
            }
            printf("Neplatný pohyb! Skúste znova.\n");
        }
    }
    
    a:
    return 0;

}

void inicializujSachovnicu(char sachovnica[BOARD_SIZE][BOARD_SIZE]) {
    char startovaciaPozicia[BOARD_SIZE][BOARD_SIZE] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };
    memcpy(sachovnica, startovaciaPozicia, sizeof(startovaciaPozicia));
}

// Funkcia na zadanie nickname pre hráča
void zadajNickname(Hrac* hrac, int cisloHrac) {
    printf("Zadajte nickname pre hráča %d: ", cisloHrac);
    fgets(hrac->meno, MAX_NICKNAME_LENGTH, stdin);
    hrac->meno[strcspn(hrac->meno, "\n")] = 0;
}

// Funkcia na výber farby pre hráča 1
void vyberFarbu(Hrac* hrac) {
    int vyber;
    printf("Hráč %s, vyber si farbu:\n1. Biely\n2. Čierny\n", hrac->meno);
    printf("Zadajte 1 alebo 2: ");
    scanf("%d", &vyber);
    while (vyber != 1 && vyber != 2) {
        printf("Neplatná voľba! Zadajte 1 alebo 2: ");
        scanf("%d", &vyber);
    }
    hrac->farba = vyber;
}

// Vyčistenie konzoly
void vycistiKonzolu() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Zobrazenie šachovnice
void zobrazSachovnicu(char sachovnica[BOARD_SIZE][BOARD_SIZE]) {
    printf("  A B C D E F G H\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (isupper(sachovnica[i][j])) {
                printf("%s%c%s ", WHITE_PIECE_COLOR, sachovnica[i][j], RESET_COLOR); 
            }
            else if (islower(sachovnica[i][j])) {
                printf("%s%c%s ", BLACK_PIECE_COLOR, sachovnica[i][j], RESET_COLOR); 
            }
            else {
                printf("%c ", sachovnica[i][j]);
            }
        }
        printf("%d\n", 8 - i);
    }
    printf("  A B C D E F G H\n");
}

// Overenie, či je figúrka biela
int jeBielaFigurka(char figura) {
    return (figura >= 'A' && figura <= 'Z');
}

// Overenie, či je figúrka čierna
int jeCiernaFigurka(char figura) {
    return (figura >= 'a' && figura <= 'z');
}

int jeValidnyPohyb(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS, int hrac) {
    if (startR < 0 || startR >= BOARD_SIZE || startS < 0 || startS >= BOARD_SIZE ||
        cielR < 0 || cielR >= BOARD_SIZE || cielS < 0 || cielS >= BOARD_SIZE) {
        return 0;
    }

    char figura = sachovnica[startR][startS];
    char ciel = sachovnica[cielR][cielS];

    if ((hrac == 1 && !jeBielaFigurka(figura)) || (hrac == 2 && !jeCiernaFigurka(figura))) {
        return 0;
    }

    if ((hrac == 1 && jeBielaFigurka(ciel)) || (hrac == 2 && jeCiernaFigurka(ciel))) {
        return 0;
    }

    switch (figura) {
    case 'P':
        if (startR - 1 == cielR && startS == cielS && ciel == '.') return 1;
        if (startR == 6 && startR - 2 == cielR && startS == cielS && ciel == '.' && sachovnica[startR - 1][startS] == '.') return 1;
        if (startR - 1 == cielR && abs(startS - cielS) == 1 && jeCiernaFigurka(ciel)) return 1;
        break;
    case 'p':
        if (startR + 1 == cielR && startS == cielS && ciel == '.') return 1;
        if (startR == 1 && startR + 2 == cielR && startS == cielS && ciel == '.' && sachovnica[startR + 1][startS] == '.') return 1;
        if (startR + 1 == cielR && abs(startS - cielS) == 1 && jeBielaFigurka(ciel)) return 1;
        break;
    case 'R': case 'r':
        if (startR == cielR || startS == cielS) {
            if (jeCestaPrazdna(sachovnica, startR, startS, cielR, cielS)) return 1;
        }
        break;
    case 'N': case 'n':
        if ((abs(startR - cielR) == 2 && abs(startS - cielS) == 1) ||
            (abs(startR - cielR) == 1 && abs(startS - cielS) == 2)) return 1;
        break;
    case 'B': case 'b':
        if (abs(startR - cielR) == abs(startS - cielS)) {
            if (jeCestaPrazdna(sachovnica, startR, startS, cielR, cielS)) return 1;
        }
        break;
    case 'Q': case 'q':
        if (startR == cielR || startS == cielS || abs(startR - cielR) == abs(startS - cielS)) {
            if (jeCestaPrazdna(sachovnica, startR, startS, cielR, cielS)) return 1;
        }
        break;
    case 'K': case 'k':
        if (abs(startR - cielR) <= 1 && abs(startS - cielS) <= 1) return 1;
        break;
    }

    return 0;
}


// Kontrola, či je cesta voľná
int jeCestaPrazdna(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS) {
    int dR = (cielR - startR) == 0 ? 0 : (cielR - startR) / abs(cielR - startR);
    int dS = (cielS - startS) == 0 ? 0 : (cielS - startS) / abs(cielS - startS);
    int r = startR + dR, s = startS + dS;

    while (r != cielR || s != cielS) {
        if (sachovnica[r][s] != '.') return 0;
        r += dR;
        s += dS;
    }
    return 1;
}

// Vykonanie pohybu figúrky
void vykonajPohyb(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS) {
    sachovnica[cielR][cielS] = sachovnica[startR][startS];
    sachovnica[startR][startS] = '.';
}

void ulozHru(char sachovnica[BOARD_SIZE][BOARD_SIZE], int naRade, Hrac hraci[2]) {
    FILE* subor = fopen("sachova_hra.txt", "w");
    if (subor == NULL) {
        printf("Chyba pri ukladaní hry!\n");
        return;
    }


    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(subor, "%c", sachovnica[i][j]);
        }
        fprintf(subor, "\n");
    }


    fprintf(subor, "%d\n", naRade);
    fprintf(subor, "%s\n", hraci[0].meno);
    fprintf(subor, "%d\n", hraci[0].farba);
    fprintf(subor, "%s\n", hraci[1].meno);
    fprintf(subor, "%d\n", hraci[1].farba);

    fclose(subor);
    printf("Hra bola úspešne uložena!\n");
}

int nacitajHru(char sachovnica[BOARD_SIZE][BOARD_SIZE], int* naRade, Hrac hraci[2]) {
    FILE* subor = fopen("sachova_hra.txt", "r");
    if (subor == NULL) {
        printf("Nebolo možne načítat uloženu hru!\n");
        return 0;
    }


    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fscanf(subor, " %c", &sachovnica[i][j]);
        }
    }


    fscanf(subor, "%d", naRade);
    fscanf(subor, "%s", hraci[0].meno);
    fscanf(subor, "%d", &hraci[0].farba);
    fscanf(subor, "%s", hraci[1].meno);
    fscanf(subor, "%d", &hraci[1].farba);

    fclose(subor);
    printf("Hra bola úspešne načítaná!\n");
    return 1;
}

// Funkcia na overenie, či je kráľ hráča v šachu
bool jeSach(char sachovnica[BOARD_SIZE][BOARD_SIZE], int hrac) {
    int kralR = -1, kralS = -1;

    // Nájdeme pozíciu kráľa
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if ((hrac == 1 && sachovnica[i][j] == 'K') || (hrac == 2 && sachovnica[i][j] == 'k')) {
                kralR = i;
                kralS = j;
                break;
            }
        }
    }

    // Skontrolujeme, či je kráľ ohrozený
    if (jeSachPozicia(sachovnica, kralR, kralS, hrac)) {
        return true;
    }
    return false;
}

// Funkcia na kontrolu, či je šach-mat
bool jeSachmat(char sachovnica[BOARD_SIZE][BOARD_SIZE], int hrac) {
    printf("Kontrola, či je hráč %d v šachmate.\n", hrac);

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if ((hrac == 1 && jeBielaFigurka(sachovnica[i][j])) || (hrac == 2 && jeCiernaFigurka(sachovnica[i][j]))) {
                for (int r = 0; r < BOARD_SIZE; r++) {
                    for (int s = 0; s < BOARD_SIZE; s++) {
                        if (jeValidnyPohyb(sachovnica, i, j, r, s, hrac)) {

                            char temp = sachovnica[r][s];
                            sachovnica[r][s] = sachovnica[i][j];
                            sachovnica[i][j] = '.';

                            bool sach = jeSach(sachovnica, hrac);

                            sachovnica[i][j] = sachovnica[r][s];
                            sachovnica[r][s] = temp;

                            if (!sach) {
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    printf("Hráč %d je v šachmate.\n", hrac);
    return true;
}
int jeRosada(char sachovnica[BOARD_SIZE][BOARD_SIZE], int hrac, int startR, int startS, int cielR, int cielS) {
   
    if (!jeCestaRosadyVolna(sachovnica, startR, startS, startR, 7)) {
        printf("Cesta pre kráľovskú rošádu nie je voľná.\n");
    }
    if (jeSachPozicia(sachovnica, startR, startS, hrac)) {
        printf("Kráľ je v šachu, rošáda nie je možná.\n");
    }
    if (hrac == 1 && startR == 7 && startS == 4) {
        // Kráľovská rošáda (O-O)
        if (cielR == 7 && cielS == 6 &&
            sachovnica[7][5] == '.' && sachovnica[7][6] == '.' && sachovnica[7][7] == 'R' &&
            jeCestaRosadyVolna(sachovnica, startR, startS, 7, 7) &&
            !jeSachPozicia(sachovnica, 7, 4, 1) &&
            !jeSachPozicia(sachovnica, 7, 5, 1) &&
            !jeSachPozicia(sachovnica, 7, 6, 1)) {
            return 1;
        }
        // Dámska rošáda (O-O-O)
        if (cielR == 7 && cielS == 2 &&
            sachovnica[7][1] == '.' && sachovnica[7][2] == '.' && sachovnica[7][3] == '.' && sachovnica[7][0] == 'R' &&
            jeCestaRosadyVolna(sachovnica, startR, startS, 7, 0) &&
            !jeSachPozicia(sachovnica, 7, 4, 1) &&
            !jeSachPozicia(sachovnica, 7, 3, 1) &&
            !jeSachPozicia(sachovnica, 7, 2, 1)) {
            return 1;
        }
    }
    // Kráľovská a dámska rošáda pre čierneho hráča
    if (hrac == 2 && startR == 0 && startS == 4) {
        // Kráľovská rošáda (O-O)
        if (cielR == 0 && cielS == 6 &&
            sachovnica[0][5] == '.' && sachovnica[0][6] == '.' && sachovnica[0][7] == 'r' &&
            jeCestaRosadyVolna(sachovnica, startR, startS, 0, 7) &&
            !jeSachPozicia(sachovnica, 0, 4, 2) &&
            !jeSachPozicia(sachovnica, 0, 5, 2) &&
            !jeSachPozicia(sachovnica, 0, 6, 2)) {
            return 1;
        }
        // Dámska rošáda (O-O-O)
        if (cielR == 0 && cielS == 2 &&
            sachovnica[0][1] == '.' && sachovnica[0][2] == '.' && sachovnica[0][3] == '.' && sachovnica[0][0] == 'r' &&
            jeCestaRosadyVolna(sachovnica, startR, startS, 0, 0) &&
            !jeSachPozicia(sachovnica, 0, 4, 2) &&
            !jeSachPozicia(sachovnica, 0, 3, 2) &&
            !jeSachPozicia(sachovnica, 0, 2, 2)) {
            return 1;
        }
    }

    return 0; // Rošáda nie je možná
}
// Funkcia na kontrolu, či je medzi kráľom a vežou voľná cesta
int jeCestaRosadyVolna(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int cielR, int cielS) {
    int krokS = (cielS > startS) ? 1 : -1; 
    int s = startS + krokS;

    while (s != cielS) {
        if (sachovnica[startR][s] != '.') {
            return 0;
        }
        s += krokS;
    }

    return 1; // Cesta je voľná
}



int jeSachPozicia(char sachovnica[BOARD_SIZE][BOARD_SIZE], int r, int s, int hrac) {
    for (int rr = 0; rr < BOARD_SIZE; rr++) {
        for (int ss = 0; ss < BOARD_SIZE; ss++) {
            if ((hrac == 1 && jeCiernaFigurka(sachovnica[rr][ss])) ||
                (hrac == 2 && jeBielaFigurka(sachovnica[rr][ss]))) {
                if (jeValidnyPohyb(sachovnica, rr, ss, r, s, 3 - hrac)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void zobrazMoznePozicie(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int hrac) {
    
    printf("Možne pozície pre figurku na %c%d:\n", 'A' + startS, 8 - startR);
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int s = 0; s < BOARD_SIZE; s++) {
            if (jeValidnyPohyb(sachovnica, startR, startS, r, s, hrac)) {
                printf("%c%d ", 'A' + s, 8 - r);
            }
        }
    }
    if (jeRosada(sachovnica, hrac, startR, startS, startR, 6)) {
        printf("\nRošáda možná: G%d (kráľovská).\n", 8 - startR);
    }
    if (jeRosada(sachovnica, hrac, startR, startS, startR, 2)) {
        printf("Rošáda možná: C%d (dámska).\n", 8 - startR);
    }
    // Kontrola rošády pre kráľa
    if ((hrac == 1 && startR == 7 && startS == 4 && sachovnica[startR][startS] == 'K') ||
        (hrac == 2 && startR == 0 && startS == 4 && sachovnica[startR][startS] == 'k')) {
        // Kráľovská rošáda (O-O)
        if (jeRosada(sachovnica, hrac, startR, startS, startR, 6)) {
            printf("%c%d ", 'A' + 6, 8 - startR);
        }
        // Dámska rošáda (O-O-O)
        if (jeRosada(sachovnica, hrac, startR, startS, startR, 2)) {
            printf("%c%d ", 'A' + 2, 8 - startR);
        }
    }
    printf("\n");
}

int maDostupnePohyby(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int hrac) {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int s = 0; s < BOARD_SIZE; s++) {
            if (jeValidnyPohyb(sachovnica, startR, startS, r, s, hrac)) {
                return 1; // Má aspoň jeden možný pohyb
            }
        }
    }
    return 0; // Nemá žiadne možné pohyby
}

void zobrazSachovnicuSoZvyraznenim(char sachovnica[BOARD_SIZE][BOARD_SIZE], int startR, int startS, int hrac) {
    printf("  A B C D E F G H\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", 8 - i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            int jeMoznyTah = jeValidnyPohyb(sachovnica, startR, startS, i, j, hrac);

            // Skontrolujeme, či je možné vykonať rošádu
            if ((sachovnica[startR][startS] == 'K' || sachovnica[startR][startS] == 'k') &&
                ((jeRosada(sachovnica, hrac, startR, startS, startR, 6) && i == startR && j == 6) ||
                    (jeRosada(sachovnica, hrac, startR, startS, startR, 2) && i == startR && j == 2))) {
                jeMoznyTah = 1; // Označte políčka pre rošádu
            }

            if (jeMoznyTah) {
                // Zvýraznenie políčka zelenou farbou, ak je pohyb možný
                printf("\033[42m"); // Zelené pozadie
            }

            // Zobrazenie figúrky
            if (isupper(sachovnica[i][j])) {
                printf("%s%c%s ", WHITE_PIECE_COLOR, sachovnica[i][j], RESET_COLOR); // Biela figúrka
            }
            else if (islower(sachovnica[i][j])) {
                printf("%s%c%s ", BLACK_PIECE_COLOR, sachovnica[i][j], RESET_COLOR); // Čierna figúrka
            }
            else {
                printf("%c ", sachovnica[i][j]);
            }

            // Resetovanie farieb
            if (jeMoznyTah) {
                printf("%s", RESET_COLOR);
            }
        }
        printf("%d\n", 8 - i);
    }
    printf("  A B C D E F G H\n");

    // Výpis možných pohybov
    printf("Možné ťahy pre figúrku na %c%d:\n", 'A' + startS, 8 - startR);
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (jeValidnyPohyb(sachovnica, startR, startS, i, j, hrac)) {
                printf("%c%d -> %c%d\n", 'A' + startS, 8 - startR, 'A' + j, 8 - i);
            }
        }
    }

    // Kontrola rošády
    if ((sachovnica[startR][startS] == 'K' || sachovnica[startR][startS] == 'k')) {
        if (jeRosada(sachovnica, hrac, startR, startS, startR, 6)) {
            printf("%c%d -> G%d (Kráľovská rošáda)\n", 'A' + startS, 8 - startR, 8 - startR);
        }
        if (jeRosada(sachovnica, hrac, startR, startS, startR, 2)) {
            printf("%c%d -> C%d (Dámska rošáda)\n", 'A' + startS, 8 - startR, 8 - startR);
        }
    }
}
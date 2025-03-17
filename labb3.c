#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILNAMN 40
#define MAX_PATIENTER 1000
#define MAX_NAMN 40
#define MAX_PERSONNUMMER 12
#define MAX_BILDREFERENSER 10
#define MAX_RAD 200

struct patientData {
    char namn[MAX_NAMN];
    char personnummer[MAX_PERSONNUMMER];
    int bildreferenser[MAX_BILDREFERENSER];
};
typedef struct patientData Patientdata;

struct patient {
    Patientdata patienter[MAX_PATIENTER];
    int antalPatienter;
};
typedef struct patient Patient;

void lasInData(const char *pFilnamn, Patient *pPatienter);
void sparaData(const char *pFilnamn, Patient *pPatienter);
void huvudMeny(const char *pFilnamn);
void registreraNyPatient(Patient *pPatienter);
void skrivUtAllaPatienter(Patient *pPatienter);
int arBildReferensUnik(Patient *pPatienter, int referens);
void sorteraPatienter(Patient *pPatienter);
int hittaPatienter(Patient *pPatienter, int sokKategori, const char *sokStrang, int *matchandeIndex, int maxAntalTraffar);
void anvandarSokning(Patient *pPatienter);
void laggTillBilder(Patient *pPatienter);
void avregistreraPatient(Patient *pPatienter);
int valideraPersonnummer(const char *personnummer);
int konverteraTillFulltDatum(const char *personnummer);
void taBortBilder(Patient *pPatienter);
int kontrolleraKontrollsiffra(const char *personnummer);

int main()
{
    char fil[MAX_FILNAMN];

    printf("Patientjournalsystem\n");
    printf("Vilken fil vill du använda: ");
    scanf("%39s", fil);
    while (getchar() != '\n'); 

    huvudMeny(fil);

    return 0;
}

void huvudMeny(const char *pFilnamn)
{
    Patient patient;
    patient.antalPatienter = 0;

    // läser in data från filen om de finns
    lasInData(pFilnamn, &patient);

    int menyVal;

    do
    {
        printf("\nHuvudmeny\n");
        printf("\t1) Registrera nya patienter\n");
        printf("\t2) Skriva ut alla patienter\n");
        printf("\t3) Söka efter patienter\n");
        printf("\t4) Lägg till bilder\n");
        printf("\t5) Sortera patienter\n");
        printf("\t6) Avregistrera patienter\n");
        printf("\t7) Ta bort bilder\n");
        printf("\t8) Avsluta programmet\n");
        printf("Ange alternativ: ");

        if (scanf("%d", &menyVal) != 1) {
            printf("Ogiltig inmatning.\n");
            while (getchar() != '\n'); // rensa resten av raden vid felinmatning
            continue;
        } else{
            while (getchar() != '\n'); // rensa resten av raden inkluderat \n
        }

        switch (menyVal)
        {
        case 1:
            registreraNyPatient(&patient);
            break;
        case 2:
            skrivUtAllaPatienter(&patient);
            break;
        case 3:
            anvandarSokning(&patient);
            break;
        case 4:
            laggTillBilder(&patient);
            break;
        case 5:
            sorteraPatienter(&patient);
            break;
        case 6:
            avregistreraPatient(&patient);
            break;
        case 7:
            taBortBilder(&patient);
            break;
        case 8:
            printf("Sparar patientdata i %s\n", pFilnamn);
            sparaData(pFilnamn, &patient);
            printf("Avslutar programmet.\n");
            break;
        default:
            printf("Ogiltigt alternativ.\n");
            break;
        }
    }
    while (menyVal != 8);
}

void lasInData(const char *pFilnamn, Patient *pPatienter)
{
    int i;
    FILE *pFilPekare = fopen(pFilnamn, "r"); //öppnar i read mode
    if (pFilPekare == NULL)
    {
        printf("Filen '%s' hittades inte. Skapar en ny fil.\n", pFilnamn);
        pFilPekare = fopen(pFilnamn, "w");
        if (pFilPekare == NULL)
        {
            printf("Kunde inte skapa filen.\n");
            return;
        }
        fclose(pFilPekare);
        return;
    }

    char rad[MAX_RAD]; // tillfällig array för rad 3 bildreferenser

    while (1)
    {
        // läs in personnummer 1
        if (fscanf(pFilPekare, "%11s", (*pPatienter).patienter[(*pPatienter).antalPatienter].personnummer) != 1)
        {
            break; // går ej läsa in fler personnummer vilket betyder vi inte kan lägga fler patienter
        }
        fgetc(pFilPekare); // tar bort newline efter personnumret \n

        // läs in namn 2
        if (fgets((*pPatienter).patienter[(*pPatienter).antalPatienter].namn, MAX_NAMN, pFilPekare) == NULL)
        {
            break; // går ej läsa in mer namn så avbryt
        }
    
        (*pPatienter).patienter[(*pPatienter).antalPatienter].namn[strcspn((*pPatienter).patienter[(*pPatienter).antalPatienter].namn, "\n")] = '\0'; // ta bort eventuell newline från slutet av namnet

        // läs raden med bildreferenser 3
        if (fgets(rad, sizeof(rad), pFilPekare) == NULL)
        {
            break; // personnr, namn och bilder är nu färdigt finns inget mer så avbryt
        }

        // nollställ alla bildreferenser
        for (i = 0; i < MAX_BILDREFERENSER; i++)
        {
            (*pPatienter).patienter[(*pPatienter).antalPatienter].bildreferenser[i] = 0;
        }

        char *token = strtok(rad, " \t");
        i = 0;
        while (token != NULL && i < MAX_BILDREFERENSER)
        {
            // omvandla token till int
            (*pPatienter).patienter[(*pPatienter).antalPatienter].bildreferenser[i] = atoi(token);
            i++;
            token = strtok(NULL, " \t");
        }

        // öka antal patienter
        (*pPatienter).antalPatienter++;
        if ((*pPatienter).antalPatienter >= MAX_PATIENTER)
        {
            printf("Max antal patienter uppnått under inläsning.\n");
            break;
        }
    }
    fclose(pFilPekare);
}

void sparaData(const char *pFilnamn, Patient *pPatienter)
{
    int i, j;
    FILE *pFilPekare = fopen(pFilnamn, "w"); //write mode
    if (pFilPekare == NULL)
    {
        printf("Kunde inte öppna filen för att spara data.\n");
        return;
    }

    for (i = 0; i < (*pPatienter).antalPatienter; i++)
    {
        // personnummer på egen rad
        fprintf(pFilPekare, "%s\n", (*pPatienter).patienter[i].personnummer);

        // namn på egen rad
        fprintf(pFilPekare, "%s\n", (*pPatienter).patienter[i].namn);

        // alla bildreferenser på en egen rad
        for (j = 0; j < MAX_BILDREFERENSER; j++)
        {
            if ((*pPatienter).patienter[i].bildreferenser[j] != 0)
            {
                fprintf(pFilPekare, "%d ", (*pPatienter).patienter[i].bildreferenser[j]);
            }
        }
        fprintf(pFilPekare, "\n");  // avsluta rad
    }
    fclose(pFilPekare);
}

void registreraNyPatient(Patient *pPatienter)
{
    printf("\n");

    char personnummer[MAX_PERSONNUMMER];
    char namn[MAX_NAMN];

    while (1)
    {
        if ((*pPatienter).antalPatienter >= MAX_PATIENTER){
            printf("Maximalt antal patienter har redan uppnåtts.\n");
            break;  
        }

        printf("Ange personnummer (q för att avsluta): ");
        if (scanf("%11s", personnummer) != 1)
        {
            printf("Ogiltig inmatning! Försök igen.\n");
            while (getchar() != '\n');
            continue;
        }

        if (strcmp(personnummer, "q") == 0)
        {
            printf("Avslutar registrering.\n");
            break;
        }

        if (!valideraPersonnummer(personnummer)) {
            continue;
        }
        if (!kontrolleraKontrollsiffra(personnummer)) {
            printf("Felaktig kontrollsiffra! Försök igen.\n");
            continue;
        }

        int finnsAllaredan = 0;
        for (int i = 0; i < (*pPatienter).antalPatienter; i++)
        {
            if (strcmp((*pPatienter).patienter[i].personnummer, personnummer) == 0)
            {
                printf("Personnumret är redan registrerat.\n");
                finnsAllaredan = 1;
                break;
            }
        }
        if (finnsAllaredan)
        {
            continue; // försök igen
        }

        // läs namn
        while (getchar() != '\n'); 
        printf("Ange namn: ");
        if (scanf("%39[^\n]", namn) != 1)
        {
            printf("Ogiltig inmatning för namn! Försök igen.\n");
            while (getchar() != '\n');
            continue;
        }

        // newindex sparar den position där patientens data ska lagras
        int newIndex = (*pPatienter).antalPatienter;
        (*pPatienter).antalPatienter++; // officiellt en ny patient i databasen

        // kopiera in personnummer o namn på newindex
        strcpy((*pPatienter).patienter[newIndex].personnummer, personnummer);
        strcpy((*pPatienter).patienter[newIndex].namn, namn);

        // nollställ bildreferenser
        for (int i = 0; i < MAX_BILDREFERENSER; i++) {
            (*pPatienter).patienter[newIndex].bildreferenser[i] = 0;
        }

        // läsa in bildreferenser
        int antalReferenser = 0;
        while (antalReferenser < MAX_BILDREFERENSER)
        {
            printf("Ange bildreferens (eller 0 för att avsluta): ");
            int referens;
            if (scanf("%d", &referens) != 1)
            {
                printf("Ogiltig inmatning! Försök igen.\n");
                while (getchar() != '\n');
                continue;
            }

            if (referens == 0)
            {
                // klar med denna patient
                break;
            }

            // kolla om referensen redan finns globalt
            if (arBildReferensUnik(pPatienter, referens))
            {
                // lägg in i denna patients array
                (*pPatienter).patienter[newIndex].bildreferenser[antalReferenser++] = referens;
            }
            else
            {
                printf("Referensen %d finns redan!\n", referens);
            }
        }
        printf("Patient registrerad.\n");
    }
}

void skrivUtAllaPatienter(Patient *pPatienter)
{
    int i, j;
    
    if ((*pPatienter).antalPatienter == 0)
    {
        printf("Databasen är tom.\n");
        return;
    }

    printf("Personnummer    Namn            Bildreferenser\n");
    printf("________________________________________________\n");

    // loopa igenom alla patienter
    for (i = 0; i < (*pPatienter).antalPatienter; i++)
    {
        printf("%-15s %-15s [", (*pPatienter).patienter[i].personnummer, (*pPatienter).patienter[i].namn);

        int forstaBildUtskriven = 0; // kollar om vi har skrivit ut något

        // loopa igenom bildreferenser
        for (j = 0; j < MAX_BILDREFERENSER; j++)
        {
            int ref = (*pPatienter).patienter[i].bildreferenser[j];

            if (ref != 0)
            {
                if (forstaBildUtskriven) // Om det inte är första utskriften, lägg till ", "
                {
                    printf(", ");
                }
                printf("%d", ref);
                forstaBildUtskriven = 1; // nu har det skrivits ut en bildreferensreferens
            }
        }
        printf("]\n"); 
    }
}

int arBildReferensUnik(Patient *pPatienter, int referens)
{
    int i, j;

    for (i = 0; i < (*pPatienter).antalPatienter; i++)
    {
        for (j = 0; j < MAX_BILDREFERENSER; j++)
        {
            if ((*pPatienter).patienter[i].bildreferenser[j] == referens)
            {
                return 0; // bildreferensen finns redan
            }
        }
    }
    return 1; // bildreferensen är unik
}

void sorteraPatienter(Patient *pPatienter)
{
    printf("\n");

    char np;
    int i, j;

    if ((*pPatienter).antalPatienter == 0)
    {
        printf("Databasen är tom.\n");
        return;
    }

    printf("Vill du sortera genom namn eller personnummer (namn = n / personnummer = p)?: ");
    scanf(" %c", &np);
    while (getchar() != '\n'); 

    if (np == 'n')
    {
        for (i = 0; i < (*pPatienter).antalPatienter - 1; i++)
        {
            for (j = 0; j < (*pPatienter).antalPatienter - i - 1; j++)
            {
                if (strcmp((*pPatienter).patienter[j].namn, (*pPatienter).patienter[j + 1].namn) > 0)
                {
                    Patientdata temp = (*pPatienter).patienter[j];
                    (*pPatienter).patienter[j] = (*pPatienter).patienter[j + 1];
                    (*pPatienter).patienter[j + 1] = temp;
                }
            }
        }
        printf("Patienterna har sorterats efter namn.\n");
    }
    else if (np == 'p')
    {
        for (i = 0; i < (*pPatienter).antalPatienter - 1; i++)
        {
            for (j = 0; j < (*pPatienter).antalPatienter - i - 1; j++)
            {
                // konvertera personnummer till yyyymmdd för rätt jämförelse
                int datum1 = konverteraTillFulltDatum((*pPatienter).patienter[j].personnummer);
                int datum2 = konverteraTillFulltDatum((*pPatienter).patienter[j + 1].personnummer);

                if (datum1 > datum2) // jämför hela datumet som ett tal
                {
                    Patientdata temp = (*pPatienter).patienter[j];
                    (*pPatienter).patienter[j] = (*pPatienter).patienter[j + 1];
                    (*pPatienter).patienter[j + 1] = temp;
                }
            }
        }
        printf("Patienterna har sorterats efter personnummer.\n");
    } else
    {
        printf("Fel bokstav. Försök igen.\n");
    }
}

int hittaPatienter(Patient *pPatienter, int sokKategori, const char *sokStrang, int *matchandeIndex, int maxAntalTraffar)
{
    int antalMatchande = 0;
    int totaltAntal = (*pPatienter).antalPatienter;
    int i, j;

    for (i = 0; i < totaltAntal; i++)
    {
        int hittad = 0;

        if (sokKategori == 1)  // personnummer
        {
            if (strcmp((*pPatienter).patienter[i].personnummer, sokStrang) == 0)
            {
                hittad = 1;
            }
        }
        else if (sokKategori == 2)  // namn (delsträng)
        {
            if (strstr((*pPatienter).patienter[i].namn, sokStrang) != NULL)
            {
                hittad = 1;
            }
        }
        else if (sokKategori == 3)  // bildreferens
        {
            int ref = atoi(sokStrang); // str till int
            for (j = 0; j < MAX_BILDREFERENSER; j++)
            {
                if ((*pPatienter).patienter[i].bildreferenser[j] == ref)
                {
                    hittad = 1;
                    break;
                }
            }
        }

        if (hittad)
        {
            // skriv ut rubrik enbart vid första träffen
            if (antalMatchande == 0)
            {
                printf("Personnummer\tNamn\tBildreferenser\n");
                printf("____________________________________________________________\n");
            }

            // skriv ut patientinfo
            printf("%s\t%s\t[", (*pPatienter).patienter[i].personnummer, (*pPatienter).patienter[i].namn);

            int forstaBild = 1;
            for (j = 0; j < MAX_BILDREFERENSER; j++)
            {
                int bildID = (*pPatienter).patienter[i].bildreferenser[j];
                if (bildID != 0)
                {
                    if (!forstaBild){
                        printf(", ");
                    }
                    printf("%d", bildID);
                    forstaBild = 0;
                }
            }
            printf("]\n");

            // spara index om det finns plats i matchandeIndex
            if (antalMatchande < maxAntalTraffar)
            {
                matchandeIndex[antalMatchande] = i;
            }
            antalMatchande++;
        }
    }
    return antalMatchande;
}

void anvandarSokning(Patient *pPatienter)
{
    printf("\n");

    // om databasen är tom då avbryt
    if ((*pPatienter).antalPatienter == 0)
    {
        printf("Databasen är tom. Återgår till huvudmenyn.\n");
        return;
    }

    int menyVal;
    do
    {
        printf("Sök på personnummer(1), namn(2), bildreferens(3), avsluta(4): ");
        if (scanf("%d", &menyVal) != 1 || menyVal < 1 || menyVal > 4)
        {
            printf("Ogiltigt alternativ. Försök igen.\n");
            while (getchar() != '\n'); 
            continue;
        }

        if (menyVal == 4)
        {
            printf("Avslutar sökning och återgår till huvudmenyn.\n");
            return;
        }

        // läs söksträng
        char term[50];
        while (getchar() != '\n');
        if (menyVal == 1)
        {
            printf("Ange personnummer: ");
            scanf("%49[^\n]", term);
        }
        else if (menyVal == 2)
        {
            printf("Ange söksträng: ");
            scanf("%49[^\n]", term);
        }
        else
        {
            // menyVal == 3
            printf("Ange bildreferens: ");
            scanf("%49[^\n]", term);
        }

        // ropa hjälpfunktionen
        int hittadeIndex[1000];
        int antalFunna = hittaPatienter(pPatienter, menyVal, term, hittadeIndex, 1000);

        // om inga träffar
        if (antalFunna == 0)
        {
            if (menyVal == 1)
                printf("Personnumret saknas i databasen.\n");
            else if (menyVal == 2)
                printf("Inga patienter hittades med det namnet.\n");
            else
                printf("Ingen patient hittades med den bildreferensen.\n");
        }
    }
    while (1);
}

void laggTillBilder(Patient *pPatienter)
{
    printf("\n");

    // kontrollera om databasen är tom
    if ((*pPatienter).antalPatienter == 0)
    {
        printf("Databasen är tom. Återgår till huvudmenyn.\n");
        return;
    }

    // låt användaren loopa sökningar tills exakt en träff
    while (1)
    {
        int sokVal;
        printf("Sök pa personnummer(1), namn(2), bildreferens(3), avsluta(4): ");
        if (scanf("%d", &sokVal) != 1)
        {
            while (getchar() != '\n');
            printf("Ogiltig inmatning.\n");
            continue;
        }
        if (sokVal == 4)
        {
            return;
        }
        if (sokVal < 1 || sokVal > 3)
        {
            printf("Ogiltigt val. Försök igen.\n");
            continue;
        }

        // läs in söksträng
        while (getchar() != '\n');
        char sokStrang[40];
        if (sokVal == 1)
        {
            printf("Ange personnummer: ");
            scanf("%39[^\n]", sokStrang);
        }
        else if (sokVal == 2)
        {
            printf("Ange sökstrang: ");
            scanf("%39[^\n]", sokStrang);
        }
        else // sokVal == 3
        {
            printf("Ange bildreferens: ");
            scanf("%39[^\n]", sokStrang);
        }

        // Anropa vår sök-funktion
        int hittadeIndex[1000];
        int antalFunna = hittaPatienter(pPatienter, sokVal, sokStrang, hittadeIndex, 1000);

        if (antalFunna == 0)
        {
            // inga träffar
            if (sokVal == 1){
                printf("Personnumret saknas i databasen\n");
            } else if (sokVal == 2){
                printf("Inga patienter hittades med det namnet.\n");
            } else{
                printf("Ingen patient hittades med den bildreferensen.\n");
            }
            printf("Du fick inte exakt en träff.\n");
        } else if (antalFunna > 1){
            printf("Du fick inte exakt en träff.\n"); // flera träffar
        } else{
            int endaIndex = hittadeIndex[0]; // exakt en träff så dags att lägga till bilder

            // räkna hur många bilder redan finns
            int antalUpptagna = 0;
            for (int i = 0; i < MAX_BILDREFERENSER; i++)
            {
                if ((*pPatienter).patienter[endaIndex].bildreferenser[i] != 0){
                    antalUpptagna++;
                }
            }

            // nu lägg in nya bildreferenser
            while (1)
            {
                if (antalUpptagna >= MAX_BILDREFERENSER)
                {
                    printf("Antalet referenser fyllt\n");
                    printf("Avslutar lagga till bilder.\n");
                    return;
                }

                int nyRef;
                printf("Ange bildreferens (eller 0 för att avsluta): ");
                if (scanf("%d", &nyRef) != 1)
                {
                    while (getchar() != '\n');
                    printf("Ogiltig inmatning!\n");
                    continue;
                }

                if (nyRef == 0)
                {
                    printf("Avslutar lagga till bilder.\n");
                    return;
                }

                // kolla om den finns redan
                if (!arBildReferensUnik(pPatienter, nyRef))
                {
                    printf("Referensen finns redan!\n");
                    continue;
                }

                // lägg till i första lediga plats
                for (int i = 0; i < MAX_BILDREFERENSER; i++)
                {
                    if ((*pPatienter).patienter[endaIndex].bildreferenser[i] == 0)
                    {
                        (*pPatienter).patienter[endaIndex].bildreferenser[i] = nyRef;
                        antalUpptagna++;
                        break;
                    }
                }
            } 
        } 
    } 
}

void avregistreraPatient(Patient *pPatienter)
{
    printf("\n");

    if ((*pPatienter).antalPatienter == 0)
    {
        printf("Databasen är tom. Återgår till huvudmenyn.\n");
        return;
    }

    while (1)
    {
        // fråga användaren om hur den vill söka
        int sokVal;
        printf("Sok pa personnummer(1), namn(2), bildreferens(3), avsluta(4): ");
        if (scanf("%d", &sokVal) != 1)
        {
            while (getchar() != '\n');
            printf("Ogiltig inmatning.\n");
            continue;
        }
        if (sokVal == 4)
        {
            return;
        }
        if (sokVal < 1 || sokVal > 3)
        {
            printf("Ogiltigt val. Försök igen.\n");
            continue;
        }

        // läs in söksträng beroende på sokVal
        while (getchar() != '\n'); 
        char sokStrang[40];
        if (sokVal == 1)
        {
            printf("Ange personnummer: ");
            scanf("%39[^\n]", sokStrang);
        }
        else if (sokVal == 2)
        {
            printf("Ange sokstrang: ");
            scanf("%39[^\n]", sokStrang);
        }
        else
        {
            // sokVal == 3
            printf("Ange bildreferens: ");
            scanf("%39[^\n]", sokStrang);
        }

        int hittadeIndex[1000];
        int antalFunna = hittaPatienter(pPatienter, sokVal, sokStrang, hittadeIndex, 1000);

        // hantera resultat
        if (antalFunna == 0)
        {
            // ingen träff
            if (sokVal == 1){
                printf("Personnumret saknas i databasen\n");
            } else if (sokVal == 2){
                printf("Inga patienter hittades med det namnet.\n");
            } else{
                printf("Ingen patient hittades med den bildreferensen.\n");
            }
            printf("Du fick inte exakt en traff.\n");
        } else if (antalFunna > 1)
        {
            printf("Du fick inte exakt en traff.\n"); // flera träffar
        } else
        {
            // exakt en träff
            int patientIndex = hittadeIndex[0];

            printf("Vill du avregistrera patienten (j/n)? ");
            while (getchar() != '\n'); 
            char svar = getchar();

            if (svar == 'j' || svar == 'J')
            {
                 // ta bort patienten
                for (int i = patientIndex; i < (*pPatienter).antalPatienter - 1; i++)
                {
                    (*pPatienter).patienter[i] = (*pPatienter).patienter[i + 1];
                }

                (*pPatienter).antalPatienter--; //minska antalet patient

                printf("Patienten avregistreras.\n");
            } else{
                printf("Avregistrering avbruten.\n");
            }
            return;
        }
    } 
}

int valideraPersonnummer(const char *personnummer)
{
    // kontrollera längd
    if (strlen(personnummer) != 11) {
        printf("Fel format! Personnummer måste vara 10 siffror med ett '-' (YYMMDD-XXXX).\n");
        return 0;
    }

    // kontrollera att de första 6 tecknen är siffror
    for (int i = 0; i < 6; i++) {
        if (personnummer[i] < '0' || personnummer[i] > '9') {
            printf("Fel format! De första 6 tecknen måste vara siffror.\n");
            return 0;
        }
    }

    // kontrollera att sjunde tecknet är '-'
    if (personnummer[6] != '-') {
        printf("Fel format! Det sjunde tecknet måste vara '-'.\n");
        return 0;
    }

    // kontrollera att de sista 4 tecknen är siffror
    for (int i = 7; i < 11; i++) {
        if (personnummer[i] < '0' || personnummer[i] > '9') {
            printf("Fel format! De sista 4 tecknen måste vara siffror.\n");
            return 0;
        }
    }

    return 1;  
}

int konverteraTillFulltDatum(const char *personnummer)
{
    char årStr[3] = {personnummer[0], personnummer[1], '\0'}; // ta ut yy
    int år = atoi(årStr);

    int fulltÅr;
    if (år <= 25) //räknas med ascii
    { 
        fulltÅr = 2000 + år; // 00-25 räknas som 2000-tal tex 01 2001
    } else
    {
        fulltÅr = 1900 + år; 
    }

    // Skapa yyyymmdd manuellt
    int månad = (personnummer[2] - '0') * 10 + (personnummer[3] - '0');
    int dag = (personnummer[4] - '0') * 10 + (personnummer[5] - '0');

    return fulltÅr * 10000 + månad * 100 + dag; // returnera 20010824 som heltal ex
}

void taBortBilder(Patient *pPatienter)
{
    printf("\n");

    // kontrollera om databasen är tom
    if ((*pPatienter).antalPatienter == 0)
    {
        printf("Databasen är tom. Återgår till huvudmenyn.\n");
        return;
    }

    // låt användaren loopa sökningar tills exakt en träff
    while (1)
    {
        int sokVal;
        printf("Sok pa personnummer(1), namn(2), bildreferens(3), avsluta(4): ");
        if (scanf("%d", &sokVal) != 1)
        {
            while (getchar() != '\n');
            printf("Ogiltig inmatning.\n");
            continue;
        }
        if (sokVal == 4)
        {
            return;
        }
        if (sokVal < 1 || sokVal > 3)
        {
            printf("Ogiltigt val. Försök igen.\n");
            continue;
        }

        // läs in söksträng
        while (getchar() != '\n');
        char sokStrang[40];
        if (sokVal == 1)
        {
            printf("Ange personnummer: ");
            scanf("%39[^\n]", sokStrang);
        }
        else if (sokVal == 2)
        {
            printf("Ange sokstrang: ");
            scanf("%39[^\n]", sokStrang);
        }
        else // sokVal == 3
        {
            printf("Ange bildreferens: ");
            scanf("%39[^\n]", sokStrang);
        }

        // anropa vår sökfunktion
        int hittadeIndex[1000];
        int antalFunna = hittaPatienter(pPatienter, sokVal, sokStrang, hittadeIndex, 1000);

        if (antalFunna == 0)
        {
            if (sokVal == 1)
            {
                printf("Personnumret saknas i databasen\n");
            } else if (sokVal == 2)
            {
                printf("Inga patienter hittades med det namnet.\n");
            } else
            {
                printf("Ingen patient hittades med den bildreferensen.\n");
            }
            printf("Du fick inte exakt en traff.\n");
        } else if (antalFunna > 1)
        {
            printf("Du fick inte exakt en traff.\n"); // flera träffar
        } else
        {
            // exakt en träff så dags att ta bort bilder
            int endaIndex = hittadeIndex[0];

            // räkna hur många bilder som finns
            int antalUpptagna = 0;
            for(int j = 0; j < MAX_BILDREFERENSER; j++)
            {
                if((*pPatienter).patienter[endaIndex].bildreferenser[j] != 0){
                    antalUpptagna++;
                }
            }
            if (antalUpptagna == 0)
            {
                printf("Patienten har inga bildreferenser.\n");
                return;
            }

            // nu mata in bildreferenser som ska tas bort
            while (1)
            {
                int refAttTaBort;
                printf("Ange bildreferens att ta bort (eller 0 för att avsluta): ");
                if (scanf("%d", &refAttTaBort) != 1)
                {
                    while (getchar() != '\n');
                    printf("Ogiltig inmatning!\n");
                    continue;
                }

                if (refAttTaBort == 0)
                {
                    printf("Avslutar ta bort bilder.\n");
                    return;
                }

                // försök hitta referensen i patientens bildlista
                int hittad = 0;
                for (int j = 0; j < MAX_BILDREFERENSER; j++)
                {
                    if ((*pPatienter).patienter[endaIndex].bildreferenser[j] == refAttTaBort)
                    {
                        // sätt den till 0 och minska räknaren
                        (*pPatienter).patienter[endaIndex].bildreferenser[j] = 0;
                        antalUpptagna--;
                        hittad = 1;
                        printf("Referensen %d har tagits bort.\n", refAttTaBort);
                        break;
                    }
                }

                if (!hittad)
                {
                    printf("Referensen %d finns inte hos patienten!\n", refAttTaBort);
                }

                if (antalUpptagna == 0)
                {
                    printf("Patienten har inte fler bildreferenser kvar.\n");
                    printf("Avslutar ta bort bilder.\n");
                    return;
                }
            } 
        } 
    } 
}

int kontrolleraKontrollsiffra(const char *personnummer)
{
    // multiplicerar 9 siffror av personnr
    int multipel[9] = {2, 1, 2, 1, 2, 1, 2, 1, 2};
    int total = 0;
    int idx = 0;

    for (int i = 0; i < 11; i++)
    {
        if (i == 6){
            continue;
        }
        if (idx >= 9){
            break;
        }

        int siffra = personnummer[i] - '0'; // ändrar siffran från string till int via asci

        int prod = siffra * multipel[idx++]; // multiplicera enligt mönster (2,1,2,1,2,1,2,1,2) med siffran

        // addera siffersumman av prod
        while (prod > 0)
        {
            total += (prod % 10);
            prod /= 10;
        }
    }

    int kontrollSiffra = (10 - (total % 10)) % 10; // beräkna förväntad kontrollsiffra

    int sista = personnummer[10] - '0'; // sista siffran i personnumret:

    if (sista == kontrollSiffra){
        return 1;
    } else{
        return 0;
    }
}


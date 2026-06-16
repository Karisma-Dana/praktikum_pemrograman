#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


typedef struct 
{
    int day;
    int month;
    int year;
} Date;

typedef struct 
{
    char day[10];
    char triwara[10];
    char pasaran[10];
    char wuku[20];
} Weton;


const char *DAY[] = {"redite", "soma", "anggara", "buda", "wrespati", "sukra", "saniscara"};
const char *PASARAN[] = {"umanis", "paing", "pon", "wage", "kliwon"};
const char *TRIWARA[] = {"pasah", "beteng", "kajeng"};
const char *WUKU[] = {
    "sinta", "landep", "ukir", "kulantir", "taulu", "gumbreg", "wariga", "warigadian",
    "julungwangi", "sungsang", "galungan", "kuningan", "langkir", "medangsia", "pujut",
    "pahang", "krulut", "merakih", "tambir", "medangkungan", "matal", "uye", "menail",
    "perangbakat", "bala", "ugu", "wayang", "kelawu", "dukut", "watugunung"
};
const char* MONTHS[] = {
    "january", "february", "march", "april", "may", "june", 
    "july", "august", "september", "october", "november", "december"
};
const int size_day = sizeof(DAY) / sizeof(DAY[0]);
const int size_pasaran = sizeof(PASARAN) / sizeof(PASARAN[0]);
const int sizeTriwara = sizeof(TRIWARA) / sizeof(TRIWARA[0]);
const int size_wuku = sizeof(WUKU) / sizeof(WUKU[0]);



const Date base_date = {1, 1, 1900};
const int base_day = 1;
const int base_pasaran = 1;
const int base_triwara = 1;
const int base_wuku = 12;


int isKabisat(int year){
    return (year % 4 == 0 && year % 100 != 0 ) || (year % 400 == 0);

}
    
int getMonth(int idx_month, int year){
    int ref_month[12] = {31, 28, 31, 30, 31, 30, 31, 30, 31, 30, 31, 30};
    if (isKabisat(year)) ref_month[1] = 29;
    return ref_month[idx_month - 1];

}

int find_index(const char **array, char *value, int size){
    for(int i = 0; i < size; i++){
        if(strcmp(value, array[i]) == 0){
            return i;
        }
    }
    printf("\ncant find element %s on array, please try again", value);
    return -1;
}


int find_day(int diffrence, char *result_day){
    int idx = (diffrence + base_day) % 7;
    if (result_day == NULL) return idx;
    strcpy(result_day, DAY[idx]);
    return idx;
}

int find_pasran(int difference, char* result_pasaran){
    int idx = (difference + base_pasaran) % 5;
    if (result_pasaran == NULL) return idx;
    strcpy(result_pasaran, PASARAN[idx]);
    return idx;

}

int find_triwara(int difference, char* result_triwara){
    int idx = (difference + base_triwara) % 3;
    if (result_triwara == NULL){
        return idx;
    }
    strcpy(result_triwara, TRIWARA[idx]);
    return idx;
}

int find_wuku(int difference, char* result_wuku){
    int idx = (difference / 7 + base_wuku) % 30;
    if(result_wuku == NULL){
        return idx;
    }
    strcpy(result_wuku, WUKU[idx]);
    return idx;
}

int day_diffrence(Date firstDate, Date endDate){
    int result = 0;

    for (int i = firstDate.year; i < endDate.year; i++){
        result += (isKabisat(i)) ? 366 : 365;
    }

    // eliminate date from first year to fisrt date;
    for (int i = 1; i < firstDate.month; i++){
        result -= (getMonth(i, firstDate.year));
    }

    result -= (firstDate.day - 1);
    for (int i = 1; i < endDate.month; i++){
        result +=(getMonth(i, endDate.year));
    }
    result += (endDate.day - 1);

    return result;
}

int find_weton(Date date, Weton *weton){
    int difference = day_diffrence(base_date, date);

    find_day(difference, weton->day);
    find_pasran(difference, weton->pasaran);
    find_triwara(difference, weton->triwara);
    find_wuku(difference, weton->wuku);
}

int find_birthDay(Weton weton, int firstYear, int endYear){
    int idx_day, idx_pasaran, idx_triwara, idx_wuku;

    

    idx_day = find_index(DAY, weton.day, size_day);
    idx_pasaran = find_index(PASARAN, weton.pasaran, size_pasaran);
    idx_triwara = find_index(TRIWARA, weton.triwara, sizeTriwara);
    idx_wuku = find_index(WUKU, weton.wuku, size_wuku);

    if (idx_day == -1 || idx_pasaran == -1 || idx_triwara == -1 || idx_wuku == -1){
        printf("\nyour weton is not valid, please check again");
    }

    Date current_date;

    printf("\nlooking for birthDay base on your weton : %s %s %s %s ", weton.day, weton.triwara, weton.pasaran, weton.wuku);
    int count_date = 0;
    for(int year = firstYear; year <= endYear; year ++ ){

    
        for(int month = 1;  month <=12; month ++ ){
            int max_day = getMonth(month, year);
            for(int day = 1; day <= max_day; day ++ ){
                current_date.day = day;
                current_date.month = month;
                current_date.year = year;


                int difference = day_diffrence(base_date, current_date);

                int checkDay = find_day(difference, NULL) == idx_day;
                int checkPasaran = find_pasran(difference,NULL ) == idx_pasaran;
                int checkTriwara = find_triwara(difference,NULL ) == idx_triwara;
                int checkWuku = find_wuku(difference,NULL ) == idx_wuku;

                if (checkDay && checkPasaran && checkTriwara && checkWuku){
                    count_date ++;
                    printf("\n%i -> %i %s %i", count_date, current_date.day, MONTHS[month -1], year);
                }
            }
        }
    }

    if (count_date == 0){
        printf("\ncant find your birthDay base on your weton : %s %s %s %s ", weton.day, weton.triwara, weton.pasaran, weton.wuku);
    }
    printf("\n\n");
}

int dateFormating(char *str, Date *date, int order){
    // 1 for str to date
    // 0 for date to str
    switch (order)
    {
    case 1:
        int day, month, year;
        if (sscanf(str,"%d-%d-%d", &day, &month, &year) == 3){
            if ( (day > getMonth(month, year) || day < 1) && (month > 12 || month < 1 ) && (year < 1900) ){
                printf("\n === Your date format wrong and cant find year under 1900");
                return 0;
            }else{
                date->day = day;
                date->month = month;
                date->year = year;
                return 1;
            }
        }
        return 1;
        break;

    case 0:
        sprintf(str,"%02d-%02d-%04d", date->day, date->month, date->year);
        return 1;
        break;

    default:
        break;
    }
}

int lowerCase(char *words){
    for(int i = 0; words[i] != '\0'; i++){
        words[i] = tolower(words[i]);
    }
}


int main(){
    int userPick;
    char userInput[40];

    Date userDate;
    Weton userWeton;
    char date[30];

    while (1)
    {
        printf("\n\nPROGRAMM FINDING YOUR WETON BASE ON YOUR BIRTHDAY, AND VICE VERSA");
        printf("\nchoose the menu by the number");
        printf("\n1. find weton base on your birth day");
        printf("\n2. find birth day base on your weton");
        printf("\n3. out of the program");
        printf("\nwhat do you want to find ? = ");
        scanf("%i", &userPick);

        switch (userPick)
        {
        case 1:
            while (getchar() != '\n');
            printf("\ninput your birth day (format = DD-MM-YYYY) = ");
            fgets(userInput, sizeof(userInput), stdin);

            if(dateFormating(userInput, &userDate, 1) ){
                find_weton(userDate, &userWeton);
                dateFormating(date, &userDate, 0);
                
                printf("\n\nyour weton base on your birthday (%s) : ", date);
                printf("\nDay = %s", userWeton.day);
                printf("\nTriwara = %s", userWeton.triwara);
                printf("\nPasaran = %s", userWeton.pasaran);
                printf("\nWuku = %s", userWeton.wuku);
                printf("\nFULL WETON = %s %s %s %s", userWeton.day, userWeton.triwara, userWeton.pasaran, userWeton.wuku);
            }
            break;
            
        case 2:
            while (getchar() != '\n');

            int firstYear, endYear;
            printf("\n OPTION FIND BIRTHDAT BASE ON YOUR WETON");
            printf("format input : \nDay Triwara Pasaran Wuku \nExample : wrespati Kajeng Kliwon Warigadian");
            printf("\nYour input = ");

            char inputWeton[70];
            fgets(inputWeton, sizeof(inputWeton), stdin);
            inputWeton[strcspn(inputWeton, "\n")] = 0;

            printf("\nGet range year \nfirst year = ");
            scanf("%d", &firstYear);
            printf("\nend year = ");
            scanf("%d", &endYear);

            lowerCase(inputWeton);

            if (sscanf(inputWeton, "%s %s %s %s", userWeton.day, userWeton.triwara, userWeton.pasaran, userWeton.wuku) == 4 && firstYear > 1900 && endYear > 1900){
                find_birthDay(userWeton, firstYear, endYear);
            }else{
                printf("\n check your formating input and the year cant be under 1900");
            }
            break;

        case 3:
            printf("Thank you for using the programm, see you");
            return 0;
            break;
        
        default:
            break;
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "time_func.h"

#define SS_DEFAULT 1 
#define SPL_DEFAULT 30 
#define ITV_DEFAULT 24 

/*
 * Check if an input is a number
 *    0:No    1:Yes
 */
int int_check(char string[100]){
    int n = strlen(string);
    int check = 0, result = 0;
    if (strcmp(string,"\n") == 0 || strcmp(string," ") == 0) result = 0;
    for( int i=0;i<n;i++){
        if((string[i] >= '0') && (string[i] <= '9')) check++;
    }
    if (check == n) result = 1;
return result;
}

int main( int argc, char *argv[] ){
    int error_check = 0;
    int ss = -1, ss_check = 0;                  /* sensors quantity */
    int spl = -1, spl_check = 0;                /* sampling time */
    int itv = -1, itv_check = 0;                /* interval time */
    int ss_rad = -1, spl_rad = -1, itv_rad = -1;    /* position of above variables in command-line */
    int* ss_ptr,*spl_ptr,*itv_ptr;
    ss_ptr = &ss;
    spl_ptr = &spl;
    itv_ptr = &itv;

    FILE *fp_log1= fopen ("task1.log", "a");

    /* C:\\dust_sim –n [num_sensors] –st [sampling] –si [interval] */

    /*
     * find the position of variables in command line
     * if it does not exist, return -1 
     */
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i],"-n") == 0){ss_check++; ss_rad = i;}
        if (strcmp(argv[i],"-st") == 0) {spl_check++; spl_rad = i;}  
        if (strcmp(argv[i],"-si") == 0) {itv_check++; itv_rad = i;}
    }

    if ((argc %2) == 0) /* number of command arguments must be an odd integer */
        error_check = 1;
    else {
        /* 
         * if 0 required argument was found, the count(argc) must be 1
         * if 1 required argument was found, the count(argc) must be 3
         * so on..      
         */
        if (((ss_check + spl_check + itv_check) == 0) && (argc != 1)) error_check = 1;
        else if (((ss_check + spl_check + itv_check) == 1) && (argc != 3)) error_check = 1;
        else if (((ss_check + spl_check + itv_check) == 2) && (argc != 5)) error_check = 1;
        else if (((ss_check + spl_check + itv_check) == 3) && (argc != 7)) error_check = 1;
        if (error_check == 0) {
            /* argument's position must be among 1,3,5 or -1 (non-exist) */    
            if((ss_rad != -1) && (ss_rad != 1) && (ss_rad != 3) && (ss_rad != 5)) error_check = 1;
            else if((spl_rad != -1) && (spl_rad != 1) && (spl_rad != 3) && (spl_rad != 5)) error_check = 1;
            else if((itv_rad != -1) && (itv_rad != 1) && (itv_rad != 3) && (itv_rad != 5)) error_check = 1;
            else {
                if(ss_rad != -1) {
                    if(ss_rad == (argc - 1)) error_check = 2;
                    else {
                        switch (int_check(argv[ss_rad+1])) {
                            case 0: 
                                error_check = 2;
                                break;
                            default :
                                *ss_ptr = (int)atoi(argv[ss_rad+1]);
                                break;
                        }
                    }
                }
                else ss = SS_DEFAULT;

                if(spl_rad != -1) {
                    if(spl_rad == (argc - 1)) error_check = 2;
                    else {
                        switch (int_check(argv[spl_rad+1])) {
                            case 0: 
                                error_check = 2;
                                break;
                            default :
                                *spl_ptr = (int)atoi(argv[spl_rad+1]);
                                break;
                        }
                    }
                }
                else spl = SPL_DEFAULT;

                if(itv_rad != -1){
                    if(itv_rad == argc-1) error_check = 2;
                    else {
                        switch (int_check(argv[itv_rad+1])) {
                            case 0: 
                                error_check = 2;
                                break;
                            default :
                                *itv_ptr = (int)atoi(argv[itv_rad+1]);
                                break;
                        }
                    }
                }
                else itv = ITV_DEFAULT;
            }
        }
    }

    switch (error_check) {
        case 1: 
            fprintf(fp_log1,"Error 01: Invalid command\n");
            break;
        case 2:
            fprintf(fp_log1,"Error 02: Invalid argument\n"); 
            break;
        default:
        {   
            int ss_id;
            FILE *fp_sensor_w = fopen("dust_sensor.csv", "w+");

            /* Computer time */
            time_t rawtime;
            struct tm * timeinfo;
            time (&rawtime);
            timeinfo = localtime (&rawtime);
            struct Time time_end;
            struct Time time_start;
            struct Time *end_ptr = &time_end;
            struct Time *start_ptr = &time_start;
            time_end.year = timeinfo->tm_year + 1900;
            time_end.month = timeinfo->tm_mon + 1;
            time_end.day = timeinfo->tm_mday;
            time_end.hour = timeinfo->tm_hour;
            time_end.min = timeinfo->tm_min;
            time_end.sec = timeinfo->tm_sec;

            time_start_func(end_ptr, itv, start_ptr);

            if (fp_sensor_w == NULL) fprintf(fp_log1, "Error 03: Denied access dust_sensor.csv\n");
            else {
                fprintf(fp_sensor_w, "id,time,values\n");
                struct Time turn_now;
                struct Time turn_next;
                struct Time* turn_now_ptr = &turn_now;
                struct Time* turn_next_ptr = &turn_next;
                turn_now.year = time_start.year;
                turn_now.month = time_start.month;
                turn_now.day = time_start.day;
                turn_now.hour = time_start.hour;
                turn_now.min = time_start.min;
                turn_now.sec = time_start.sec;
                srand((int) time(0));
                for(int i = 1; i <= sample_quantity(itv, spl); i++) {
                    next_sample_time(turn_now_ptr, spl, turn_next_ptr);
                    for(ss_id = 1; ss_id <= ss; ss_id++) {
                        fprintf(fp_sensor_w,"%d,%04d:%02d:%02d %02d:%02d:%02d,%.1f\n", ss_id, turn_now.year, turn_now.month,
                                turn_now.day, turn_now.hour, turn_now.min, turn_now.sec, rand() % 9/10.0+rand()%600);
                    }
                    turn_now.year = turn_next.year;
                    turn_now.month = turn_next.month;
                    turn_now.day = turn_next.day;
                    turn_now.hour = turn_next.hour;
                    turn_now.min = turn_next.min;
                    turn_now.sec = turn_next.sec;
                }
            }
            fclose(fp_sensor_w);
        }
    }
return 0;
}
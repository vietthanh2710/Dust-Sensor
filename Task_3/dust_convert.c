#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time_func.h"
#include "dust_func.h"

int main(int argc, char *argv[]) {
    char first_line[1024], fname[1024];
    int error_check = 0;
    FILE* fp_aqi_r;
    FILE* fp_dat_w;
    FILE *fp_log3= fopen ("task3.log", "a");
    /* 
     * er1: invalid command
     * er2: file not found or cannot be accessed
     * er3: invalid csv file
     * er4: cannot override hex_filename.dat
     * er5: data is missing at line XX
     */
    if(argc == 3) {
        fp_aqi_r = fopen(argv[1], "r");
        fp_dat_w= fopen(argv[2], "w");
    }else error_check = 1;

    if(error_check == 0) {
        if (fp_aqi_r == NULL) error_check = 2;
        else {
            fgets(first_line, 1024, fp_aqi_r);
            if (strcmp(first_line, "id,time,values,aqi,pollution\n") == 0) error_check = 0;
            else error_check = 3;
        }
        if(fp_dat_w == NULL) error_check = 4;

        if (error_check == 1) fprintf(fp_log3, "Error 01: invalid command\n");
        else if (error_check == 2) fprintf(fp_log3, "Error 02: file not found or cannot be accessed\n");
        else if (error_check == 3) fprintf(fp_log3, "Error 03: invalid csv file\n"); 
        else if (error_check == 4) fprintf(fp_log3, "Error 04: Cannot overwrite %s", argv[2]);
    }

    if(error_check == 0) {
        /* 
         * Split raw data to 
         *  + inlier data (use for the following tasks) -> id,values,aqi,time
         *  + notify lines with syntax error -> task3.log  
         */
        FILE* fp_in_w = fopen("dust_inliers.csv", "w+");

        int syn_er_count = 0;
        int valid_check = 0;
        char buffer[1024]; 

        int id, aqi;
        char time[100];
        float values;

        int row = 0;
        int column = 0;
        /* Print appropriate data to inliers file */
        while (fgets(buffer, 1024, fp_aqi_r)) {
            valid_check = 0;
            column = 0;
            row++;
            if (row == 0)
                continue;
            // Splitting the data
            char* get_1 = strtok(buffer, ",");
            while (get_1) {
                /* Column 1: id */
                if (column == 0) {
                    if (id_check(get_1) == 1) 
                        id = atoi(get_1);
                    else valid_check++;
                }
                /* Column 2: time */
                if (column == 1) {
                    if (time_check(get_1) == 1)
                        strcpy(time, get_1);      
                    else valid_check++;           
                }
                /* Column 3: values */
                if (column == 2) {
                    if (values_check(get_1) == 0)
                        valid_check++;
                    else values = atof(get_1);
                }
                /* Column 4: aqi */
                if (column == 3) {
                    if (aqi_check(get_1) == 0)
                        valid_check++;
                    else aqi = atoi(get_1);
                }
                /* Column 5: Pollution status -> skip */

                get_1 = strtok(NULL, ",");
                column++;
            }
            if (valid_check != 0) {
                fprintf(fp_log3,"Error 05: data is missing at line %d\n", row);
                syn_er_count++;
            }else {
                fprintf(fp_in_w,"%d,%.1f,%d,%s\n", id, values, aqi, time);
            } 
        }
        fclose(fp_aqi_r);
        fclose(fp_in_w);
        struct Time time_f;
        struct Time* time_f_ptr = &time_f;

        row = 0;
        /* id,values,aqi,time */
        FILE* fp_in_r = fopen("dust_inliers.csv", "r");
        while (fgets(buffer, 1024, fp_in_r)) {
            column = 0;
            row++;
            if (row == 0)
                continue;
            char* get_2 = strtok(buffer, ",");
            while (get_2) {
                /* Column 3: time */
                if (column == 3) 
                    if (row == 1) str2time(get_2, time_f_ptr);      
                get_2 = strtok(NULL, ",");
                column++;     
            }
        }
        fclose(fp_in_r);

        fp_in_r = fopen("dust_inliers.csv", "r");
        struct Time time_get, temp_t;
        struct Time *time_get_ptr = &time_get;
        struct Time *temp_t_ptr = &temp_t;

        temp_t.year = 2020;
        temp_t.month = 1;
        temp_t.day = 1;
        temp_t.hour = 0;
        temp_t.min = 0;
        temp_t.sec = 0;

        unsigned int offset = 1577836800 + time_diff(temp_t_ptr,time_f_ptr);    /* After this temp_t==time_f */
        /*1577836800 is duration(sec) from 1970:01:01 00:00:00 to 2020:01:01 00:00:00*/

        id=0;
        aqi=0;
        values=0.0;

        char id_hex[100], time_hex[100], aqi_hex[100], values_754[100], length[3];
        char chksum_str[100], chksum_result[3];
        strcpy(length, "0F");
        strcpy(chksum_str, "");
        
        row = 0;
        while (fgets(buffer, 1024, fp_in_r)) {
            strcpy(chksum_str, "");
            column = 0;
            row++;
            if (row == 0)
                continue;
            char* get_3 = strtok(buffer, ",");
            while (get_3) {
                /* Column 1: id */
                if (column == 0) {
                    id=atoi(get_3);
                }
                /* Column 2: values */
                if (column == 1) {
                    values = atof(get_3);
                }
                /* Column 3: aqi */
                if (column == 2) {
                    aqi = atoi(get_3);
                }                
                /* Column 4: time */
                if (column == 3) {
                    str2time(get_3, time_get_ptr);       
                }
                get_3 = strtok(NULL, ",");
                column++;
            }
            dec2hex(id, 1, id_hex);
            dec2hex(offset + time_diff (temp_t_ptr, time_get_ptr), 4, time_hex);
            dec2hex(aqi, 2, aqi_hex);
            float2hex754(values, 4, values_754);

            strcat(chksum_str, length);
            strcat(chksum_str," ");
            strcat(chksum_str, id_hex);
            strcat(chksum_str," ");
            strcat(chksum_str, time_hex);
            strcat(chksum_str," ");
            strcat(chksum_str, values_754);
            strcat(chksum_str," ");
            strcat(chksum_str, aqi_hex);

            checksum(chksum_str, chksum_result);

            /*Reset temp_t to time_f */
            temp_t.year = time_f.year;
            temp_t.month = time_f.month;
            temp_t.day = time_f.day;
            temp_t.hour = time_f.hour;
            temp_t.min = time_f.min;
            temp_t.sec = time_f.sec;

            fprintf(fp_dat_w,"00 %s %s %s %s %s %s FF\n", length, id_hex, time_hex, values_754, aqi_hex, chksum_result);
        }
        fclose(fp_in_r);
        remove("dust_inliers.csv");
    }
return 0;
}
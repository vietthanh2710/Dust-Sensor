#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock.h>
#include "time_func.h"
#include "dust_func.h"

int gettimeofday ( struct timeval *tp ,  struct timezone *tz );
/*
 * return 
 * 0: another 
 * 1: .dat file
 * 2: .csv file
 */
int file_check(char name[100]) {
    int result = 0;
    int column, dot_c = 0;
    for (int i = 0; i < strlen(name); i++) {
        if (name[i] == '.') dot_c++; 
    }
    if (dot_c != 1) result = 0;
    else {
        column = 0;
        char * token = strtok(name, ".");
        while( token != NULL ) {
            if (column == 1) {
                if (strcmp(token,"dat") == 0) result = 1;
                else if (strcmp(token,"csv") == 0) result = 2;
                else result = 0;
            }
            token = strtok(NULL, ".");
            column++;
        }
    }
return result;
}

/*
 * check demanded elements need to be sorted
 * 0: invalid elements 
 * 1: valid elements
 */
int ele_check(char str[100]) {
    int result = 0;
    int comma_c = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == ',') comma_c++; 
    }
    if ((comma_c == 0) && 
        ((strcmp(str,"id") == 0) || (strcmp(str,"time") == 0) || (strcmp(str,"values") == 0))) result = 1;
    if ((comma_c == 1) || (comma_c == 2)) {
        char * token = strtok(str, ",");
        while( token != NULL ) {
            if ((strcmp(token,"id") == 0) ||
                (strcmp(token,"time") == 0) ||
                (strcmp(token,"values") == 0)) 
                result++;
            token = strtok(NULL, ",");
        }
        if (result == (comma_c + 1)) result = 1;
        else result = 0;
    }
return result;
}

int main(int argc, char *argv[]) {
    char first_line[1024], csv_name[100], dat_name[100], unaccessable_name[100];
    char temp_str_1[100], temp_str_2[100];
    int error_check = 0, mode = 0;
    int temp_int_1, temp_int_2;
    FILE* fp_csv;
    FILE* fp_dat;
    FILE *fp_er_log = fopen ("dust_convert_error.log", "a");
    /*
     * Execute mode:
     * 1: .csv to .dat 
     * 2: .dat to .csv 
     */

    /* 
     * er1: invalid command
     * er2: denied access FILENAME
     * er3: invalid file format
     * er4: data is missing at line XX
     * er5: data is duplicated at lines X, Y
     * er6: invalid data packet at line XX
     */
    if(argc >= 3) {
        strcpy(temp_str_1, argv[1]);
        strcpy(temp_str_2, argv[2]);
        temp_int_1 = file_check(temp_str_1);
        temp_int_2 = file_check(temp_str_2);
        if ((temp_int_1 == 1) && (temp_int_2 == 2)) {
            strcpy(dat_name, argv[1]);
            strcpy(csv_name, argv[2]);
            fp_dat = fopen(argv[1], "r");
            fp_csv = fopen(argv[2], "w");
            mode = 2;
        } else if ((temp_int_1 == 2) && (temp_int_2 == 1)) {
            strcpy(dat_name, argv[2]);
            strcpy(csv_name, argv[1]);
            fp_csv = fopen(argv[1], "r");
            fp_dat = fopen(argv[2], "w"); 
            mode = 1;
        } else error_check = 1;
    }else error_check = 1;

    if (error_check == 0) {
        if ((mode == 1) && (argc != 3)) error_check = 1;
        if (mode == 2) {
            if ((argc != 3) && (argc != 5) && (argc != 6)) error_check = 1;
            else if (argc == 3) {error_check = 0;}
            else if (argc == 5) {
                if ((strcmp(argv[3],"-s") != 0)) error_check = 1;
                else if ((strcmp(argv[4],"-asc") != 0) && (strcmp(argv[4],"-dsc") != 0)) error_check = 1;
            }
            else if (argc == 6) {
                char temp_str[100];
                strcpy(temp_str, argv[4]);
                if ((strcmp(argv[3],"-s") != 0)) error_check = 1;
                else if (ele_check(temp_str) == 0) error_check = 1;
                else if ((strcmp(argv[5],"-asc") != 0) && (strcmp(argv[5],"-dsc") != 0)) error_check = 1;
            }
        } 
    }

    if (error_check == 0) {
        if (fp_csv == NULL) {
            error_check = 2;
            strcpy(unaccessable_name, csv_name); 
        } else if (fp_dat == NULL) {
            error_check = 2;
            strcpy(unaccessable_name, dat_name); 
        } else {
            if (mode == 1) {
                fgets(first_line, 1024, fp_csv);
                if (strcmp(first_line, "id,time,values,aqi,pollution\n") == 0) error_check = 0;
                else error_check = 3;
            }else if (mode == 2) {
                // nothing to do
            }
        }
    }
    if (error_check == 1) fprintf(fp_er_log, "Error 01: invalid command\n");
    else if (error_check == 2) fprintf(fp_er_log, "Error 02: denied access %s\n", unaccessable_name);
    else if (error_check == 3) fprintf(fp_er_log, "Error 03: invalid file format\n");

    temp_int_1 = 0;
    temp_int_2 = 0;

    /* Start excecute */
    if(error_check ==0) {
        FILE *fp_run_log = fopen ("dust_convert_run.log", "w");
        FILE *fp_non_dup;
        FILE* fp_in_w;
        char line[100] = {0};
        char line2[100] = {0};
        int diff_check = 0, dup_c = 0;
        unsigned int input_row = 0, non_dup_row = 0;
        int dup_list[100];

        /************************
         * Mode 1: .csv to .dat *
         ************************/
        if (mode == 1) {
            fp_in_w = fopen("dust_inliers.csv", "w+");
            fp_non_dup = fopen("dust_non_dup.csv", "w+");
            /* Notify duplicated lines and remove them */
            while (fgets(line, 80, fp_csv)) {   
                rewind(fp_non_dup);
                diff_check = 0;
                non_dup_row = 0;
                while (fgets(line2, 80, fp_non_dup))
                {   
                    if (strcmp(line, line2) == 0) { 
                        diff_check++;
                        dup_c++;
                        fprintf(fp_er_log, "Error 05: duplicated data at lines %d, %d\n",non_dup_row + dup_c, input_row + 1);
                        dup_list[dup_c - 1] =  input_row + 1;
                        /* Remember fgets the csv's first line before, so line 0 is skipped */                    
                    }
                    non_dup_row++;
                }
                if (diff_check == 0) {
                    fprintf(fp_non_dup,"%s",line);
                }
                input_row++;
            }
            fclose(fp_csv);
            fclose(fp_non_dup);

            /* 
             * Split raw data to 
             *  + inlier data (use for the following tasks) -> id,values,aqi,time
             *  + notify lines with syntax error -> dust_error_log.log  
             */
            fp_non_dup = fopen("dust_non_dup.csv", "r");
            int syn_er_count = 0;
            int valid_check = 0;
            char buffer[1024]; 

            int id, aqi;
            char time[100];
            float values;

            int row = 0;
            int column = 0;
            /* Print appropriate data to inliers file */
            while (fgets(buffer, 1024, fp_non_dup)) {
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
                    temp_int_1 = row;
                    for (int i = 0; i < dup_c; i++){
                        if (temp_int_1 >= dup_list[i]) temp_int_1++; /* Find the original row in the input file */
                    }
                    fprintf(fp_er_log,"Error 05: data is missing at line %d\n", temp_int_1);
                    syn_er_count++;
                }else {
                    fprintf(fp_in_w,"%d,%.1f,%d,%s\n", id, values, aqi, time);
                } 
            }
            fprintf(fp_run_log,"Total number of rows: %d\nSuccesfully converted rows: %d\nError rows: %d",
                    input_row, input_row - dup_c - syn_er_count, dup_c + syn_er_count);
            fclose(fp_non_dup);
            remove("dust_non_dup.csv");
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

            //unsigned int offset = 1577836800 + time_diff(temp_t_ptr,time_f_ptr); /*GMT +00*/
            unsigned int offset = 1577836800 + time_diff(temp_t_ptr,time_f_ptr) - 7*3600; /*GMT +07*/
            /* After this temp_t==time_f */
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

                fprintf(fp_dat,"00 %s %s %s %s %s %s FF\n", length, id_hex, time_hex, values_754, aqi_hex, chksum_result);
            }
            fclose(fp_in_r);
            remove("dust_inliers.csv");
        }
        /***** END OF MODE 1 *****/
        
        /************************
         * MODE 2: .dat to .csv *
         ************************/
        if (mode == 2) {
            fp_in_w = fopen("dust_inliers.dat", "w+");
            fp_non_dup = fopen("dust_non_dup.dat", "w+");
            /* Notify duplicated lines and remove them */
            while (fgets(line, 80, fp_dat)) {   
                rewind(fp_non_dup);
                diff_check = 0;
                non_dup_row = 0;
                while (fgets(line2, 80, fp_non_dup))
                {   
                    if (strcmp(line, line2) == 0) { 
                        diff_check++;
                        dup_c++;
                        fprintf(fp_er_log, "Error 05: duplicated data at lines %d, %d\n",non_dup_row + dup_c -1, input_row);
                        dup_list[dup_c - 1] =  input_row;                   
                    }
                    non_dup_row++;
                }
                if (diff_check == 0) {
                    fprintf(fp_non_dup,"%s",line);
                }
                input_row++;
            }
            fclose(fp_dat);
            fclose(fp_non_dup);

            /* 
             * Split raw data to 
             *  + inlier data (use for the following tasks)
             *  + notify lines with syntax error -> dust_error_log.log  
             */
            fp_non_dup = fopen("dust_non_dup.dat", "r");
            int syn_er_count = 0;
            int valid_check = 0;
            char buffer[1024]; 

            int row = 0;
            int column = 0;
            /* Print appropriate data to inliers file */
            while (fgets(buffer, 1024, fp_non_dup)) {
                valid_check = 0;
                column = 0;
                valid_check = dat_line_check(buffer);
                if (valid_check != 1) {
                    temp_int_1 = row;
                    for (int i = 0; i < dup_c; i++) {
                        if (temp_int_1 >= dup_list[i]) temp_int_1++; /* Find the original row in the input file */
                    }
                    printf("line %d\n", temp_int_1);
                    fprintf(fp_er_log,"Error 06: invalid data packet at line %d\n", temp_int_1);
                    syn_er_count++;
                }else {
                    fprintf(fp_in_w,"%s", buffer);
                } 
                row++;
            }
            fprintf(fp_run_log,"Total number of rows: %d\nSuccesfully converted rows: %d\nError rows: %d\n",
                    input_row, input_row - dup_c - syn_er_count, dup_c + syn_er_count);
            fclose(fp_non_dup);
            remove("dust_non_dup.dat");
            fclose(fp_in_w);
            strcpy(buffer,"");
            
            /* Start execute with valided data */
            struct Time *time = (struct Time*)calloc(row,sizeof(struct Time));
            int *id = (int*)calloc(row,sizeof(int));
            int *time_sec = (int*)calloc(row,sizeof(int));
            float *value = (float*)calloc(row, sizeof(float));
            char id_r[3], time_1[3], time_2[3], time_3[3], time_4[3], value_r[9], aqi_str[100];
            int aqi;
            row = 0;

            /* Converting Section */
            FILE* fp_in_r = fopen("dust_inliers.dat", "r");
            while (fgets(buffer, 1024, fp_in_r)) {
                column = 0;
                strcpy(value_r, "");
                char *spc_deli = strtok(buffer, " ");
                while (spc_deli) {
                    if (column == 2) {
                        /* Converting ID */
                        id[row] = hex2dec(spc_deli);
                        /*****************/
                    }

                    if (column == 3) strcpy(time_1, spc_deli);
                    if (column == 4) strcpy(time_2, spc_deli);
                    if (column == 5) strcpy(time_3, spc_deli);
                    if (column == 6) strcpy(time_4, spc_deli);

                    if ((column >= 7) && (column <= 10)) {
                        strcat(value_r, spc_deli);
                    }
                    spc_deli = strtok(NULL, " ");
                    column++;
                }
                /* Converting Time */
                int d[4];
                d[0] = hex2dec(time_1),
                d[1] = hex2dec(time_2),
                d[2] = hex2dec(time_3),
                d[3] = hex2dec(time_4);
                char temp1[9] = "\0", temp2[9] = "\0", b[33] = "\0";
                for (int i = 0; i < 4; i++) {
                    strcpy(temp1, "");
                    strcpy(temp2, "");        
                    dec2bi(d[i], temp1);
                    if (strlen(temp1) < 8) {
                        for (int j = 0; j < strlen(temp1); j++) {
                            temp2[7 - j] = temp1[strlen(temp1) - 1 - j]; 
                        }
                        for (int j = 0; j < (8 - strlen(temp1)); j++) {
                            temp2[j] = '0'; 
                        }
                        strcat(b, temp2);
                    } else strcat(b, temp1);
                }
                time_sec[row] = bi2dec(b);
                struct tm * timeinfo;
                timeinfo = localtime ((time_t*) &time_sec[row]);
                time[row].year  = timeinfo->tm_year + 1900;
                time[row].month = timeinfo->tm_mon + 1;
                time[row].day   = timeinfo->tm_mday;
                time[row].hour  = timeinfo->tm_hour;
                time[row].min   = timeinfo->tm_min;
                time[row].sec   = timeinfo->tm_sec;

                /* Converting Values*/
                value[row] = hex754_2float(value_r);

                row++; 
            }
            fclose(fp_in_r);
            remove("dust_inliers.dat");

            /* Sorting and printing section*/
            if (argc > 3) {
                int* re_bubble = (int*)calloc(row, sizeof(int));
                int* re_quick = (int*)calloc(row, sizeof(int));
                int sort_sign;                  /* 0: asc    1: dsc */
                char pri_queue[50];
                if (argc == 6) strcpy(pri_queue, argv[4]);
                else strcpy(pri_queue, "id,time,values");

                if (strcmp(argv[argc - 1], "-asc") == 0) sort_sign = 0;
                else sort_sign = 1;

                struct timeval start, end2, end3;
                gettimeofday(&start, NULL);
                quick_vt(id, time_sec, value, row, pri_queue, sort_sign, re_quick); 
                gettimeofday(&end2, NULL); 
                bubble_vt(id, time_sec, value, row, pri_queue, sort_sign, re_bubble);
                gettimeofday(&end3, NULL);

                float quick_ms = 0.0001 * (float)(end2.tv_usec - start.tv_usec);
                float bubble_ms = 0.0001 * (float)(end3.tv_usec - end2.tv_usec);
                fprintf(fp_run_log,"Sorting algorithm quick [ms]: %.2f\n",quick_ms); 
                fprintf(fp_run_log,"Sorting algorithm bubble [ms]: %.2f\n",bubble_ms);

                for (int i = 0; i < row; i++) {
                    if ((aqi_num(value[re_bubble[i]]) >= 0) && (aqi_num(value[re_bubble[i]]) < 50)) {
                        strcpy(aqi_str, "Good");
                    } else if((aqi_num(value[re_bubble[i]]) >= 50) && (aqi_num(value[re_bubble[i]]) < 100)) {
                        strcpy(aqi_str, "Moderate");
                    } else if((aqi_num(value[re_bubble[i]]) >= 100) && (aqi_num(value[re_bubble[i]]) < 150)) {
                        strcpy(aqi_str, "Slightly unhealthy");
                    } else if((aqi_num(value[re_bubble[i]]) >= 150) && (aqi_num(value[re_bubble[i]]) < 200)) {
                        strcpy(aqi_str, "Unhealthy");
                    } else if((aqi_num(value[re_bubble[i]]) >= 200) && (aqi_num(value[re_bubble[i]]) < 300)) {
                        strcpy(aqi_str, "Very unhealthy");
                    } else if((aqi_num(value[re_bubble[i]]) >= 300) && (aqi_num(value[re_bubble[i]]) < 400)) {
                        strcpy(aqi_str, "Hazardous");
                    } else if((aqi_num(value[re_bubble[i]]) >= 400) && (aqi_num(value[re_bubble[i]]) <= 500)){
                        strcpy(aqi_str, "Extremely hazardous");
                    }

                    fprintf(fp_csv,"%2d,%4d:%02d:%02d %02d:%02d:%02d,%.1f,%d,%s\n",
                        id[re_bubble[i]], time[re_bubble[i]].year, time[re_bubble[i]].month, time[re_bubble[i]].day,
                        time[re_bubble[i]].hour, time[re_bubble[i]].min, time[re_bubble[i]].sec, value[re_bubble[i]], aqi_num(value[re_bubble[i]]),aqi_str);
                }


                free(re_bubble);
                free(re_quick);
            } else if (argc == 3){
                for (int i = 0; i < row; i++) {
                    if ((aqi_num(value[i]) >= 0) && (aqi_num(value[i]) < 50)) {
                        strcpy(aqi_str, "Good");
                    } else if((aqi_num(value[i]) >= 50) && (aqi_num(value[i]) < 100)) {
                        strcpy(aqi_str, "Moderate");
                    } else if((aqi_num(value[i]) >= 100) && (aqi_num(value[i]) < 150)) {
                        strcpy(aqi_str, "Slightly unhealthy");
                    } else if((aqi_num(value[i]) >= 150) && (aqi_num(value[i]) < 200)) {
                        strcpy(aqi_str, "Unhealthy");
                    } else if((aqi_num(value[i]) >= 200) && (aqi_num(value[i]) < 300)) {
                        strcpy(aqi_str, "Very unhealthy");
                    } else if((aqi_num(value[i]) >= 300) && (aqi_num(value[i]) < 400)) {
                        strcpy(aqi_str, "Hazardous");
                    } else if((aqi_num(value[i]) >= 400) && (aqi_num(value[i]) <= 500)){
                        strcpy(aqi_str, "Extremely hazardous");
                    }

                    fprintf(fp_csv,"%2d,%4d:%02d:%02d %02d:%02d:%02d,%.1f,%d,%s\n",
                        id[i], time[i].year, time[i].month, time[i].day,
                        time[i].hour, time[i].min, time[i].sec, value[i], aqi_num(value[i]), aqi_str);
                }
            }
        }
    }
return 0;
}
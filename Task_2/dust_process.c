#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "time_func.h"
#include "dust_func.h"

int main(int argc, char *argv[] ) {
    int error_check=0;
    char first_line[1024],fname[1024];
    FILE* fp;
    /*
     * er1: invalid command
     * er2: file not found or cannot be accessed
     * er3: invalid csv file
     * er4: data is missing at line XX
     */
    if(argc == 1) {
        fp = fopen("dust_sensor.csv", "r");
    }else if(argc == 2) { 
        fp = fopen(argv[1], "r");
    }else error_check = 1;

    if (fp == NULL) error_check = 2;
    else{
        fgets(first_line, 1024, fp);
        if (strcmp(first_line,"id,time,values\n") == 0) error_check = 0;
        else error_check = 3;
    }
    
    FILE *fp_log2 = fopen ("task2.log", "a");
    if (error_check == 1) fprintf(fp_log2, "Error 01: invalid command\n");
    else if (error_check == 2) fprintf(fp_log2, "Error 02: file not found or cannot be accessed\n");
    else if (error_check == 3) fprintf(fp_log2, "Error 03: invalid csv file\n");    

    if (error_check == 0) {
        /*  Split raw data to 
         *   + inlier data (use for the following tasks) -> id,values,time
         *   + outlier data (out of sensor's range) -> id,time,values
         *   + notify lines with syntax error -> task2.log \
         */
        FILE* fp_in_w = fopen("dust_inliers.csv", "w+");
        FILE* fp_out_w = fopen("dust_outliers.csv", "w+");
        fprintf(fp_out_w, "\n");
        fprintf(fp_out_w, "id,time,values\n");

        int out_count = 0, syn_er_count = 0;
        int valid_check = 0;                    
        int outlier_check = 0;              
        char buffer[1024];

        int id;
        char time[100];
        float values;
         
        int row = 0;
        int column = 0;
        /* Print appropriate data to inliers and outliers file */
        while (fgets(buffer, 1024, fp)) {
            valid_check = 0;
            outlier_check = 0;
            column = 0;
            row++;
            if (row == 0)
                continue;
            /* Splitting the data */
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
                    else if (values_check(get_1) == -1){
                        values = atof(get_1);
                        outlier_check++;
                        if (valid_check == 0) out_count++;
                    }
                    else if (values_check(get_1) == 1)
                        values = atof(get_1);
                }
                get_1 = strtok(NULL, ",");
                column++;
            }
            if (valid_check!=0) {
                fprintf(fp_log2,"Error 04: data is missing at line %d\n",row);
                syn_er_count++;
            }else {
                if(outlier_check!=0) 
                    fprintf(fp_out_w,"%d,%s,%.1f\n",id, time, values);
                else 
                    fprintf(fp_in_w,"%d,%.1f,%s\n",id, values, time);
            }
        }                                       
        int in_count = row - out_count - syn_er_count;    /* Calculate the row of inliers data */
        fclose(fp);
        fclose(fp_out_w);
        fclose(fp_in_w);

        /* Notify the number of outliers on top of the outliers file */
        FILE *fp_out_r, *fptr2;
        int linectr = 0;
        char str[256];        
        fp_out_r = fopen("dust_outliers.csv", "r");
        fptr2 = fopen("temp.txt", "w");
        while (!feof(fp_out_r)) {
            strcpy(str, "\0");
            fgets(str, 256, fp_out_r);
            if (!feof(fp_out_r)) {
                linectr++;
                if (linectr != 1) 
                    fprintf(fptr2, "%s", str);
                else 
                    fprintf(fptr2, "Number of outliers: %d\n", out_count);
            }
        }
        fclose(fp_out_r);
        fclose(fptr2);
        remove("dust_outliers.csv");
        rename("temp.txt", "dust_outliers.csv");

        /* Get the biggest id and the first-last appeared time in the inliers file */
        struct Time time_f, time_l;
        struct Time *time_f_ptr = &time_f;
        struct Time *time_l_ptr = &time_l;

        row = 0;
        int id_max = 1, temp_int;
        FILE* fp_in_r = fopen("dust_inliers.csv", "r");
        while (fgets(buffer, 1024, fp_in_r)) {
            column = 0;
            row++;
            if (row == 0)
                continue;

            char* get_2 = strtok(buffer, ",");
            while (get_2) {
                /* Column 1: id */
                if (column == 0) {
                    temp_int = atoi(get_2);
                    if (temp_int > id_max) id_max = temp_int;
                }
                /* Column 3: time */
                if (column == 2) {
                    if (row == 1)
                        str2time(get_2, time_f_ptr);      
                    else if (row == in_count) 
                        str2time(get_2,time_l_ptr);             
                }
                get_2 = strtok(NULL, ",");
                column++;
            }          
        }
        fclose(fp_in_r);
        
        //Process inliers data
        fp_in_r = fopen("dust_inliers.csv", "r");
        
        FILE *fp_aqi_w = fopen("dust_aqi.csv","w");
        fprintf(fp_aqi_w, "id,time,values,aqi,pollution\n");

        struct Time temp_get,temp_f,temp_l;
        struct Time *temp_get_ptr = &temp_get;
        struct Time *temp_f_ptr = &temp_f;
        struct Time *temp_l_ptr = &temp_l;

        temp_f.year = time_f.year;
        temp_f.month = time_f.month;
        temp_f.day = time_f.day;
        temp_f.hour = time_f.hour;
        temp_f.min = time_f.min;
        temp_f.sec = time_f.sec;

        temp_l.year = time_f.year;
        temp_l.month = time_f.month;
        temp_l.day = time_f.day;
        temp_l.hour = time_f.hour;
        temp_l.min = 59;
        temp_l.sec = 59;
        
        /* Get the interval time*/
        int sec = 0;
        int si_hour, si_min, si_sec;
        int *si_hour_ptr = &si_hour;
        int *si_min_ptr = &si_min;
        int *si_sec_ptr = &si_sec;
        while ((temp_f.year != time_l.year) || (temp_f.month != time_l.month) || 
               (temp_f.day != time_l.day) || (temp_f.hour != time_l.hour) || 
               (temp_f.min != time_l.min) || (temp_f.sec != time_l.sec)) {
            next_sample_time(temp_f_ptr, 1, temp_f_ptr);
            sec++;
        }
        s2h(sec, si_hour_ptr, si_min_ptr, si_sec_ptr);
        
        temp_f.year = time_f.year; 
        temp_f.month = time_f.month;
        temp_f.day = time_f.day;
        temp_f.hour = time_f.hour;
        temp_f.min = time_f.min;
        temp_f.sec = time_f.sec;
        temp_int=0;
        float temp_float;
        /* For dust_aqi.csv */
        float *count = (float*)calloc(id_max, sizeof(float));
        float *sum = (float*)calloc(id_max, sizeof(float));

        /* For dust_summary.csv */
        float *count_all = (float*)calloc(id_max, sizeof(float));
        float *sum_all = (float*)calloc(id_max, sizeof(float));

        float *max = (float*)calloc(id_max, sizeof(float));
        float *min = (float*)calloc(id_max, sizeof(float));
        for (int i = 1; i <= id_max; i++){
            min[i-1] = 600;   /* initialization is a large value (out of sensor's range) */
        }
        struct Time *max_time = (struct Time*)calloc(id_max,sizeof(struct Time));
        struct Time *min_time = (struct Time*)calloc(id_max,sizeof(struct Time));

        /*  For dust_statistics.csv */
        int *sta_c = (int*)calloc((id_max*7),sizeof(int));
        
        row = 0;
        int flag=0;
        int elm_c=0;
        char aqi_str[30];
        while (fgets(buffer, 1024, fp_in_r)) {
            column = 0;
            row++;
            if (row == 0)
                continue;
            char* get_3 = strtok(buffer, ",");
            while (get_3) {
                /* Column 1: id */
                if (column == 0) {
                    temp_int = atoi(get_3);
                    elm_c++;
                }
                /* Column 2: values */
                if (column == 1) {
                    temp_float = atof(get_3);
                    elm_c++;
                }
                /* Column 3: time */
                if (column == 2) {
                    str2time(get_3, temp_get_ptr);  
                    elm_c++;          
                }
                get_3 = strtok(NULL, ",");
                column++;

                if ((elm_c % 3) == 0) {
                    /* For dust_summary.csv*/
                    for (int i = 1; i <= id_max; i++) {
                        if (temp_int == i) {
                            count_all[i-1] = count_all[i-1] + 1.0;
                            sum_all[i-1] = sum_all[i-1] + temp_float;

                            if (temp_float > max[i-1]) {
                                max[i-1] = temp_float;
                                max_time[i-1].year = temp_get.year;
                                max_time[i-1].month = temp_get.month;
                                max_time[i-1].day = temp_get.day;
                                max_time[i-1].hour = temp_get.hour;
                                max_time[i-1].min = temp_get.min;
                                max_time[i-1].sec = temp_get.sec;
                            }

                            if (temp_float < min[i-1]) {
                                min[i-1] = temp_float;
                                min_time[i-1].year = temp_get.year;
                                min_time[i-1].month = temp_get.month;
                                min_time[i-1].day = temp_get.day;
                                min_time[i-1].hour = temp_get.hour;
                                min_time[i-1].min = temp_get.min;
                                min_time[i-1].sec = temp_get.sec;
                            }
                        }
                    }

                    /* For dust_aqi.csv */
                    if (element_check(temp_get_ptr, temp_f_ptr, temp_l_ptr) == 1) {
                        for (int i = 1; i <= id_max; i++){
                            if(temp_int == i) {
                                count[i-1] = count[i-1] + 1.0;
                                sum[i-1] = sum[i-1] + temp_float;
                            }
                        }
                    }else flag++;
                    if (row == in_count) flag++;
                    if (flag == 1 ) {
                        next_sample_time(temp_l_ptr, 1, temp_l_ptr);
                        for (int i = 0; i < id_max; i++) {
                            if(count[i] != 0) {
                            if ((aqi_num(sum[i] / count[i]) >= 0) && (aqi_num(sum[i] / count[i]) < 50)) {
                                strcpy(aqi_str, "Good");
                                sta_c[i * 7 + 0]++;
                            }
                            else if((aqi_num(sum[i] / count[i]) >= 50) && (aqi_num(sum[i] / count[i]) < 100)) {
                                strcpy(aqi_str, "Moderate");
                                sta_c[i * 7 + 1]++;
                            }
                            else if((aqi_num(sum[i] / count[i]) >= 100) && (aqi_num(sum[i] / count[i]) < 150)) {
                                strcpy(aqi_str, "Slightly unhealthy");
                                sta_c[i * 7 + 2]++;
                            }
                            else if((aqi_num(sum[i] / count[i]) >= 150) && (aqi_num(sum[i] / count[i]) < 200)) {
                                strcpy(aqi_str, "Unhealthy");
                                sta_c[i * 7 + 3]++;
                            }
                            else if((aqi_num(sum[i] / count[i]) >= 200) && (aqi_num(sum[i] / count[i])< 300)) {
                                strcpy(aqi_str, "Very unhealthy");
                                sta_c[i * 7 + 4]++;
                            }
                            else if((aqi_num(sum[i]/count[i]) >= 300) && (aqi_num(sum[i] / count[i])< 400)){
                                strcpy(aqi_str, "Hazardous");
                                sta_c[i * 7 + 5]++;
                            }
                            else if((aqi_num(sum[i] / count[i]) >= 400) && (aqi_num(sum[i] / count[i]) <= 500)){
                                strcpy(aqi_str, "Extremely hazardous");
                                sta_c[i * 7 + 6]++;
                            }
                            fprintf(fp_aqi_w, "%d,%04d:%02d:%02d %02d:%02d:%02d,%.1f,%d,%s\n", i+1, temp_l.year, temp_l.month,
                                    temp_l.day, temp_l.hour, temp_l.min, temp_l.sec, sum[i]/count[i], aqi_num(sum[i]/count[i]), aqi_str);
                            }
                        }
                        flag=0;

                        temp_f.year = temp_l.year;
                        temp_f.month = temp_l.month;
                        temp_f.day = temp_l.day;
                        temp_f.hour = temp_l.hour;
                        temp_f.min = temp_l.min;
                        temp_f.sec = temp_l.sec;

                        temp_l.year = temp_f.year;
                        temp_l.month = temp_f.month;
                        temp_l.day = temp_f.day;
                        temp_l.hour = temp_f.hour;
                        temp_l.min = 59;
                        temp_l.sec = 59;
                        
                        for (int i = 1 ; i <= si_hour; i++) {
                            if (element_check(temp_get_ptr, temp_f_ptr, temp_l_ptr) == 1) {
                                for (int i = 1; i <= id_max; i++) {
                                    if(temp_int == i){
                                        count[i-1] = 1.0;
                                        sum[i-1] = temp_float;
                                    }else{
                                        count[i-1] = 0.0;
                                        sum[i-1] = 0.0;
                                    }
                                }
                            }else {
                                next_sample_time(temp_l_ptr, 1, temp_l_ptr);
                                temp_f.year = temp_l.year;
                                temp_f.month = temp_l.month;
                                temp_f.day = temp_l.day;
                                temp_f.hour = temp_l.hour;
                                temp_f.min = temp_l.min;
                                temp_f.sec = temp_l.sec;

                                temp_l.year = temp_f.year;
                                temp_l.month = temp_f.month;
                                temp_l.day = temp_f.day;
                                temp_l.hour = temp_f.hour;
                                temp_l.min = 59;
                                temp_l.sec = 59;
                                for (int i = 1; i <= id_max; i++) {
                                    if(temp_int == i){
                                        count[i-1] = 1.0;
                                        sum[i-1] = temp_float;
                                    }else{
                                        count[i-1] = 0.0;
                                        sum[i-1] = 0.0;
                                    }
                                }
                            }
                        }
                    } 
                }
            }  
        }
        free(count);
        free(sum);

        /* For dust_summary.csv*/
        FILE *fp_sum_w = fopen("dust_summary.csv", "w");
        fprintf(fp_sum_w, "id,parameters,time,values\n");
        for(int i = 0; i < id_max; i++) {
            fprintf(fp_sum_w, "%d,max,%04d:%02d:%02d %02d:%02d:%02d,%.1f\n", i+1, max_time[i].year, max_time[i].month,
                    max_time[i].day, max_time[i].hour, max_time[i].min, max_time[i].sec, max[i]);
            fprintf(fp_sum_w, "%d,min,%04d:%02d:%02d %02d:%02d:%02d,%.1f\n", i+1, min_time[i].year, min_time[i].month,
                    min_time[i].day, min_time[i].hour, min_time[i].min, min_time[i].sec, min[i]);
            fprintf(fp_sum_w, "%d,mean,%02d:%02d:%02d,%.1f\n", i+1, si_hour, si_min, si_sec, sum_all[i] / count_all[i]);
        }
        /* For dust_statistics.csv    */
        FILE *fp_sta_w = fopen("dust_statistics.csv", "w");
        fprintf(fp_sta_w, "id,pollution,duration\n");
        for(int i = 0; i < (id_max); i++) {
            fprintf(fp_sta_w, "%d,Good,%d\n", i + 1, sta_c[i * 7 + 0]);
            fprintf(fp_sta_w, "%d,Moderate,%d\n", i + 1, sta_c[i * 7 + 1]);
            fprintf(fp_sta_w, "%d,Slightly unhealthy,%d\n", i + 1, sta_c[i * 7 + 2]);
            fprintf(fp_sta_w, "%d,Slightly unhealthy,%d\n", i + 1, sta_c[i * 7 + 3]);
            fprintf(fp_sta_w, "%d,Very unhealthy,%d\n", i + 1, sta_c[i * 7 + 4]);
            fprintf(fp_sta_w, "%d,Hazardous,%d\n", i + 1, sta_c[i * 7 + 5]);
            fprintf(fp_sta_w, "%d,Extremely hazardous,%d\n", i + 1, sta_c[i * 7 + 6]);
        }
        free(count_all); free(sum_all);
        free(max); free(min);
        free(max_time); free(min_time);
        free(sta_c);

        fclose(fp_in_r);
        fclose(fp_aqi_w);
        fclose(fp_sum_w);
        fclose(fp_sta_w);
        remove("dust_inliers.csv");
    }
    return 0;
}
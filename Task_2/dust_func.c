#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "time_func.h"

/* 1. 
 *   0 - 0
 *   12 - 50
 *   35.5 - 100
 *   55.5 - 150
 *   150.5 - 200
 *   250.5 - 300
 *   350.5 - 400
 *   550.5 - 500
 */
int aqi_num(float value) {
    int result;
    if ((value >= 0.0) && (value< 12.0)) result = ceil((value - 0.0) * (50.0 - 0.0) / (12.0 - 0.0) + 0.0);
    else if ((value >= 12.0) && (value < 35.5)) result = round((value - 12.0) * (100.0 - 50.0) / (35.5 - 12.0) + 50.0);
    else if ((value >= 35.5) && (value < 55.5)) result = round((value - 35.5) * (150.0 - 100.0) / (55.5 - 35.5) + 100);
    else if ((value >= 55.5) && (value < 150.5)) result = round((value - 55.5) * (200.0 - 150.0) / (150.5 - 55.5) + 150);
    else if ((value >= 150.5) && (value < 250.5)) result = round((value - 150.5) * (300.0 - 200.0) / (250.5 - 150.5) + 200);
    else if ((value >= 250.5) && (value < 350.5)) result = round((value - 250.5) * (400.0 - 300.0) / (350.5 - 250.5) + 300);
    else if ((value >= 350.5) && (value <= 550.5)) result = round((value - 350.5) * (500.0 - 400.0) / (550.5 - 350.5) + 400);
return result;
}

/* 2. id must be a positive integer 1,2,3...(without 0)
 *    0:invalid     1:valid   
 */
int id_check(char id[100]) {
    int n = strlen(id);
    int check = 0, check_zero, result = 0;
    if ((strcmp(id, "\n") ==0) || (strcmp(id, " ") == 0)) result = 0;
    for (int i = 0; i < n; i++) {
        if ((id[i] >= '0') && (id[i] <= '9')) check++;
    }
    for (int i = 0; i < n; i++) {
        if (id[i] == '0') check_zero++;
    }
    if (check_zero == n) result = 0;
    else if (check == n) result = 1;
return result;
}

/*3.
 *  0:invalid     1:valid   
 */
int time_check(char str[100]){ 
    int result = 1;
    int year, month, day, hour, min, sec;
    int colon_c = 0, spc_c = 0, int_c = 0;
    if (strlen(str) != 19) result = 0;
    else {
        for (int i=0; i < strlen(str); i++) {
            if ((str[i] >= '0') && (str[i] <= '9')) int_c++;
            else if (str[i] == ':') colon_c++;
            else if (str[i] == ' ') spc_c++;
            else if (((str[i] < '0') && (str[i] != ' ') || (str[i] > ':'))) result = 0;
        }
        if ((int_c != 14) || (colon_c != 4) || (spc_c != 1)) result = 0;
    }
    if (result == 1) {
        char str_year[5] = {str[0], str[1], str[2], str[3],'\0'};
        char str_month[3] = {str[5], str[6], '\0'};
        char str_day[3] = {str[8], str[9], '\0'};
        char str_hour[3] = {str[11], str[12], '\0'};
        char str_min[3] = {str[14], str[15], '\0'};
        char str_sec[3] = {str[17], str[18], '\0'};

        year = atoi(str_year);
        month = atoi(str_month);
        day = atoi(str_day);

        hour = atoi(str_hour);
        min = atoi(str_min);
        sec = atoi(str_sec);
        int day_max1 = day_max(year, month);
        if ((day <= day_max1) && (day >= 1) && (month >= 1) && (month <= 12) && (year >= 1)) result = 1;
        else result = 0;
        if ((hour>23) || (min>59) || (sec>59)) result = 0;
    }
return result;
}

/* 4. Check the sensor value 
 *    0: not a float
 *    -1: float but outlier
 *    1: float and inlier   
 */
int values_check(char str[100]) {
    int result = 1;
    int zero_check = 0;
    int check = 0, check_dot = 0;
    if ((strcmp(str,"\n") == 0) || (strcmp(str," ")==0) || (strcmp(str,"")==0)) result = 0;
                                                //
    /* check how many '0' or '.' */  
    for (int i = 0; i < (strlen(str) - 1); i++) {   
        if (str[i] == '0') check++;
        else if(str[i] == '.') check_dot++;
    }    
    /* check if input equals to (float)0.0 */  
    if (((check == (strlen(str) - 2)) && (check_dot == 1)) || (check == (strlen(str)-1))) 
        zero_check = 1; 
    else {
        if (atof(str) == 0) result = 0;         /* if input ain't (float)0 but atof() return to 0.000 => not a float */
        else if (check_dot > 1) result = 0;     /* if there are more than 1 dot => not a float */
    } 
    for (int i = 0; i < (strlen(str) - 1); i++) {
        /* if any character different from '.' '0->9' => not a float */
        if(((str[i] < '0') && (str[i] != '.') && (str[i] != '-'))|| (str[i]>'9')) result = 0; 
    }
    if (result == 1) {
        if ((atof(str) >= 5) && (atof(str) <= 550.5)) result = 1;
        else result = -1;
    } 
return result;
}

/* 5. aqi must be an integer number 0-500
 *    0:invalid     1:valid   
 */
int aqi_check(char aqi[100]) {
    int n = strlen(aqi);
    int check = 0, check_zero, result = 0;
    if ((strcmp(aqi,"\n") == 0) || (strcmp(aqi," ") == 0) || (strcmp(aqi, "\0") == 0)) result = 0;
    for (int i = 0; i < n; i++) {
        if((aqi[i] >= '0') && (aqi[i] <= '9')) check++;
    }
    for (int i = 0; i < n; i++) {
        if(aqi[i] == '0') check_zero++;
    }
    if (check_zero == n) result = 1;
    else if (check == n) result = 1;
    if ((result == 0) && ((atoi(aqi) < 0) || (atoi(aqi) > 500))) result = 1;
return result;
}

/*6.*/
void dec2hex (int decimalNumber, int bytes_num, char* hex_bytes_display) {
    strcpy(hex_bytes_display,"");
	long int quotient;
	int i = 1, j, temp;
	char hexadecimalNumber[100];
	quotient = decimalNumber;
	while (quotient != 0) {
		temp = quotient % 16;
		if (temp < 10) temp = temp + 48;
        else temp = temp + 55;
		hexadecimalNumber[i++] = temp;
		quotient = quotient / 16;
	}
    int x = 0;
    char hex_temp[100], temp_str[100];
	for (j = i -1 ; j > 0; j--) {
	    hex_temp[x] = hexadecimalNumber[j];  
        if(x != (i-2)) x++; 
    }
    hex_temp[x+1] = '\0'; 

    /*
     * if hexadecimal formation has odd elements => add one 0 before it
     * Eg: 123AB -> 0123AB
     */
    if((strlen(hex_temp) % 2) != 0) {
       strcpy(temp_str, "0");
       strcat(temp_str, hex_temp);
       strcpy(hex_temp, temp_str); 
    }   
    /*
     * Add space between each 2-digits
     * Eg: 12AB -> 12 AB 
     */
    char hex_display[100];
    int spc;
    int spc_c = 0;
    if ((strlen(hex_temp) % 2) == 0) {
        spc = strlen(hex_temp) / 2 - 1;
        int spc_c = 0;
        for (int i = 0; i < strlen(hex_temp); i++) {
            hex_display[i+spc_c] = hex_temp[i];
            if ((i + spc_c) < (strlen(hex_temp) + spc)) {
                if ((i + spc_c + 1) == (-1 + 3 * (spc_c + 1))) {
                    hex_display[-1 + 3 * (spc_c + 1)] = ' ';
                    spc_c++;
                }
            }
        } 
        hex_display[strlen(hex_temp) + spc] = '\0';
    }
    /* 
     * Display the hex with the length is defined 
     * Eg: 12 AB -> 00 00 12 AB (4-byte length)
     * But if the ordered length is shorter than default -> won't change 
     */
    if (((strlen(hex_display) + 1) / 3) < bytes_num) {
        for (int i = 1; i <= (bytes_num - ((strlen(hex_display) + 1) / 3)); i++){
            strcat(hex_bytes_display, "00 ");
        }
        strcat(hex_bytes_display, hex_display);
    }
    else strcpy(hex_bytes_display, hex_display);
}

/* 7. Use for ieee754 with a real num smaller than 1 */
void float2bi (double realNumber, char* binary) {
    strcpy(binary,"00000000000000000000000");
    int single_pre = (int) (realNumber * pow(2, 23));
    for (int i = 22; i >= 0; i--){
        if ((single_pre - pow(2,i)) >= 0) { 
            single_pre = single_pre - pow(2,i);
            binary[22 - i] = '1';
        }
    }
}
/* 8. */
void dec2bi (int decimalNumber, char* binary) {
    if (decimalNumber == 0) strcpy(binary, "0");
    else {  
        strcpy(binary, "");
        long int quotient;
        int i = 1, j, temp;
        char Number[100];
        quotient = decimalNumber;
        while (quotient != 0) {
            temp = quotient % 2;
            temp =temp + 48;
            Number[i++] = temp;
            quotient = quotient / 2;
        }
        int x = 0;
        char temp_str[100];
        for (j = i - 1 ; j > 0; j--) {
            temp_str[x] = Number[j];  
            if(x != (i - 2)) x++; 
        }
        temp_str[x+1] = '\0'; 
        strcpy(binary, temp_str);
    }
}
/* 9.Just for a real num greater than 1.0 */
void float2hex754(double realNumber, int bytes_num, char* hex_bytes_display) {
    int dec = (int)realNumber;
    double after_point = realNumber - dec;
    char dec_bi[100], after_point_bi[24], real_bi[33], temp_str[100];
    strcpy(real_bi, "0");
    dec2bi(dec, dec_bi);
    float2bi(after_point, after_point_bi);
  
    int e = 126 + strlen(dec_bi);
    char e_bi[9];
    dec2bi(e, e_bi);
    if (strlen(e_bi) < 8) {
        strcpy(temp_str,"0");
        strcat(temp_str, e_bi);
        strcpy(e_bi, temp_str);
    }
    strcpy(temp_str, "\0");
    temp_str[23] = '\0';

    for (int i = 1; i < strlen(dec_bi); i++) {
        temp_str[i-1] = dec_bi[i];
    }
    for (int i = 0; i < (24 - strlen(dec_bi)); i++) {
        temp_str[i + strlen(dec_bi) - 1] = after_point_bi[i];
    }
    strcat(real_bi, e_bi);
    strcat(real_bi, temp_str);    
    /* received the ieee-754 binary formation of real number input */

    /* Convert binary to hexadecimal */
    unsigned int dec_result = 0;
    for (int i = 0; i < 32; i++){
        if (real_bi[31 - i] == '1') dec_result = dec_result + pow(2, i);
    }
    dec2hex (dec_result, bytes_num, hex_bytes_display);
}

/* 10. Just for 1-byte hex */
int hex2dec (char hex2[3]) {
    int digit[2];
    for (int i = 0; i < 2; i++) {
        if (((int)hex2[i] >= 48) && ((int)hex2[i] <= 57)) digit[i] = (int)hex2[i] - 48;
        else digit[i] = (int)hex2[i] - 55;
    }
return (digit[0] * pow(16, 1) + digit[1] * pow(16, 0));
}

/* 11. */
int bi2dec (char bi[100]) {
    int result = 0;
    int *digit = (int*)calloc(strlen(bi),sizeof(int));
    for (int i = 0; i < strlen(bi); i++) {
        digit[strlen(bi) - i - 1] = (int)bi[i] - 48;
    }
    for(int i = 0; i < strlen(bi); i++) {
        result = result + digit[i] * pow(2, i);
    }
    free(digit);
return result;
}

/* 12. binary to 2-complement to hex */
void bi2twocomp2hex(char bi[100], char* result) {
    char temp[9];
    int mark = 0;
    temp[8] = '\0';
    for (int i = 0; i < 8; i++) {
        temp[7 - i] = bi[strlen(bi) - 1 - i];
    }
    for (int i = 7; i >= 0; i--) {
        if(temp[i] == '1') {mark = i; break;}
    }
    for (int i = mark - 1; i >= 0; i--) {
        if (temp[i] == '1') temp[i] = '0';
        else temp[i] = '1';
    }
    dec2hex(bi2dec(temp), 1, result);
}

/* 13. */
void checksum (char hex_12[100], char* result) {
    int sum = 0;
    char binary[100];
    char * token = strtok(hex_12, " ");
    while( token != NULL ) {
        sum = sum + hex2dec(token);
        token = strtok(NULL, " ");
    }
    dec2bi(sum, binary);
    bi2twocomp2hex(binary, result);
}

/* 14. */
float afterdot_bi2float(char str[100]) {
    float result = 0;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '1') result = result + pow(2, - (i + 1));
    }
return result;
}

/* 15. */
float hex754_2float (char str[9]) {
    float result = 0;
    char h1[3] = {str[0], str[1], '\0'},
         h2[3] = {str[2], str[3], '\0'},
         h3[3] = {str[4], str[5], '\0'},
         h4[3] = {str[6], str[7], '\0'};
    int d[4];
    d[0] = hex2dec(h1),
    d[1] = hex2dec(h2),
    d[2] = hex2dec(h3),
    d[3] = hex2dec(h4);
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
    /* b[0] is the float sign*/
    char exp_b[9] = {b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], '\0'};
    char man_b[24] = {b[9], b[10], b[11], b[12], b[13], b[14], b[15], b[16], 
                    b[17], b[18], b[19], b[20], b[21], b[22], b[23], b[24], 
                    b[25], b[26], b[27], b[28], b[29], b[30], b[31], '\0'};
    int exp_result = bi2dec(exp_b) - 127;
    float man_result = afterdot_bi2float(man_b);
    result = (1 + man_result) * pow(2, exp_result);
    if (b[0] == '1') result = 0 - result;
    return result;
}


/* 16. 
 * Return:
 * 1: correct form
 * 0: others
 */
int dat_line_check(char str[100]){ 
    int result = 1;
    int spc_c = 0, char_c = 0;
    if (strlen(str) != 45) result = 0;
    else {
        for (int i = 0; i < strlen(str) - 1; i++) {
            if (((str[i] >= '0') && (str[i] <= '9')) || ((str[i] >= 'A') && (str[i] <= 'F'))) char_c++;
            else if (str[i] == ' ') spc_c++;
            else result = 0;
        }
        if ((char_c != 30) || (spc_c != 14)) result = 0;
    }
    if (result == 1) {
        for (int i = 2; i <= 41; i = i + 3){
            if(str[i] != ' ') result = 0;
        }
    }
    if (result == 1) {
        char start_r[3] = {str[0], str[1], '\0'};
        char length_r[3] = {str[3], str[4], '\0'};
        char id_r[3] = {str[6], str[7], '\0'};
        char value_r[9] = {str[21], str[22], str[24], str[25], str[27], str[28], str[30], str[31], '\0'};
        char aqi_r[6] = {str[33], str[34], str[35], str[36], str[37], '\0'};
        char chksum_r[3] = {str[39], str[40], '\0'};
        char stop_r[3] = {str[42], str[43], '\0'};

        char chksum_str_r[36] = {str[3], str[4], ' ', str[6], str[7], ' ',
        str[9], str[10], ' ', str[12], str[13], ' ', str[15], str[16], ' ',str[18], str[19], ' ',
        str[21], str[22], ' ', str[24], str[25], ' ',str[27], str[28], ' ',str[30], str[31], ' ',
        str[33], str[34], ' ', str[36], str[37], '\0'};
        
        char chksum_cal[100], aqi_cal[6];
        float value_fl;
        if ((strcmp(start_r, "00") != 0) ||
            (strcmp(length_r, "0F") != 0) || 
            (strcmp(stop_r, "FF") != 0)) {
            result = 0;
        } else {
            checksum(chksum_str_r,chksum_cal);
            if (strcmp(chksum_cal,chksum_r) != 0) result = 0;
        }
        
        if (result == 1) {
            time_t now_sec;
            int time_sec_cal;
            now_sec = time(NULL);
            char time_1[3] = {str[9], str[10], '\0'},
                 time_2[3] = {str[12], str[13], '\0'},
                 time_3[3] = {str[15], str[16], '\0'},
                 time_4[3] = {str[18], str[19], '\0'};
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
            time_sec_cal = bi2dec(b);
            if (time_sec_cal > now_sec) result = 0;

            value_fl = hex754_2float(value_r);
            dec2hex(aqi_num(value_fl), 2, aqi_cal);
            if (strcmp(aqi_cal, aqi_r) != 0) result = 0;
        }
    }
return result;
}

void swap_int(int *a, int *b) {
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

void swap_float(float *a, float *b) {
    float temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

/* 17 */
void bubble_vt(int *id, int *time, float *values, int num,
                  char *pri_queue, int sort_sign, int *result) {
    int *re = (int*)calloc(num,sizeof(int));
    for(int i = 0; i < num; i++) {
        re[i] = i;
    }
    /* Determine order of precedence */
    int pri[3] = {0, 0, 0},
        column = 0;
    char pri_copy[50];
    strcpy(pri_copy,pri_queue);
    char *pri_deli = strtok(pri_copy, ",");
    while (pri_deli){
        if (strcmp(pri_deli, "id") == 0) pri[column] = 1;
        else if (strcmp(pri_deli, "time") == 0) pri[column] = 2;
        else if (strcmp(pri_deli, "values") == 0) pri[column] = 3;
        pri_deli = strtok(NULL, ",");
        column++;
    }
    if (column == 1) {
        if (pri[0] == 1) pri[1] = 2;
        else if (pri[0] == 2) pri[1] = 1;
        else if (pri[0] == 3) pri[1] = 1;
    }
    if ((pri[0] != 1) && (pri[1] != 1)) pri[2] = 1; 
    else if ((pri[0] != 2) && (pri[1] != 2)) pri[2] = 2;
    else if ((pri[0] != 3) && (pri[1] != 3)) pri[2] = 3;  
    /* Store data */
    float *data = (float*)calloc(3 * num,sizeof(float));
    for (int j = 0; j < 3; j++) {
        switch (pri[j]) {
            case 1: /* id */ 
                for (int i = 0; i < num; i++) {
                    data[i + j * num] = (float)id[i];
                }
            break;
            case 2: /* time */ 
                for (int i = 0; i < num; i++) {
                    data[i + j * num] = (float)time[i];
                }
            break;
            default: /* values */ 
                for (int i = 0; i < num; i++) {
                    data[i + j * num] = (float)values[i];
                }     
            break;       
        }
    }
    /* Bubble algorithym */
    int sorted_c = 0;
    int t1 = 0;
    float t2,t3,t4;
    while (sorted_c < num) {
        for (int j = 0; j < (num - sorted_c - 1); j++) {
            if ( (data[j + 1] < data[j]) || 
                ((data[j + 1] == data[j]) && (data[j + 1 + 1 * num] < data[j + 1 * num])) ||
                ((data[j + 1] == data[j]) && (data[j + 1 + 1 * num] == data[j + 1 * num]) && (data[j + 1 + 2 * num] < data[j + 2 * num]))
               ) {
                swap_int(&re[j + 1], &re[j]);
                swap_float(&data[j + 1], &data[j]);
                swap_float(&data[j + 1 + 1 * num], &data[j + 1 * num]);
                swap_float(&data[j + 1 + 2 * num], &data[j + 2 * num]);
            }
        }
        sorted_c++;
    }
    free (data);
    for(int i = 0; i < num; i++) {
        if (sort_sign == 1) result[i] = re[num - 1 - i];
        else result[i] = re[i];
    }
    free (re);
}

/* 18. */
void quick_loop(float *data, int num, int start, int stop, int *result) {
    int *re = (int*)calloc(num, sizeof(int));
    for(int i = 0; i < num; i++) {
        re[i] = result[i];
    }
    int pivot = stop;
    int small = start;
    int big = stop - 1;
    for (int i = start; i < stop; i++) {
        if ((data[i] < data[pivot]) || 
            ((data[i] == data[pivot]) && (data[i + 1 * num] < data[pivot + 1 * num])) ||
            ((data[i] == data[pivot]) && (data[i + 1 * num] == data[pivot+ 1 * num]) && (data[i+ 2 * num] < data[pivot + 2 * num])) ) {
            swap_int(&re[i], &re[small]);
            swap_float(&data[i], &data[small]);
            swap_float(&data[i + 1 * num], &data[small + 1 * num]);
            swap_float(&data[i + 2 * num], &data[small + 2 * num]);
            small++;
        }else if ((data[i] > data[pivot]) || 
                  ((data[i] == data[pivot]) && (data[i + 1 * num] > data[pivot + 1 * num])) ||
                  ((data[i] == data[pivot]) && (data[i + 1 * num] == data[pivot + 1 * num]) && (data[i+ 2 * num] > data[pivot + 2 * num]))){
            swap_int(&re[i], &re[big]);
            swap_float(&data[i], &data[big]);
            swap_float(&data[i + 1 * num], &data[big + 1 * num]);
            swap_float(&data[i + 2 * num], &data[big + 2 * num]);
            big--;
            i--;
            stop--;
        }
    }
    swap_int(&re[pivot], &re[small]);
    swap_float(&data[pivot], &data[small]);
    swap_float(&data[pivot + 1 * num], &data[small + 1 * num]);
    swap_float(&data[pivot + 2 * num], &data[small + 2 * num]);
    for(int i = 0; i < num; i++) {
        result[i] = re[i];
    }
    if (((small - 1) - start) > 0) {
        quick_loop(data, num, start, small - 1, result);
    }
    if ((pivot - (small + 1)) > 0) {
        quick_loop(data, num, small + 1, pivot, result);
    }
    free (re);
}

/* 19. */
void quick_vt(int *id, int *time, float *values, int num,
                  char *pri_queue, int sort_sign, int *result) {
    int *re = (int*)calloc(num, sizeof(int));
    for(int i = 0; i < num; i++) {
        re[i] = i;
    }
    /* Determine order of precedence */
    int pri[3] = {0, 0, 0},
        column = 0;
    char pri_copy[50];
    strcpy(pri_copy,pri_queue);
    char *pri_deli = strtok(pri_copy, ",");
    while (pri_deli){
        if (strcmp(pri_deli, "id") == 0) pri[column] = 1;
        else if (strcmp(pri_deli, "time") == 0) pri[column] = 2;
        else if (strcmp(pri_deli, "values") == 0) pri[column] = 3;
        pri_deli = strtok(NULL, ",");
        column++;
    }
    if (column == 1) {
        if (pri[0] == 1) pri[1] = 2;
        else if (pri[0] == 2) pri[1] = 1;
        else if (pri[0] == 3) pri[1] = 1;
    }
    if ((pri[0] != 1) && (pri[1] != 1)) pri[2] = 1; 
    else if ((pri[0] != 2) && (pri[1] != 2)) pri[2] = 2;
    else if ((pri[0] != 3) && (pri[1] != 3)) pri[2] = 3;  
    /* Store data */
    float *data = (float*)calloc(3 * num,sizeof(float));
    for (int j = 0; j < 3; j++) {
        switch (pri[j]) {
            case 1: /* id */ 
                for (int i = 0; i < num; i++) {
                    data[i + j * num] = (float)id[i];
                }
            break;
            case 2: /* time */ 
                for (int i = 0; i < num; i++) {
                    data[i + j * num] = (float)time[i];
                }
            break;
            default: /* values */ 
                for (int i = 0; i < num; i++) {
                    data[i + j * num] = (float)values[i];
                }     
            break;       
        }
    }
    quick_loop(data, num, 0, num - 1, re);
    free(data);
    for(int i = 0; i < num; i++) {
        if (sort_sign == 1) result[i] = re[num - 1 - i];
        else result[i] = re[i];
    }
    free (re);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEC_MAX 59
#define MIN_MAX 59
#define HOUR_MAX 23

struct Time {
    int year,month,day;
    int hour,min,sec;
};
/* 1. Check if (now) is between (first) and (last) (include 2 ends) ?
 *    0: no    1:yes 
 */
int element_check(struct Time* now, struct Time* first, struct Time* last) {
   if (((*now).year == (*first).year) && ((*now).month == (*first).month) && 
       ((*now).day == (*first).day) && ((*now).hour == (*first).hour)) {
      if (((*now).min > (*first).min) && ((*now).min <= (*last).year)) return 1;
      else if(((*now).min == (*first).min) && ((*now).sec >= (*first).sec)) return 1;
   }else return 0;
return 0;
}

/* 2. Convert a string "YYYY:MM:DD hh:mm:ss" to defined Time structure */
void str2time(char str[100], struct Time *time_ptr) {
    int count = 0;
    char temp[100];
    strcpy(temp, str);
    char* pch;
    pch = strtok (temp, " :");
    while (pch != NULL) {  
        if(count == 0)
            (*time_ptr).year = atoi(pch);
        else if(count == 1)
            (*time_ptr).month = atoi(pch);
        else if(count == 2)
            (*time_ptr).day = atoi(pch);
        else if(count == 3)
            (*time_ptr).hour = atoi(pch);
        else if(count == 4)
            (*time_ptr).min = atoi(pch);
        else if(count == 5)
            (*time_ptr).sec = atoi(pch);
        pch = strtok (NULL, " :");
        count++;
    } 
}
/* 3. Convert interval_time(hour) = (day) + (hour) */
void h2d (int interval_time, int* day_ptr, int* hour_ptr) {
    int day = interval_time / 24;
    int hour = interval_time % 24; 
    *day_ptr = day;
    *hour_ptr = hour;
}
/* 4. Convert sampling_time(sec) = (hour) + (min) + (sec) */
void s2h (int spl_time, int* hour_ptr, int* min_ptr, int* sec_ptr) {
    int hour = spl_time / 3600;
    int min = (spl_time - hour * 3600) / 60;
    int sec = spl_time - hour * 3600 - min * 60; 
    *hour_ptr = hour;
    *min_ptr = min;
    *sec_ptr = sec;
}
/* 5. Find the maximum days of a month */
int day_max(int year, int month) {
    int max;
    switch(month) {
        case 4: case 6: case 9: case 11:
            max = 30;
            break;
        case 2:
            if ((((year) % 4 == 0) && ((year) %100 !=0)) || (year % 400 == 0)) 
                max = 29;
            else 
                max =28;
            break;
        default:
                max = 31;
            break;
    }
return max;
}

/* 6. Find the previous day
 *    0: in the same month  
 *    1: last day of the previous month
 *    2: last day of the previous year
 */
int previous_date_check(struct Time *now_ptr) {
    int check = 0;
    if((*now_ptr).day > 1) check = 0;
    else if(((*now_ptr).day == 1) && ((*now_ptr).month != 1)) check = 1;
    else check = 2; 
return check;    
}

/* 7. */ 
void previous_date (struct Time *now_ptr, struct Time *pre_ptr) {   
    int check = previous_date_check(now_ptr);
    int pre_day, pre_month, pre_year;
    switch (check) {
        case 0: 
            pre_day = (*now_ptr).day - 1;
            pre_month = (*now_ptr).month;
            pre_year = (*now_ptr).year;
            break;
        case 1: 
            pre_day = day_max((*now_ptr).year, ((*now_ptr).month) -1);
            pre_month = (*now_ptr).month -1;
            pre_year = (*now_ptr).year;
            break;
        default: 
            pre_day = day_max(((*now_ptr).year-1), 12);
            pre_month = 12;
            pre_year = (*now_ptr).year -1;
            break;
    }
    (*pre_ptr).year = pre_year;
    (*pre_ptr).month = pre_month;
    (*pre_ptr).day = pre_day;
    (*pre_ptr).hour = (*now_ptr).hour;
    (*pre_ptr).min = (*now_ptr).min;
    (*pre_ptr).sec = (*now_ptr).sec;
}

/* 8. Find the following day
 *    0: default
 *    1: first day of the following month  
 *    2: first day of the following year 
 */ 
int next_date_check(struct Time *x) {
    int next_day_check = 0;
        if((*x).day < day_max((*x).year, (*x).month)) next_day_check = 0;
        else if(((*x).day == day_max((*x).year,(*x).month)) && ((*x).month != 12)) next_day_check = 1;
        else next_day_check = 2; 
return next_day_check;    
}

/* 9. */
void next_date (struct Time *now_ptr, struct Time *next_ptr) {   
    int check = next_date_check(now_ptr);
    switch (check) {
        case 0: 
            (*next_ptr).day = (*now_ptr).day + 1;
            (*next_ptr).month = (*now_ptr).month;
            (*next_ptr).year = (*now_ptr).year;
            break;
        case 1: 
            (*next_ptr).day = 1;
            (*next_ptr).month = (*now_ptr).month + 1;
            (*next_ptr).year = (*now_ptr).year;
            break;
        default: 
            (*next_ptr).day = 1;
            (*next_ptr).month = 1;
            (*next_ptr).year = (*now_ptr).year + 1;
            break;
    }
    (*next_ptr).hour = (*now_ptr).hour;
    (*next_ptr).min = (*now_ptr).min;
    (*next_ptr).sec = (*now_ptr).sec;
}

/* 10. Interval_time(hour) = k*sampling_time(sec) + k0 */
int sample_quantity(int itv, int spl){
    int itv_sec = itv * 3600;
return (itv_sec / spl + 1);
}

/* 11. */
void time_start_func (struct Time *time_end, int interval_time, struct Time *time_start) {
    int itv_day, itv_hour;
    int* itv_day_ptr = &itv_day;
    int* itv_hour_ptr = &itv_hour;
    h2d(interval_time, itv_day_ptr, itv_hour_ptr);

    struct Time temp1, temp2;
    struct Time *temp1_ptr = &temp1;
    struct Time *temp2_ptr = &temp2;
    temp1.year = (*time_end).year;
    temp1.month = (*time_end).month;
    temp1.day = (*time_end).day;

    if (itv_hour <= (*time_end).hour)  
        (*time_start).hour = (*time_end).hour - itv_hour;
    else {
        itv_day++;
        (*time_start).hour = 24 - (itv_hour - (*time_end).hour);
    }
    if (itv_day > 0) {
        for (int i = 1; i <= itv_day; i++) {
            previous_date (temp1_ptr, temp2_ptr);
            temp1.year = temp2.year;
            temp1.month = temp2.month;
            temp1.day = temp2.day;
        }
    }else {
        temp2.year = (*time_end).year;
        temp2.month = (*time_end).month;
        temp2.day = (*time_end).day;
    }
    (*time_start).year = temp2.year;
    (*time_start).month = temp2.month ;
    (*time_start).day = temp2.day;
    (*time_start).min = (*time_end).min;
    (*time_start).sec = (*time_end).sec;
}

/* 12. */
void next_sample_time(struct Time *now, int sampling_time, struct Time *next){
    int hour, min, sec;
    int *hour_ptr = &hour, *min_ptr = &min, *sec_ptr = &sec;
    
    s2h(sampling_time, hour_ptr, min_ptr, sec_ptr);

    (*next).year = (*now).year;
    (*next).month = (*now).month;
    (*next).day = (*now).day;
    (*next).hour = (*now).hour;
    (*next).min = (*now).min;
    (*next).sec = (*now).sec;
    
    if(((*now).sec + sec) > SEC_MAX) {
        if(((*now).min + min) >= MIN_MAX) {
            if(((*now).hour + hour) >= HOUR_MAX) {
                next_date(now, next);
                (*next).sec = ((*now).sec + sec) - 60;
                (*next).min = ((*now).min + min) - 60 + 1;
                (*next).hour = ((*now).hour + hour) - 24 + 1;
            }else {
                (*next).sec = ((*now).sec + sec) - 60;
                (*next).min = ((*now).min + min) - 60 + 1;
                (*next).hour = (*now).hour + hour + 1;
            }
        }else {
            if(((*now).hour + hour) > HOUR_MAX) {
                next_date(now, next);
                (*next).sec = ((*now).sec + sec) - 60;
                (*next).min = ((*now).min + min) + 1;
                (*next).hour = ((*now).hour + hour) - 24;
            }else {
                (*next).sec = ((*now).sec + sec) - 60;
                (*next).min = ((*now).min + min) + 1;
                (*next).hour = (*now).hour + hour;
            }
        }
    }else {
        if(((*now).min + min) > MIN_MAX) {
            if(((*now).hour + hour) >= HOUR_MAX) {
                next_date(now, next);
                (*next).sec = (*now).sec + sec;
                (*next).min = ((*now).min + min) - 60;
                (*next).hour = ((*now).hour + hour) - 24 + 1;
            }else {
                (*next).sec = (*now).sec + sec;
                (*next).min = ((*now).min + min) - 60;
                (*next).hour = (*now).hour + hour + 1;
            }
        }else {
            if(((*now).hour + hour) > HOUR_MAX) {
                next_date(now, next);
                (*next).sec = ((*now).sec + sec);
                (*next). min = ((*now).min + min);
                (*next). hour = ((*now).hour + hour) - 24;
            }else {
                (*next).sec = ((*now).sec + sec);
                (*next).min = ((*now).min + min);
                (*next).hour = (*now).hour + hour;
            }
        }
    } 
}

/* 13. CAUTION: time_f will become the same as time_l   */
int time_diff(struct Time *time_f_ptr, struct Time *time_l_ptr) {
    int sec = 0;
    while (((*time_f_ptr).year != (*time_l_ptr).year) || ((*time_f_ptr).month != (*time_l_ptr).month) || 
            ((*time_f_ptr).day != (*time_l_ptr).day) || ((*time_f_ptr).hour != (*time_l_ptr).hour) || 
           ((*time_f_ptr).min != (*time_l_ptr).min) || ((*time_f_ptr).sec != (*time_l_ptr).sec)) {
            next_sample_time(time_f_ptr, 1, time_f_ptr);
            sec++;
    }
return sec;
}

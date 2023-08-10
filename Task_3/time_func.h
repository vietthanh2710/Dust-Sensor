struct Time {
    int year,month,day;
    int hour,min,sec;
};
/*1*/int element_check (struct Time* now, struct Time* first, struct Time* last);
/*2*/void str2time (char str[100], struct Time *time_ptr);
/*3*/void h2d (int interval_time, int* day_result, int* hour_result);
/*4*/void s2h (int spl_time, int* hour_ptr, int* min_ptr, int* sec_ptr);
/*5*/int day_max (int year, int month);
/*6*/int previous_date_check (struct Time *now_ptr);
/*7*/void previous_date (struct Time *now_ptr,struct Time *pre_ptr);
/*8*/int next_date_check (struct Time *x);
/*9*/void next_date (struct Time *now_ptr,struct Time *next_ptr);
/*10*/int sample_quantity (int itv, int spl);
/*11*/void time_start_func (struct Time *time_end, int interval_time, struct Time *time_start);
/*12*/void next_sample_time (struct Time *now, int sampling_time, struct Time *next);
/*13*/int time_diff (struct Time *time_f_ptr, struct Time *time_l_ptr);
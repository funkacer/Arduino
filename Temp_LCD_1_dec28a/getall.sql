select
    case when datum IS NULL then "0T" else datum end as datum,
    case when datum IS NULL then "Now" WHEN DAYOFWEEK(datum) = 1 THEN "NE" WHEN DAYOFWEEK(datum) = 2 THEN "PO"
    WHEN DAYOFWEEK(datum) = 3 THEN "UT" WHEN DAYOFWEEK(datum) = 4 THEN "ST" WHEN DAYOFWEEK(datum) = 5 THEN "CT"
    WHEN DAYOFWEEK(datum) = 6 THEN "PA" WHEN DAYOFWEEK(datum) = 7 THEN "SO" else "?" end as den,
	temp_min, temp_max, w_main /*, f_dt_txt_min, f_dt_txt_max*/ from
(SELECT SUBSTRING(`f_dt_txt`, 1, 10) as datum , MIN(`s_main_temp`) temp_min, MAX(`s_main_temp`) temp_max,
    CASE WHEN min(`s_weather_id`) < 600 THEN "Rain" WHEN min(`s_weather_id`) >= 600 and min(`s_weather_id`) < 700 THEN "Snow" 
    WHEN max(`s_weather_id`) >= 800 and max(`s_weather_id`) <= 803 THEN "Clear" ELSE "Clouds" END w_main,
    min(`f_dt_txt`) as f_dt_txt_min, max(`f_dt_txt`) as f_dt_txt_max FROM `weather` GROUP BY SUBSTRING(`f_dt_txt`, 1, 10)) t1
WHERE `f_dt_txt_max` IS NULL OR SUBSTRING(`f_dt_txt_max`, 12, 13) > "12"
ORDER BY datum;

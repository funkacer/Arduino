SELECT MIN(`s_main_temp`) temp_min, MAX(`s_main_temp`) temp_max,
            CASE
            WHEN min(`s_weather_id`) < 600 THEN "Rain"
            WHEN min(`s_weather_id`) >= 600 and min(`s_weather_id`) < 700 THEN "Snow"
            WHEN max(`s_weather_id`) >= 800 and max(`s_weather_id`) <= 803 THEN "Clear"
            ELSE "Clouds"
            END w_main
            FROM `weather` WHERE s_dt <= (SELECT s_dt FROM `weather` WHERE id = 1) + 86400;

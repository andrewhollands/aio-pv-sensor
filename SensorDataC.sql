/*
	
	SensorDataC.sql

 	The contents of this module are to be copied and pasted into a new database's 'SQL' 
 	tab in phpMyAdmin to create a new SQL table 'SensorDataC'. Its purpose is to structure
 	a database.

 	Created by UCF Senior Design Spring 2022 - Summer 2022 Group 6
 	Released on June 30th, 2022

*/

CREATE TABLE SensorDataC (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    sensor VARCHAR(30) NOT NULL,
    location VARCHAR(30) NOT NULL,
    value1 VARCHAR(10),
    value2 VARCHAR(10),
    value3 VARCHAR(10),
    value4 VARCHAR(10),
    value5 VARCHAR(10),
    reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
)

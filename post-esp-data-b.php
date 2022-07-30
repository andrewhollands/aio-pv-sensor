<?php
/*
	
	post-esp-data-b.php

 	This module is to be inserted into the /var/www/html directory stored on the Raspberry
 	Pi. Its purpose is to receive data from the AIO PV Sensor at Panel B and store it in
 	its respective database.

 	Created by UCF Senior Design Spring 2022 - Summer 2022 Group 6
 	Released on June 30th, 2022

*/

$servername = "localhost";

// REPLACE with your Database name
$dbname = "esp_data_b";
// REPLACE with Database user
$username = "root";
// REPLACE with Database user password
$password = "sdouc";

$api_key_value = "uc7LkiKar7";

$api_key= $sensor = $location = $value1 = $value2 = $value3 = $value4 = $value5 = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $sensor = test_input($_POST["sensor"]);
        $location = test_input($_POST["location"]);
        $value1 = test_input($_POST["value1"]);
        $value2 = test_input($_POST["value2"]);
        $value3 = test_input($_POST["value3"]);
	    $value4 = test_input($_POST["value4"]);
	    $value5 = test_input($_POST["value5"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO SensorDataB (sensor, location, value1, value2, value3, value4, value5)
        VALUES ('" . $sensor . "', '" . $location . "', '" . $value1 . "', '" . $value2 . "', '" . $value3 . "', '" . $value4 . "', '" . $value5 . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>

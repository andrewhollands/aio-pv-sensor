<!DOCTYPE html>
<!--
	
	esp-data-b.php

 	This module is to be inserted into the /var/www/html directory stored on the Raspberry
 	Pi. Its purpose is to query data from Panel B's database and display it on an HTML
 	webpage.

 	Created by UCF Senior Design Spring 2022 - Summer 2022 Group 6
 	Released on June 30th, 2022

-->

<head>
    <title>
        Reloading page after 1 second
    </title>
      
    <script>
        function autoRefresh() {
            window.location = window.location.href;
        }
        setInterval('autoRefresh()', 400);
    </script>
</head>
<?php

$servername = "localhost";

// REPLACE with your Database name
$dbname = "esp_data_b";
// REPLACE with Database user
$username = "root";
// REPLACE with Database user password
$password = "sdouc";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, sensor, location, value1, value2, value3, value4, value5, reading_time FROM SensorDataB ORDER BY id DESC";

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>Sensor</td> 
        <td>Location</td> 
        <td>Temperature (°C)</td> 
        <td>Temperature (°F)</td> 
        <td>Irradiance (W/m^2)</td> 
	    <td>Voltage (V)</td>
	    <td>Current (A)</td> 
        <td>Timestamp</td> 
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_sensor = $row["sensor"];
        $row_location = $row["location"];
        $row_value1 = $row["value1"];
        $row_value2 = $row["value2"]; 
        $row_value3 = $row["value3"]; 
	    $row_value4 = $row["value4"];
	    $row_value5 = $row["value5"]; 
        $row_reading_time = $row["reading_time"];
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_sensor . '</td> 
                <td>' . $row_location . '</td> 
                <td>' . $row_value1 . '</td> 
                <td>' . $row_value2 . '</td> 
                <td>' . $row_value3 . '</td> 
		        <td>' . $row_value4 . '</td>
		        <td>' . $row_value5 . '</td> 
                <td>' . $row_reading_time . '</td> 
              </tr>';
    }
    $result->free();
}
$conn->close();
?> 
</table>
</body>
</html>

# All-In-One Photovoltaic Sensor
## Introduction
This GitHub repository is purposed with housing the instructions and code to set up the primary and secondary networking nodes for our All-In-One (AIO) Photovoltaic (PV) Sensor.
This design is created for the Orlando Utilities Commission (OUC) to use for two main purposes: (1) to identify and troubleshoot faulty solar panels and (2) to collect, transmit, and organize multiple datapoints, including temperature, irradiance, voltage, and current, from a solar panel into a database.
There are multiple software components that will be discussed in this document. These components include the following: (1) the operating system, Raspberry Pi OS, which will run on our main network node, (2) the web server, Apache2, which will host our database, (3) the database, MySQL, which will structurally store our collected data and (4) the data collection device, ESP32, that interfaces with various sensors and then transmits collected data to the Raspberry Pi.
Additionally, there are multiple hardware components involved in this design which will not be discussed in large detail in this document but will be discussed in other supporting documents.
## [Raspberry Pi OS](https://www.raspberrypi.com)
Downloading and installing the Raspberry Pi OS has been simplified in the recent years by the Raspberry Pi foundation.
The Raspberry Pi Imager is the easiest way to install Raspberry Pi OS without sacrificing customization to a microSD card or USB device, ready to use with the Raspberry Pi.
Download and install Raspberry Pi Imager via https://www.raspberrypi.com/software/ to a computer with an SD card reader. Put the SD card you'll use with your Raspberry Pi into the reader and run Raspberry Pi Imager.
For our design, we utilized the advanced settings menu during installation to initialize the Raspberry Pi OS with our Wi-Fi credentials and SSH enabled for easy remote access. For further, extraneous information on Raspberry Pi OS setup, refer to https://raspberrypi-guide.github.io/getting-started/install-operating-system.
## [Apache2 Web Server](https://httpd.apache.org)
At this point, the Raspberry Pi OS has been installed on the Raspberry Pi and has been connected to the Internet via Wi-Fi.
Apache2 is the web server we will use to host our database and necessary webpages.
To install Apache2 on the Raspberry Pi, run the following command:
```
sudo apt install apache2 -y
```
By default, Apache2 will store its directory at `/var/www/html/` and we will keep this as is for our purposes.
## [PHP: Hypertext Preprocessor](https://www.php.net)
PHP is the scripting language we will use write our server's webpages, in conjunction with HTML when necessary.
To install PHP on the Raspberry Pi's Apache2 web server, first navigate to Apache2's directory, `/var/www/html/` by running
```
cd /var/www/html/
```
and then run the following command:
```
sudo apt install php -y
```
We will remain in the `/var/www/html/` directory unless stated otherwise.
Restarting the Apache2 web server may be necessary after making changes to its directory. To restart Apache2, run the following command:
```
sudo service apache2 restart
```
## [MySQL Database](https://www.mysql.com)
MySQL is the relational database platform we will use to store our collected data.
To install MySQL on the Raspberry Pi's Apache2 web server, run the following command:
```
sudo apt install mariadb-server php-mysql -y
```
and then restart the Apache2 server.
Finally, secure the MySQL by running the following command:
```
sudo mysql_secure_installation
```
Our MySQL database code can be found [here](https://github.com/andrewhollands/aio-pv-sensor/blob/main/SensorDataA.sql).
## [phpMyAdmin: MySQL Administrator](https://www.phpmyadmin.net)
phpMyAdmin is an application prebuilt to administer the MySQL database via web interface.
To install phpMyAdmin on the Raspberry Pi's Apache2 web server, run the following command:
```
sudo apt install phpmyadmin -y
```
Next, enable the PHP MySQLi extenstion by running the following command:
```
sudo phpenmod mysqli
```
and then restart the Apache2 server.
Finally, move the phpmyadmin folder to `/var/www/html` as it is likely incorrectly placed by running the following command:
```
sudo ln -s /usr/share/phpmyadmin
```
## [ESP32 Microcontroller Unit](https://www.espressif.com/en/products/socs/esp32)
ESP32 is a multipurpose microcontroller unit (MCU) that meets the needs of our AIO PV Sensor design and more.
Our design will implement two (2) ESP32 devices: one (the "sensing node") will be used to collect and wirelessly transmit hardwired data from a thermocouple, pyranometer, voltage sensor and current sensor, and the other will be used as a Wi-Fi access point (the "access point") to allow communication between the other ESP32 device and the Raspberry Pi device.
The sensing node will house a [program written in Arduino/C++](https://github.com/andrewhollands/aio-pv-sensor/blob/main/esp-poster-with-averaging-a.ino) that will convert analog values to digital values where necessary and then organize collected values into an [HTTP post](https://github.com/andrewhollands/aio-pv-sensor/blob/main/post-esp-data-a.php), which is thien transmitted to the Raspberry Pi's web server via an HTTP post request. The ESP32 is able to locate the Raspberry Pi's web server only after intelligently finding the IP address via a snippet of code in the [program written in Arduino/C++](https://github.com/andrewhollands/aio-pv-sensor/blob/main/esp-poster-with-averaging-a.ino) that will increment the IP address's last digit until a valid HTTP post response code is provided. Once the Raspberry Pi's web server has been located, the [program written in Arduino/C++](https://github.com/andrewhollands/aio-pv-sensor/blob/main/esp-poster-with-averaging-a.ino) will be interfaced with the [HTML/PHP webpage](https://github.com/andrewhollands/aio-pv-sensor/blob/main/esp-data-a.php), which is structured by this [MySQL script](https://github.com/andrewhollands/aio-pv-sensor/blob/main/SensorDataA.sql), housed on the Raspberry Pi's Apache2 web server.
The access point will house a [program written in C](https://github.com/andrewhollands/aio-pv-sensor/blob/main/esp-access-point.ino) that enables the ESP32 device to act as a Wi-Fi access point for multiple wireless device connections.
## Wrap-Up
The Apache2 web server can be accessed by navigating to the Raspberry Pi's IP address in a web browser. Until the web server is ported to a domain found on the Internet (e.g. github.com), accessing the Raspberry Pi's web server can be done by navigating to its IP address of `192.168.4.X` via HTTP where `X` is an integer between 2 and 255, inclusive.
As an example, we can access the Raspberry Pi's Apache2 web server by navigating to `http://192.168.4.2` via Safari on an iPhone connected to the common the ESP32 access point. The user will be directed to `192.168.4.2/index.html` which contains buttons to reach Panel A data, Panel B data, Panel C data, and phpMyAdmin, where each button references `esp-data-a.php`, `esp-data-b.php`, `esp-data-c.php`, and the `phpmyadmin` directory, respectively. All mentioned files and locations are stored in `var/www/html`. Additionally, within phpMyAdmin is where the user can add and remove databases as well as export, modify, and import databases by signing in with the the following credentials: `user: root, password: sdouc`.
## Bug Reports and Features Requests
If you encounter a bug or other unexpected/undesirable behavior or would like to submit a request for a new feature to be implmented for the provided software, please submit the appropriate form [here](https://github.com/andrewhollands/aio-pv-sensor/issues/new/choose).

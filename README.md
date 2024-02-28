
# Arduino Water Depth Sensor

Design, instructions, code, and information for a low-cost, Arduino-based, real-time water depth sensor using a differential pressure sensor and thermistor.


## Authors

- [@rhpearce](https://github.com/rhpearce)
- [@KCLGeography](https://github.com/KCLGeography)
- [@DrKrisChan](https://github.com/DrKrisChan)



## Instructions
A brief overview of the instructions is given below, but more detailed instructions can be found [here] (https://drive.google.com/file/d/1HNqiERP32G9E8vRf_0rl5kWMbjWXEGlC/view?usp=sharing). As of **28/2/24** wiring has been updated, use Fritzing diagram below for most recent wiring. Follow instruction manual for logger housing construction and calibration help. 

- Ensure you have read and understand the disclaimer and license associated with this guide.
- Acquire the necessary parts listed in the [Parts List](https://docs.google.com/spreadsheets/d/1xHk34M-YiePO2RZdtJb8Tk1eWIesMuvv/edit?usp=sharing&ouid=106105173113638662095&rtpof=true&sd=true). The PCB can be obtained from [@KCLGeography](https://github.com/KCLGeography) [EnvPCBdesigns](https://github.com/KCLGeography/EnvPCBdesigns).
- Assembly can be followed with the Fritzing schematic below.
- Once the basic logger is assembled and the sensors are connected, the final code can be found [here](https://github.com/rhpearce/waterdepthsensor/blob/main/water-depth.ino).



![App Screenshot](https://github.com/rhpearce/waterdepthsensor/blob/main/water-depth-schematic.png)


## Disclaimer
The material in this repository is intended as documentation of the process by which the King's College London environmental monitoring team build our open-source loggers. Though we take care to ensure that the pages are accurate as of the date of publication, Arduino software, libraries, electronic components and interface devices are all subject to variation, change with time, and all introduce the potential for risk. The authors take no responsibility for the consequences of error or for any loss, damage or injury suffered by users or their property as a result of any of the information published on any of these pages, and such information does not form any basis of a contract with readers or users of it. The audience should verify any information provided and only proceed if they have an adequate understanding of electronics and electronics safety.

## Testing

Arduino based total water depth logger has been tested on 6-month deployment against a Solinst Level Logger Junior 5 to assess accuracy and observe any technical difficulties. More information is found in the citation information, but data from the testing can be accessed [here](https://docs.google.com/spreadsheets/d/1b9mkM1EqoMOuJWOz1teIzVWMdMKTzaIR/edit?usp=sharing&ouid=106105173113638662095&rtpof=true&sd=true).

## License

This work is licensed under [Creative Commons Attribution 4.0 International License](https://creativecommons.org/licenses/by/4.0/) - any use of any material here requires attribution.

Citation Information: to be updated.





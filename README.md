# CO2 and eCO2 sensor comparison

Small system to compare the Sensirion [SCD30](https://www.sensirion.com/en/environmental-sensors/carbon-dioxide-sensors/carbon-dioxide-sensors-co2/) (≈40€. [Mouser](https://www.mouser.es/ProductDetail/Sensirion/SCD30/?qs=rrS6PyfT74fdywu4FxpYjQ%3D%3D)) CO2 sensor, the Renesas (IDT) [ZMOD4410](https://www.renesas.com/us/en/products/sensor-products/gas-sensors/zmod4410-indoor-air-quality-sensor-platform) (≈7€. [Mouser](https://www.mouser.es/ProductDetail/Renesas-IDT/ZMOD4410AI3V/?qs=hd1VzrDQEGhLEbSZESmJlw%3D%3D)) TVOC/eCO2/IAQ sensor and the ScioSense [CCS811](https://www.sciosense.com/products/environmental-sensors/ccs811-gas-sensor-solution/) (≈7€. [Mouser](https://www.mouser.es/ProductDetail/ScioSense/CCS811B-JOPR?qs=DPoM0jnrROVuOwd2mTsTQg%3D%3D)) TVOC/eCO2 one.

## Results

All the data (2 days for this test) is stored online using InfluxDB.

Blue -> CCS811; Purple -> SCD30; Orange -> ZMOD4410

![Graph view 2days](img/influx_board_screenshot_2.png)
 [Raw data](export/2021-03-22-12-12_chronograf_data.csv)

Notes:
 * ZMOD4410 using the 2nd GEN algorithm with a fixed period of 2s.
 * CCS811 with a fixed period of 10s.
 * CCS811 NOT using temp&rh compensation.

There are another branch with other tests.

## Test board

![Board top view](img/board_top_new.jpg)
![Board bottom view  (old, not showing CCS811 sensor)](img/board_bottom.jpg)

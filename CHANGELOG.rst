^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ros2_iio_medical
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.1.0 (2026-07-01)
-------------------
* Initial release
* ``iio_bridge`` node: sysfs polling bridge for prototyping and low-rate acquisition
* ``iio_triggered_bridge`` node: epoll-based triggered buffer bridge for production use
* Automatic channel discovery via ``scan_elements`` sysfs (type, index, scale)
* Binary sample parsing: signed/unsigned, little/big endian, shift, real bits
* Supports ADS1299 (24-bit 8-channel EEG/ECG), MAX86150 (ECG+PPG),
  ti-ads1298 (ECG), MAX30102 (SpO2/heart-rate) via standard Linux IIO sysfs
* Launch files for both nodes with per-device parameter sets
* Simulation test script for sysfs-based testing without hardware
* Contributors: Md Shofiqul Islam

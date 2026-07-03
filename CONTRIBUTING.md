# Contributing to ros2_iio_medical

Thank you for your interest in contributing. This package bridges Linux IIO
biosignal kernel drivers to ROS 2 — contributions that improve hardware
support, robustness, or documentation are welcome.

## Good first issues

If you are new to the project, start here:

| Area | Task | Skill needed |
|---|---|---|
| **Testing** | Run `iio_bridge` against a real MAX30102 and report results | ROS 2, I2C hardware |
| **Testing** | Run `iio_triggered_bridge` against a real ADS1299 or ti-ads1298 | ROS 2, SPI hardware |
| **Docs** | Add wiring diagram for MAX30102 → Raspberry Pi / Orange Pi | Markdown |
| **Docs** | Add wiring diagram for ADS1299 → Orange Pi Zero 2W | Markdown |
| **Feature** | Add `sensor_msgs/msg/Imu` output option alongside Float64MultiArray | C++, ROS 2 |
| **Feature** | Add `/biosignal/ecg` topic with proper `std_msgs/Header` timestamps | C++, ROS 2 |
| **CI** | Add colcon test with ament_cmake_gtest unit test for `parse_channel_type()` | C++, CMake |

Open an issue before starting so we can discuss the approach.

## Hardware we need tested

If you have any of these devices, a test report (or PR with a launch file)
is very valuable:

- TI ADS1299 EEG ADC (SPI)
- ADI MAX86150 ECG+PPG (I2C)
- ADI MAX30102 SpO₂ (I2C)
- TI ti-ads1298 ECG (SPI)
- Any other Linux IIO device with scan_elements support

## How to contribute

1. Fork the repo
2. Create a branch: `git checkout -b feature/your-change`
3. Make your change, build with `colcon build`
4. Push and open a Pull Request

## Build

```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-select ros2_iio_medical
source install/setup.bash
```

## Test without hardware

```bash
bash src/ros2_iio_medical/test/simulate_ads1299.sh &
ros2 run ros2_iio_medical iio_bridge \
  --ros-args -p device_path:=/tmp/iio_sim/iio:device0 \
             -p num_channels:=8 \
             -p topic_name:=/biosignal/eeg
ros2 topic echo /biosignal/eeg
```

## Code style

- C++17, `ament_cmake` build system
- Follow existing code style (no trailing whitespace, 2-space indent)
- Apache-2.0 copyright header on all new files

## Related projects

- [kernel-medical/iec62304-embedded-drivers](https://github.com/kernel-medical/iec62304-embedded-drivers) — IEC 62304 compliance docs for the upstream IIO drivers this package reads from
- [kernel-soup-gen](https://github.com/kernel-medical/kernel-soup-gen) — auto-generate SOUP records for Linux kernel drivers

## License

Apache-2.0 — all contributions must be compatible.

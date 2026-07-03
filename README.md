# ros2_iio_medical

ROS2 bridge between Linux IIO biosignal devices and ROS2 topics.

Reads from Linux IIO kernel drivers (ADS1299, MAX86150, ti-ads1298, MAX30102)
and publishes biosignal data as ROS2 topics for use in medical robotics and
surgical monitoring applications.

## Supported Devices

| Chip | Type | Topic |
|---|---|---|
| TI ADS1299 | 24-bit 8-channel EEG/ECG ADC | `/biosignal/eeg` |
| ADI MAX86150 | ECG + PPG analog front-end | `/biosignal/ecg_ppg` |
| TI ti-ads1298 | ECG ADC | `/biosignal/ecg` |
| ADI MAX30102 | SpO₂ / heart-rate | `/biosignal/spo2` |

## Two Acquisition Modes

### 1. Polling Bridge (`iio_bridge`)
Reads from sysfs at a fixed timer rate. Simple, good for prototyping.

### 2. Triggered Buffer Bridge (`iio_triggered_bridge`)
Uses Linux IIO kernel buffer + `epoll`. Hardware trigger (DRDY interrupt)
drives acquisition — no timer jitter, no missed samples. Correct approach
for continuous biosignal acquisition.

```
Hardware DRDY interrupt → IIO trigger → kernel DMA → /dev/iio:deviceN
epoll_wait() wakes thread → read() → parse binary → ROS2 topic
```

## Build

```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-select ros2_iio_medical
source install/setup.bash
```

## Run

```bash
# Triggered buffer (production)
ros2 launch ros2_iio_medical iio_triggered_bridge.launch.py

# Polling (prototyping)
ros2 launch ros2_iio_medical iio_bridge.launch.py

# Monitor output
ros2 topic echo /biosignal/eeg
```

## Parameters

| Parameter | Default | Description |
|---|---|---|
| `sysfs_path` | `/sys/bus/iio/devices/iio:device0` | IIO device sysfs path |
| `dev_path` | `/dev/iio:device0` | IIO character device |
| `num_channels` | `8` | Number of channels to acquire |
| `buffer_length` | `64` | Kernel buffer depth in samples |
| `topic_name` | `/biosignal/eeg` | ROS2 output topic |

## Related Kernel Drivers

The Linux IIO drivers this bridge reads from:

- [ADS1299 driver](https://lore.kernel.org/linux-iio/) — under review
- [MAX86150 driver](https://lore.kernel.org/linux-iio/) — under review  
- [ti-ads1298 fixes](https://lore.kernel.org/linux-iio/) — merged to mainline

## License

Apache-2.0

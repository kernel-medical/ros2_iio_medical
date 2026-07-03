from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([

        # ADS1299 — 8-channel 24-bit EEG/ECG ADC via triggered buffer
        # Hardware DRDY interrupt drives acquisition — no timer jitter
        Node(
            package='ros2_iio_medical',
            executable='iio_triggered_bridge',
            name='ads1299_triggered',
            parameters=[{
                'sysfs_path':     '/sys/bus/iio/devices/iio:device0',
                'dev_path':       '/dev/iio:device0',
                'num_channels':   8,
                'buffer_length':  64,
                'topic_name':     '/biosignal/eeg',
            }],
            output='screen',
        ),

        # MAX86150 — ECG + PPG via triggered buffer
        Node(
            package='ros2_iio_medical',
            executable='iio_triggered_bridge',
            name='max86150_triggered',
            parameters=[{
                'sysfs_path':     '/sys/bus/iio/devices/iio:device1',
                'dev_path':       '/dev/iio:device1',
                'num_channels':   2,
                'buffer_length':  32,
                'topic_name':     '/biosignal/ecg_ppg',
            }],
            output='screen',
        ),

    ])

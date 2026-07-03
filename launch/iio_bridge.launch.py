from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([

        # ADS1299 — 8-channel 24-bit EEG/ECG ADC
        Node(
            package='ros2_iio_medical',
            executable='iio_bridge',
            name='ads1299_bridge',
            parameters=[{
                'device_path':    '/sys/bus/iio/devices/iio:device0',
                'num_channels':   8,
                'sample_rate_hz': 250.0,
                'topic_name':     '/biosignal/eeg',
            }],
            output='screen',
        ),

        # MAX86150 — ECG + PPG analog front-end (2 channels)
        Node(
            package='ros2_iio_medical',
            executable='iio_bridge',
            name='max86150_bridge',
            parameters=[{
                'device_path':    '/sys/bus/iio/devices/iio:device1',
                'num_channels':   2,
                'sample_rate_hz': 100.0,
                'topic_name':     '/biosignal/ecg_ppg',
            }],
            output='screen',
        ),

    ])

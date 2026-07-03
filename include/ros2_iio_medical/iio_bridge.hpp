#ifndef ROS2_IIO_MEDICAL__IIO_BRIDGE_HPP_
#define ROS2_IIO_MEDICAL__IIO_BRIDGE_HPP_

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <std_msgs/msg/header.hpp>

#include <string>
#include <vector>
#include <fstream>

namespace ros2_iio_medical
{

class IIOBridgeNode : public rclcpp::Node
{
public:
  explicit IIOBridgeNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

private:
  void timer_callback();
  void read_scale();
  bool read_channel(int channel, double & value);

  std::string device_path_;
  std::string topic_name_;
  int num_channels_;
  double sample_rate_hz_;
  double scale_;

  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace ros2_iio_medical

#endif  // ROS2_IIO_MEDICAL__IIO_BRIDGE_HPP_

#include <gazebo/common/Plugin.hh>
#include <ros/ros.h>
#include "turtlebot_gazebo_distance_sensor/DistanceSensorPlugin.h"

#include "gazebo_plugins/gazebo_ros_camera.h"

#include <string>
#include <vector>

#include <gazebo/sensors/Sensor.hh>
#include <gazebo/sensors/CameraSensor.hh>
#include <gazebo/sensors/SensorTypes.hh>

#include <sensor_msgs/Illuminance.h>

using std::vector;

namespace gazebo
{
	// Register this plugin with the simulator
	GZ_REGISTER_SENSOR_PLUGIN(GazeboRosDistance)

	////////////////////////////////////////////////////////////////////////////////
	// Constructor
	GazeboRosDistance::GazeboRosDistance()
	{
	}

	////////////////////////////////////////////////////////////////////////////////
	// Destructor
	GazeboRosDistance::~GazeboRosDistance()
	{
		ROS_DEBUG_STREAM_NAMED("camera", "Unloaded");
	}

	void GazeboRosDistance::Load(sensors::SensorPtr _sensor, sdf::ElementPtr _sdf)
	{
		// Make sure the ROS node for Gazebo has already been initialized
		if (!ros::isInitialized())
		{
			ROS_FATAL_STREAM(
					"A ROS node for Gazebo has not been initialized, unable to load plugin. " << "Load the Gazebo system plugin 'libgazebo_ros_api_plugin.so' in the gazebo_ros package)");
			return;
		}

		// Get the parent sensor.
		this->parentSensor = std::dynamic_pointer_cast<sensors::LogicalCameraSensor>(_sensor);

		// Make sure the parent sensor is valid.
		if (!this->parentSensor)
		{
			gzerr << "GazeboRosDistance requires a LogicalCamera Sensor.\n";
			return;
		}

		// Connect to the sensor update event.
		this->updateConnection = this->parentSensor->ConnectUpdated(std::bind(&GazeboRosDistance::OnUpdate, this));

		// Make sure the parent sensor is active.
		this->parentSensor->SetActive(true);

	}

	void GazeboRosDistance::OnUpdate()
	{
		vector<msgs::LogicalCameraImage_Model> victimModels;

		// Get all the models in range.
		auto models = this->parentSensor->Image();
		for (int i = 0; i < models.model_size(); i++)
		{
			auto m = models.model(i);
			auto n  = m.name();

			auto x = m.pose().position().x();
			auto y = m.pose().position().y();
			auto z = m.pose().position().z();
			auto dist = sqrt(x*x + y*y + z*z);

			// Debugging output
//			std::cout << this->parentSensor->Far() << " " << this->parentSensor->Near() << " " << this->parentSensor->HorizontalFOV() << std::endl;
			printf("Senosr No. %x, Model No. %d with Name %s at (%.1f, %.1f, %.1f) dist: %.2f\n", this, i, n.c_str(), x, y, z, dist);

			if (n.find("victim") != std::string::npos && dist <= 4) {
				victimModels.push_back(m);
			}

		}

		for (auto m : victimModels) {
			auto n  = m.name();
			auto x = m.pose().position().x();
			auto y = m.pose().position().y();
			auto z = m.pose().position().z();

			printf("Victim found with Name %s at (%f, %f, %f)\n", n.c_str(), x, y, z);
		}

	}
}

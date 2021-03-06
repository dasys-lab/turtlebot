#ifndef GAZEBO_ROS_DISTANCE_SENSOR_HH
#define GAZEBO_ROS_DISTANCE_SENSOR_HH

#include <string>

// library for processing camera data for gazebo / ros conversions

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/sensors/Sensor.hh>
#include <SystemConfig.h>
#include <chrono>

#include <turtlebot_gazebo_distance_sensor/LogicalCameraSensor.hh>

//#define LOGICAL_CAMERA_DEBUG

using namespace std;

namespace gazebo
{
	class DistanceSensorPlugin : public SensorPlugin
	{
	public:
		/**
		 * Constructor
		 */
		DistanceSensorPlugin();

		/**
		 * Destructor.
		 */
		virtual ~DistanceSensorPlugin();

		/**
		 * Load the sensor plugin.
		 * @param _sensor Pointer to the sensor that loaded this plugin.
		 * @param _sdf SDF element that describes the plugin.
		 */

		virtual void Load(sensors::SensorPtr _sensor, sdf::ElementPtr _sdf);

	private:
		/**
		 * Callback that receives the contact sensor's update signal.
		 */
		virtual void OnUpdate();

		struct ConfigModel
		{
			double range;
			/**
			 * pair.first = startAngle
			 * pair.secong = endAngle
			 */
			double publishingRate;
			vector<pair<double, double>> detectAngles;
			string name;
			string type;
		};

		struct ModelProperties
		{
			// These are determined using gazebos bounding box
			double xlength;
			double ylength;
			double zlength;
		};

		/**
		 * Connection that maintains a link between the contact sensor's
		 * updated signal and the OnUpdate callback.
		 */

		event::ConnectionPtr updateConnection;
		// ros stuff
		ros::NodeHandle nh;
		ros::Publisher modelPub;
		// Sensor orientation
		double sensorYaw;
		supplementary::SystemConfig* sc;
		// Sensor ptr
		sensors::LogicalCameraSensorPtr parentSensor;
		// Name of Robot
		string robotName;
		// Section in config file
		shared_ptr<vector<string>> modelSectionNames;
		// Maps model Name to config model
		map<string, ConfigModel> modelMap;
		physics::WorldPtr world;

		// time points of messages sent, need to determine when to send
		// next message according specified configuration frequency
		map<string, chrono::time_point<chrono::high_resolution_clock>> lastPublishedMap;

		/**
		 * load model parameters from config file
		 */
		void loadModelsFromConfig();
		/**
		 * @param model model detected by sensor
		 * @param configModel config lodaded by systemconf
		 * @param props Additional properties to be published
		 */
		void publishModel(msgs::LogicalCameraImage_Model model,
				DistanceSensorPlugin::ConfigModel& configModel,
				DistanceSensorPlugin::ModelProperties& props);
		/**
		 * @param angle model detected by sensor
		 * @param detectAngles config lodaded by systemconf
		 */
		bool isInAngleRange(double angle, vector<pair<double, double>> detectAngles);
		/**
		 * calculates angle of object from quaternium
		 */
		double quadToTheata(double x, double y, double z, double w);
		/**
		 * @param model model detected by sensor
		 * @param configModel config lodaded by systemconf
		 */
		bool isDetected(msgs::LogicalCameraImage_Model model, DistanceSensorPlugin::ConfigModel& configModel);

		/**
		 * checks if sensor is responsible for the found model
		 */
		bool isSensorResponsible(msgs::LogicalCameraImage_Model model);

		/**
		 * calculates to angle from the robot to a model
		 */
		double calculateAngle(double x, double y);
	};
}
#endif

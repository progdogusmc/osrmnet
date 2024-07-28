#include "LevrumOsrmRoutePosition.h"

#include "stdafx.h"

#include "osrm/match_parameters.hpp"
#include "osrm/nearest_parameters.hpp"
#include "osrm/route_parameters.hpp"
#include "osrm/table_parameters.hpp"
#include "osrm/trip_parameters.hpp"

#include "osrm/coordinate.hpp"
#include "osrm/engine_config.hpp"
#include "osrm/json_container.hpp"

#include "util/json_renderer.hpp"

#include "osrm/osrm.hpp"
#include "osrm/status.hpp"

#include "LevrumRouter.h"

#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include <cstdlib>

using namespace osrm;

#define METERFRACTIONOFMILE 0.000621371;

namespace osrmnet {

	const osrm::OSRM* m_osrm;

	double m_resolveMeters = 152.4; // 500 feet

	bool m_checkLocations = true;

	LevrumRouter::LevrumRouter(const std::string& path) {
		EngineConfig config;

		config.storage_config = { path.c_str() };
		config.use_shared_memory = false;

		m_osrm = new OSRM{ config };
	}

	LevrumRouter::~LevrumRouter() {
		delete m_osrm;
	}

	void LevrumRouter::SetResolveDistance(double meters)
	{
		m_resolveMeters = meters;
	}

	void LevrumRouter::SetLocationChecking(bool checkLocations)
	{
		m_checkLocations = checkLocations;
	}

	double distanceInMeters(double lon1, double lat1, double lon2, double lat2)
	{
		double radius = 6378.137; // Radius of earth in KM
		double dLat = lat2 * 3.14159265358979323846 / 180 - lat1 * 3.14159265358979323846 / 180;
		double dLon = lon2 * 3.14159265358979323846 / 180 - lon1 * 3.14159265358979323846 / 180;
		double a = sin(dLat / 2) * sin(dLat / 2) +
			cos(lat1 * 3.14159265358979323846 / 180) * cos(lat2 * 3.14159265358979323846 / 180) *
			sin(dLon / 2) * sin(dLon / 2);
		double c = 2 * atan2(sqrt(a), sqrt(1 - a));
		double d = radius * c;
		return d * 1000; // meters
	}

	bool resolveNearest(double lon, double lat)
	{
		NearestParameters nearest;
		nearest.coordinates.push_back({ util::FloatLongitude{ lon }, util::FloatLatitude{ lat } });
		json::Object result;

		const auto status1 = m_osrm->Nearest(nearest, result);
		if (status1 == Status::Ok)
		{
			auto& waypoints = result.values["waypoints"].get<json::Array>();
			auto& waypoint = waypoints.values.at(0).get<json::Object>();

			double distance = waypoint.values["distance"].get<json::Number>().value;

			return distance <= m_resolveMeters;
		}

		return false;
	}

	bool verifyResultLocations(double lon1, double lat1, double lon2, double lat2, json::Object result)
	{
		auto& waypoints = result.values["waypoints"].get<json::Array>();
		auto& waypoint1 = waypoints.values.at(0).get<json::Object>();
		auto& location1 = waypoint1.values["location"].get<json::Array>();

		double loc1lon = location1.values.at(0).get<json::Number>().value;
		double loc1lat = location1.values.at(1).get<json::Number>().value;

		auto& waypoint2 = waypoints.values.at(1).get<json::Object>();
		auto& location2 = waypoint2.values["location"].get<json::Array>();

		double loc2lon = location2.values.at(0).get<json::Number>().value;
		double loc2lat = location2.values.at(1).get<json::Number>().value;

		return (distanceInMeters(lon1, lat1, loc1lon, loc1lat) < m_resolveMeters && distanceInMeters(lon2, lat2, loc2lon, loc2lat) < m_resolveMeters);
	}
	
	std::vector<char> LevrumRouter::GetRoute(double lon1, double lat1, double lon2, double lat2, bool getSteps) const
	{
		std::vector<char> jsonContents;

		RouteParameters params;

		params.coordinates.push_back({ util::FloatLongitude{ lon1 }, util::FloatLatitude{ lat1 } });
		params.coordinates.push_back({ util::FloatLongitude{ lon2 }, util::FloatLatitude{ lat2 } });
		params.steps = getSteps;

		// Response is in JSON format
		json::Object result;

		// Execute routing request, this does the heavy lifting
		const auto status = m_osrm->Route(params, result);

		if (status == Status::Ok)
		{
			if (m_checkLocations)
			{
				if (!verifyResultLocations(lon1, lat1, lon2, lat2, result))
				{
					return jsonContents;
				}
			}
			util::json::render(jsonContents, result);
		}

		return jsonContents;
	}

	std::vector<char> LevrumRouter::Nearest(double lon, double lat) const
	{
		std::vector<char> jsonContents;

		NearestParameters nearest;
		nearest.coordinates.push_back({ util::FloatLongitude{ lon }, util::FloatLatitude{ lat } });
		json::Object result;

		const auto status1 = m_osrm->Nearest(nearest, result);
		if (status1 == Status::Ok) {
			util::json::render(jsonContents, result);
		}

		return jsonContents;
	}

	double LevrumRouter::GetDistance(double lon1, double lat1, double lon2, double lat2) const
	{
		double distance;
		double duration;

		TryGetDistanceDuration(lon1, lat1, lon2, lat2, distance, duration);

		return distance;
	}

	bool LevrumRouter::TryGetDistanceDuration(double lon1, double lat1, double lon2, double lat2, double& distance, double& duration) const
	{
		RouteParameters params;
		params.coordinates.push_back({ util::FloatLongitude{ lon1 }, util::FloatLatitude{ lat1 } });
		params.coordinates.push_back({ util::FloatLongitude{ lon2 }, util::FloatLatitude{ lat2 } });

		params.alternatives = false;
		params.steps = false;
		params.overview = RouteParameters::OverviewType::False;

		json::Object result;

		const auto status = m_osrm->Route(params, result);
		if (status == Status::Ok)
		{
			auto& routes = result.values["routes"].get<json::Array>();
			auto& route = routes.values.at(0).get<json::Object>();

			if (m_checkLocations)
			{
				auto& waypoints = result.values["waypoints"].get<json::Array>();
				auto& waypoint1 = waypoints.values.at(0).get<json::Object>();
				auto& location1 = waypoint1.values["location"].get<json::Array>();

				double loc1lon = location1.values.at(0).get<json::Number>().value;
				double loc1lat = location1.values.at(1).get<json::Number>().value;

				auto& waypoint2 = waypoints.values.at(1).get<json::Object>();
				auto& location2 = waypoint2.values["location"].get<json::Array>();

				double loc2lon = location2.values.at(0).get<json::Number>().value;
				double loc2lat = location2.values.at(1).get<json::Number>().value;

				if (distanceInMeters(lon1, lat1, loc1lon, loc1lat) > m_resolveMeters || distanceInMeters(lon2, lat2, loc2lon, loc2lat) > m_resolveMeters)
					return false;
			}

			distance = route.values["distance"].get<json::Number>().value;
			duration = route.values["duration"].get<json::Number>().value;

			return true;
		}

		distance = -1.0;
		duration = -1.0;

		return false;
	}

	LevrumOsrmRoutePosition LevrumRouter::GetLocationAtTime(double lon1, double lat1, double lon2, double lat2, double elapsedMin, double speedMph) const
	{
		RouteParameters params;
		params.coordinates.push_back({ util::FloatLongitude{ lon1 }, util::FloatLatitude{ lat1 } });
		params.coordinates.push_back({ util::FloatLongitude{ lon2 }, util::FloatLatitude{ lat2 } });
		params.steps = true;

		json::Object result;

		const auto status = m_osrm->Route(params, result);
		if (status == Status::Ok)
		{
			if (m_checkLocations && !verifyResultLocations(lon1, lat1, lon2, lat2, result))
			{
				return LevrumOsrmRoutePosition(-1.0, -1.0, -1.0);
			}

			auto& routes = result.values["routes"].get<json::Array>();
			auto& route = routes.values.at(0).get<json::Object>();
			auto& legs = route.values["legs"].get<json::Array>();
			auto& leg = legs.values.at(0).get<json::Object>();
			auto& steps = leg.values["steps"].get<json::Array>();

			if (speedMph > 0.0)
			{
				double routeDistance = route.values["distance"].get<json::Number>().value;
				double milesTraveled = (elapsedMin / 60.0) * speedMph;
				double metersTraveled = milesTraveled / METERFRACTIONOFMILE;

				double meters = 0.0;

				json::Object& lastStep = steps.values.at(0).get<json::Object>();
				for (size_t i = 0; i < steps.values.size(); i++)
				{
					auto& step = steps.values.at(i).get<json::Object>();
					double distance = step.values["distance"].get<json::Number>().value;

					if (distance + meters >= metersTraveled) // Target location is along this step
					{
						double metersAlongStep = metersTraveled - meters;
						double percentageComplete = metersAlongStep / distance;

						auto& lastManeuver = lastStep.values["maneuver"].get<json::Object>();
						auto& lastLocation = lastManeuver.values["location"].get<json::Array>();

						double lastLongitude = lastLocation.values.at(0).get<json::Number>().value;
						double lastLatitude = lastLocation.values.at(1).get<json::Number>().value;

						auto& currentManeuver = step.values["maneuver"].get<json::Object>();
						auto& nextLocation = currentManeuver.values["location"].get<json::Array>();

						double nextLongitude = nextLocation.values.at(0).get<json::Number>().value;
						double nextLatitude = nextLocation.values.at(1).get<json::Number>().value;

						double deltaX = (nextLongitude - lastLongitude) * percentageComplete;
						double deltaY = (nextLatitude - lastLatitude) * percentageComplete;

						return LevrumOsrmRoutePosition(lastLongitude + deltaX, lastLatitude + deltaY, metersTraveled / routeDistance);
					}
					else
					{
						meters += distance;
					}
					lastStep = step;
				}
			}
			else
			{
				double routeDuration = route.values["duration"].get<json::Number>().value;
				double elapsedSeconds = elapsedMin * 60.0;

				double seconds = 0.0;

				json::Object& lastStep = steps.values.at(0).get<json::Object>();
				for (size_t i = 0; i < steps.values.size(); i++)
				{
					auto& step = steps.values.at(i).get<json::Object>();
					double duration = step.values["duration"].get<json::Number>().value;

					if (duration + seconds >= elapsedSeconds)
					{
						double secondsAlongStep = elapsedSeconds - seconds;
						double percentageComplete = secondsAlongStep / duration;

						auto& lastManeuver = lastStep.values["maneuver"].get<json::Object>();
						auto& lastLocation = lastManeuver.values["location"].get<json::Array>();

						double lastLongitude = lastLocation.values.at(0).get<json::Number>().value;
						double lastLatitude = lastLocation.values.at(1).get<json::Number>().value;

						auto& currentManeuver = step.values["maneuver"].get<json::Object>();
						auto& nextLocation = currentManeuver.values["location"].get<json::Array>();

						double nextLongitude = nextLocation.values.at(0).get<json::Number>().value;
						double nextLatitude = nextLocation.values.at(1).get<json::Number>().value;

						double deltaX = (nextLongitude - lastLongitude) * percentageComplete;
						double deltaY = (nextLatitude - lastLatitude) * percentageComplete;

						return LevrumOsrmRoutePosition(lastLongitude + deltaX, lastLatitude + deltaY, elapsedSeconds / routeDuration);
					}
					else
					{
						seconds += duration;
					}
					lastStep = step;
				}
			}
		}
		else if (status == Status::Error)
		{
			return LevrumOsrmRoutePosition(-1.0, -1.0, -1.0);
		}
	}

};

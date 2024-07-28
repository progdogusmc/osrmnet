#pragma once

#include "OsrmFwdDecl.h"
#include "LevrumOsrmRoutePosition.h"

#include <string>
#include <vector>

namespace osrmnet {
	class __declspec(dllexport) LevrumRouter
	{
	public:
		LevrumRouter(const std::string& path);
		~LevrumRouter();

		void SetResolveDistance(double meters);
		void SetLocationChecking(bool checkLocations);
		std::vector<char> Nearest(double lon, double lat) const;
		std::vector<char> GetRoute(double lon1, double lat1, double lon2, double lat2, bool getSteps) const;
		double GetDistance(double lon1, double lat1, double lon2, double lat2) const;
		bool TryGetDistanceDuration(double lon1, double lat1, double lon2, double lat2, double& distance, double& duration) const;
		LevrumOsrmRoutePosition GetLocationAtTime(double lon1, double lat1, double lon2, double lat2, double elapsedMin, double speedMph) const;
	};
}

#pragma once

namespace osrmnet {
	struct LevrumOsrmRoutePosition
	{
	public:
		LevrumOsrmRoutePosition(double _longitude, double _latitude, double _percentComplete)
		{
			Longitude = _longitude;
			Latitude = _latitude;
			PercentComplete = _percentComplete;
		}

		double Longitude;
		double Latitude;
		double PercentComplete;
	};
}
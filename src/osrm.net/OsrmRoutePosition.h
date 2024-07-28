#pragma once

namespace osrmnet {
	public ref class OsrmRoutePosition
	{
	public:
		OsrmRoutePosition(double _longitude, double _latitude, double _percentComplete)
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
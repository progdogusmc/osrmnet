#include "stdafx.h"

#include "LevrumRouter.h"
#include "OsrmRoutePosition.h"

#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include <cstdlib>

#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Security;

using namespace msclr::interop;

namespace osrmnet {
	[SuppressUnmanagedCodeSecurityAttribute]
		public ref class Router {
		private:
			const osrmnet::LevrumRouter* m_router;
		public:
			Router(String^ path) {
				std::string strPath = marshal_as<std::string>(path);

				m_router = new osrmnet::LevrumRouter(marshal_as<std::string>(path));
			}
			~Router() {
				delete m_router;
			}

			String^ GetRoute(double lon1, double lat1, double lon2, double lat2, bool getSteps)
			{
				std::vector<char> jsonContents = m_router->GetRoute(lon1, lat1, lon2, lat2, getSteps);

				if (jsonContents.size() == 0)
					return String::Empty;

				String^ output = gcnew String(&jsonContents[0], 0, jsonContents.size());

				return output;
			}

			String^ Nearest(double lon, double lat) {
				std::vector<char> jsonContents = m_router->Nearest(lon, lat);

				if (jsonContents.size() == 0)
					return String::Empty;

				String^ output = gcnew String(&jsonContents[0], 0, jsonContents.size());

				return output;
			}

			double GetDistance(double lon1, double lat1, double lon2, double lat2)
			{
				return m_router->GetDistance(lon1, lat1, lon2, lat2);
			}

			bool TryGetDistanceDuration(double lon1, double lat1, double lon2, double lat2, [Out] double% distance, [Out] double% duration)
			{
				double dist;
				double dur;

				if (m_router->TryGetDistanceDuration(lon1, lat1, lon2, lat2, dist, dur))
				{
					distance = dist;
					duration = dur;

					return true;
				}

				distance = -1.0;
				duration = -1.0;

				return false;
			}

			OsrmRoutePosition^ GetLocationAtTime(double lon1, double lat1, double lon2, double lat2, double elapsedMin, double speedMph)
			{
				const osrmnet::LevrumOsrmRoutePosition& position = m_router->GetLocationAtTime(lon1, lat1, lon2, lat2, elapsedMin, speedMph);

				return gcnew OsrmRoutePosition(position.Longitude, position.Latitude, position.PercentComplete);
			}
	};
}
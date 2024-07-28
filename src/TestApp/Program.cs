using osrmnet;

Router router = new Router(@"C:\projects\osrm\oregon-latest.osrm");

Console.WriteLine(router.GetDistance(-123.2716328, 44.5589315, -123.082965, 44.6325776));
Console.WriteLine(router.GetRoute(-123.2716328, 44.5589315, -123.082965, 44.6325776, true));

OsrmRoutePosition pos = router.GetLocationAtTime(-123.2716328, 44.5589315, -123.082965, 44.6325776, 2.0, 10.0);

Console.WriteLine("{0:F6},{1:F6} = {2:F3}", pos.Longitude, pos.Latitude, pos.PercentComplete);

Console.ReadLine();
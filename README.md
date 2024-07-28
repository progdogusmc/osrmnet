osrmnet
=======

This project provides necessary .NET wrapper to use OSRM library directly without the web server (https://github.com/Project-OSRM/osrm-backend).

It also includes Levrum, Inc. specific routing functionality for use in Code3 Strategist

Our design goal is to follow as close as possible OSRM project's APIs and data types.  User should be able to use OSRM documentation to use this library.

Levrum Users can contact Kevin Kelly for support at:  progdog@gmail.com

Contribution is always welcome, please email me:  ssuluh@yahoo.com

# What's New
- Dotnet 8 is now the current supported runtime

# Supported OSRM Version
[GitHub Project-OSRM/osrm-backend 5.26](https://github.com/Project-OSRM/osrm-backend/tree/5.26)

# Installation
1. System Requirements:
   - Minimum: Windows 10 1803 or greater (and its Windows Server equivalence)
   - Microsoft Visual C++ Redistributable (https://docs.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
   - Dotnet 8.0 Runtime (https://dotnet.microsoft.com/en-us/download/dotnet/8.0)

1. Pull package osrmnet package (http://www.nuget.org/packages/osrmnet) in your project

# Build Locally
Please read the OSRM Wiki:  https://github.com/Project-OSRM/osrm-backend/wiki/Windows-Compilation.
Ensure the correct MS C/C++ runtime library version installed and valid temp directory specified in your .stxxl.txt before you run osrm-extract and other osrm binaries.

To build locally:

Open Powershell and run .\build.ps1 from the root directory of the project.

# How To Get Precompiled osrm libraries and tools
osrm-backend project does not officially support Windows platform. The windows binaries in that project are not the latest.
1. Clone this project
2. Run this command from powershell:
`.\build.ps1 -Target "GetDependencies"
3. Location of the osrm libraries and app: .\osrmnet\src\osrm.net\libosrm\redist\osrm_5.26.0.zip

# License
Copyright 2024 Levrum, Inc.

Copyright 2016 Surya Suluh and OSRMNET

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


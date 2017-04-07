# DirectX 12 App (Universal Windows)
This folder contains the code generated by Visual Studio 2015 when creating a DirectX 12 Universal Windows App.  This project uses the Universal Windows App Platform with CoreWindow (no XAML).

### Optional Features
This template has been updated to build against the Windows 10 Anniversary Update SDK. In this SDK a new revision of Root Signatures is available for Direct3D 12 apps to use. Root Signature 1.1 allows for apps to declare when descriptors in a descriptor heap won't change or the data descriptors point to won't change.  This allows the option for drivers to make optimizations that might be possible knowing that something (like a descriptor or the memory it points to) is static for some period of time.
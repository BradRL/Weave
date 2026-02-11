# Build Notes #

---

Step by step process on how to update the build and deploy the application.

1. Update C++ files as needed and `Save All`.
2. Build CMake project via `Build` tab or `F6`.
3. _(Optional)_ Debug or Run application in Visual Studio CLI
	- Run ```$ cd ./out/build/x64-debug```
	- Run ```$ ./Weave.exe [command] [args] [flags]```
4. Test via environment variables.
	- Open powershell as Administrator.
	- Run ```cd "C:\Users\mrble\OneDrive - Leeds Beckett University\University Files\Year-3\Production Project\Weave"```
	- Run ```Copy-Item "out\build\x64-debug\Weave.exe" "C:\Program Files\Weave\bin\Weave.exe" -Force```

---


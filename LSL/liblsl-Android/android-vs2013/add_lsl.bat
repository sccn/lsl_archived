REM This command is called by Configuration Properties -> Ant Build -> Pre-Ant Event -> Command Line under the Tegra-Android build.
REM It is used to add lsl libraries to a fat apk.
xcopy /E /Y "%1bin\liblsl-android-arm.so" "%2libs\armeabi-v7a\" 
xcopy /E /Y "%1bin\liblsl-android-aarch64.so" "%2libs\arm64-v8a\" 
xcopy /E /Y "%1bin\liblsl-android-x86.so" "%2libs\x86\"
xcopy /E /Y "%1bin\liblsl-android-x64.so" "%2libs\x86_64\"  
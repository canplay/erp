%ANDROID_HOME%/build-tools/34.0.0/zipalign -v 4 dist/capacitor/android/apk/release/app-release-unsigned.apk dist/capacitor/android/apk/release/app-release-signed.apk
%ANDROID_HOME%/build-tools/34.0.0/apksigner sign --ks ../../../../release.keystore --ks-key-alias canplay dist/capacitor/android/apk/release/app-release-signed.apk

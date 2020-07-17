#! /bin/sh

BASEDIR=$(dirname $0)

cd $BASEDIR/..

if [ -d build-mac ]; then
  sudo rm -f -R build-mac
fi

#---------------------------------------------------------------------------------------------------------
#variables

IPLUG2_ROOT=../..
XCCONFIG=$IPLUG2_ROOT/common-mac.xcconfig
SCRIPTS=$IPLUG2_ROOT/Scripts

DEMO=0
if [ "$1" == "demo" ]; then
  DEMO=1
fi

VERSION=`echo | grep PLUG_VERSION_HEX config.h`
VERSION=${VERSION//\#define PLUG_VERSION_HEX }
VERSION=${VERSION//\'}
MAJOR_VERSION=$(($VERSION & 0xFFFF0000))
MAJOR_VERSION=$(($MAJOR_VERSION >> 16))
MINOR_VERSION=$(($VERSION & 0x0000FF00))
MINOR_VERSION=$(($MINOR_VERSION >> 8))
BUG_FIX=$(($VERSION & 0x000000FF))

FULL_VERSION=$MAJOR_VERSION"."$MINOR_VERSION"."$BUG_FIX

PLUGIN_NAME=`echo | grep BUNDLE_NAME config.h`
PLUGIN_NAME=${PLUGIN_NAME//\#define BUNDLE_NAME }
PLUGIN_NAME=${PLUGIN_NAME//\"}

DMG_NAME=$PLUGIN_NAME-v$FULL_VERSION-mac

if [ $DEMO == 1 ]; then
  DMG_NAME=$DMG_NAME-demo
fi

VST2=`echo | grep VST2_PATH $XCCONFIG`
VST2=$HOME${VST2//\VST2_PATH = \$(HOME)}/$PLUGIN_NAME.vst

VST3=`echo | grep VST3_PATH $XCCONFIG`
VST3=$HOME${VST3//\VST3_PATH = \$(HOME)}/$PLUGIN_NAME.vst3

AU=`echo | grep AU_PATH $XCCONFIG`
AU=$HOME${AU//\AU_PATH = \$(HOME)}/$PLUGIN_NAME.component

APP=`echo | grep APP_PATH $XCCONFIG`
APP=$HOME${APP//\APP_PATH = \$(HOME)}/$PLUGIN_NAME.app

# Dev build folder
AAX=`echo | grep AAX_PATH $XCCONFIG`
AAX=${AAX//\AAX_PATH = }/$PLUGIN_NAME.aaxplugin
AAX_FINAL="/Library/Application Support/Avid/Audio/Plug-Ins/$PLUGIN_NAME.aaxplugin"

PKG="installer/build-mac/$PLUGIN_NAME Installer.pkg"
PKG_US="installer/build-mac/$PLUGIN_NAME Installer.unsigned.pkg"

CERT_ID=`echo | grep CERTIFICATE_ID $XCCONFIG`
CERT_ID=${CERT_ID//\CERTIFICATE_ID = }

echo $VST2
echo $VST3
echo $AU
echo $APP
echo $AAX

if [ $DEMO == 1 ]; then
 echo "making $PLUGIN_NAME version $FULL_VERSION DEMO mac distribution..."
#   cp "resources/img/AboutBox_Demo.png" "resources/img/AboutBox.png"
else
 echo "making $PLUGIN_NAME version $FULL_VERSION mac distribution..."
#   cp "resources/img/AboutBox_Registered.png" "resources/img/AboutBox.png"
fi

sleep 2

echo "touching source to force recompile"
echo ""
touch *.cpp

#---------------------------------------------------------------------------------------------------------
#remove existing tmp folder (for zip, if used instead of pkg)

#if [ -d installer/tmp ]
#then
#  rm -R installer/tmp
#fi

#mkdir installer/tmp

#---------------------------------------------------------------------------------------------------------
#remove existing binaries

echo "remove existing binaries"
echo ""

if [ -d $APP ]; then
  sudo rm -f -R -f $APP
fi

if [ -d $AU ]; then
 sudo rm -f -R $AU
fi

if [ -d $VST2 ]; then
  sudo rm -f -R $VST2
fi

if [ -d $VST3 ]; then
  sudo rm -f -R $VST3
fi

if [ -d "${AAX}" ]; then
  sudo rm -f -R "${AAX}"
fi

if [ -d "${AAX_FINAL}" ]; then
  sudo rm -f -R "${AAX_FINAL}"
fi

#---------------------------------------------------------------------------------------------------------
# build xcode project. Change target to build individual formats

xcodebuild -project ./projects/$PLUGIN_NAME-macOS.xcodeproj -xcconfig ./config/$PLUGIN_NAME-mac.xcconfig DEMO_VERSION=$DEMO -target "All" -UseModernBuildSystem=NO -configuration Release 2> ./build-mac.log

# if [ -s build-mac.log ]; then
#   echo "build failed due to following errors:"
#   echo ""
#   cat build-mac.log
#   exit 1
# else
#   rm build-mac.log
# fi

#---------------------------------------------------------------------------------------------------------
# set bundle icons - http://www.hamsoftengineering.com/codeSharing/SetFileIcon/SetFileIcon.html

echo "setting icons"
echo ""

if [ -d $AU ]; then
  SetFileIcon -image resources/$PLUGIN_NAME.icns -file $AU
fi

if [ -d $VST2 ]; then
  SetFileIcon -image resources/$PLUGIN_NAME.icns -file $VST2
fi

if [ -d $VST3 ]; then
  SetFileIcon -image resources/$PLUGIN_NAME.icns -file $VST3
fi

if [ -d "${AAX}" ]; then
  SetFileIcon -image resources/$PLUGIN_NAME.icns -file "${AAX}"
fi

#---------------------------------------------------------------------------------------------------------
#strip symbols from binaries

echo "stripping binaries"
echo ""

if [ -d $APP ]; then
  strip -x $APP/Contents/MacOS/$PLUGIN_NAME
fi

if [ -d $AU ]; then
  strip -x $AU/Contents/MacOS/$PLUGIN_NAME
fi

if [ -d $VST2 ]; then
  strip -x $VST2/Contents/MacOS/$PLUGIN_NAME
fi

if [ -d $VST3 ]; then
  strip -x $VST3/Contents/MacOS/$PLUGIN_NAME
fi

if [ -d "${AAX}" ]; then
  strip -x "${AAX}/Contents/MacOS/$PLUGIN_NAME"
fi

#---------------------------------------------------------------------------------------------------------
# code sign AAX binary

#echo "copying AAX ${PLUGIN_NAME} from 3PDev to main AAX folder"
#sudo cp -p -R "${AAX}" "${AAX_FINAL}"
#mkdir "${AAX_FINAL}/Contents/Factory Presets/"
#
#echo "code sign AAX binary"
#/Applications/PACEAntiPiracy/Eden/Fusion/Current/bin/wraptool sign --verbose --account XXXX --wcguid XXXX --signid "Developer ID Application: ""${CERT_ID}" --in "${AAX_FINAL}" --out "${AAX_FINAL}"

#---------------------------------------------------------------------------------------------------------


#---------------------------------------------------------------------------------------------------------
# installer
sudo rm -R -f installer/$PLUGIN_NAME-mac.dmg

echo "building installer"
echo ""

./scripts/makeinstaller-mac.sh $FULL_VERSION

# echo "code-sign installer for Gatekeeper on macOS 10.8+"
# echo ""
# mv "${PKG}" "${PKG_US}"
# productsign --sign "Developer ID Installer: ""${CERT_ID}" "${PKG_US}" "${PKG}"
# rm -R -f "${PKG_US}"

#set installer icon
SetFileIcon -image resources/$PLUGIN_NAME.icns -file "${PKG}"

#---------------------------------------------------------------------------------------------------------
# dmg, can use dmgcanvas http://www.araelium.com/dmgcanvas/ to make a nice dmg
echo "building dmg"
echo ""

if [ -d installer/$PLUGIN_NAME.dmgCanvas ]; then
 dmgcanvas installer/$PLUGIN_NAME.dmgCanvas installer/$DMG_NAME.dmg
else
 cp installer/changelog.txt installer/build-mac/
 cp installer/known-issues.txt installer/build-mac/
 cp "manual/$PLUGIN_NAME manual.pdf" installer/build-mac/
 hdiutil create installer/$DMG_NAME.dmg -format UDZO -srcfolder installer/build-mac/ -ov -anyowners -volname $PLUGIN_NAME
fi

# sudo rm -R -f installer/build-mac/

#---------------------------------------------------------------------------------------------------------
# dSYMs
sudo rm -R -f installer/*-dSYMs.zip

echo "packaging dSYMs"
echo ""
zip -r ./installer/$PLUGIN_NAME-v$FULL_VERSION-dSYMs.zip ./build-mac/*.dSYM

#---------------------------------------------------------------------------------------------------------
# zip

# echo "copying binaries..."
# echo ""
# cp -R $AU installer/tmp/$PLUGIN_NAME.component
# cp -R $VST2 installer/tmp/$PLUGIN_NAME.vst
# cp -R $VST3 installer/tmp/$PLUGIN_NAME.vst3
# cp -R $AAX installer/tmp/$PLUGIN_NAME.aaxplugin
# cp -R $APP installer/tmp/$PLUGIN_NAME.app
#
# echo "zipping binaries..."
# echo ""
# ditto -c -k installer/tmp installer/$PLUGIN_NAME-mac.zip
# rm -R installer/tmp

#---------------------------------------------------------------------------------------------------------

#if [ $DEMO == 1 ]
#then
#  git checkout installer/IPlugEffect.iss
#  git checkout installer/IPlugEffect.pkgproj
#  git checkout resources/img/AboutBox.png
#fi

echo "done"

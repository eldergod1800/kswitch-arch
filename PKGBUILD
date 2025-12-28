# Maintainer: https://github.com/Zyren-Lab/KSwitch
pkgname=kswitch-data-transfer
pkgver=1.0.3
pkgrel=1
pkgdesc="Open-source data transfer tool for Linux - Samsung Smart Switch alternative"
arch=('x86_64')
url="https://github.com/Zyren-Lab/KSwitch"
license=('GPL3')
depends=('java-runtime>=17' 'android-tools')
makedepends=('git' 'jdk17-openjdk')
conflicts=('kswitch')
source=("git+https://github.com/Zyren-Lab/KSwitch.git#tag=${pkgver}")
sha256sums=('5f1e486b2cf04be3ef216c91a1206797a3de2b50c640ee74ff357d058ab6f336')

build() {
  cd "$srcdir/KSwitch"
  
  # Set Java home
  export JAVA_HOME=/usr/lib/jvm/java-17-openjdk
  export PATH=$JAVA_HOME/bin:$PATH
  
  # Make gradlew executable
  chmod +x gradlew
  
  # Build with stacktrace for better error messages
  ./gradlew --stacktrace packageUberJarForCurrentOS
}

package() {
  cd "$srcdir/KSwitch"
  
  # Find the built JAR (handle any filename)
  jarfile=$(find build/compose/jars -name "KSwitch-linux-*.jar" -type f | head -n1)
  
  if [ -z "$jarfile" ]; then
    echo "Error: Could not find built JAR file"
    exit 1
  fi
  
  # Install the JAR
  install -Dm644 "$jarfile" "$pkgdir/usr/share/java/kswitch/kswitch.jar"
  
  # Create launcher script (named kswitch-app to avoid conflict)
  install -Dm755 /dev/stdin "$pkgdir/usr/bin/kswitch-app" << 'EOF'
#!/bin/bash
exec java -jar /usr/share/java/kswitch/kswitch.jar "$@"
EOF
  
  # Create desktop entry
  install -Dm644 /dev/stdin "$pkgdir/usr/share/applications/kswitch.desktop" << 'EOF'
[Desktop Entry]
Name=KSwitch
Comment=Backup and restore Android devices
Exec=kswitch-app
Icon=phone
Terminal=false
Type=Application
Categories=Utility;System;
EOF
}

#!/bin/bash

set -e
set -u

function myecho() {
  echo -n " [+] "
  echo -ne '\033[32;1m'
  echo -n $@
  echo -e '\033[0m'
}

OLD_RCD="${RUNSWIFT_CHECKOUT_DIR-}"
unset RUNSWIFT_CHECKOUT_DIR
unset CTC_DIR

# Set up git
cat << USER_CONFIG
If the user info is incorrect, please configure it like:
  git config user.name Jayen
  git config user.email jayen@cse.unsw.edu.au
USER_CONFIG
echo Your user name: $(git config user.name)
echo Your email: $(git config user.email)

# Set up ssh_config

for host in robot1 robot2
do
  if [ ! -f ~/.ssh/config ] || ! grep -q "Host $host" ~/.ssh/config ; then (
      echo "Host $host"
      echo "  Hostname $host.local"
      echo "  HostKeyAlias $host"
      echo "  CheckHostIP no"
      echo "  User nao"
      echo
    ) >> ~/.ssh/config
  fi
done

## TODO: remove lines from bashrc from old runswift stuff, path, etc.

# Set up bash
export RUNSWIFT_CHECKOUT_DIR="${RUNSWIFT_CHECKOUT_DIR-"$(readlink -f "$(dirname $0)"/..)"}"
echo RUNSWIFT_CHECKOUT_DIR is $RUNSWIFT_CHECKOUT_DIR
if ! grep -q "# Robocup stuff" ~/.bashrc ; then (
echo >> ~/.bashrc
echo "# Robocup stuff" >> ~/.bashrc
echo export RUNSWIFT_CHECKOUT_DIR=\"$RUNSWIFT_CHECKOUT_DIR\" >> ~/.bashrc
echo export PATH=\"\$RUNSWIFT_CHECKOUT_DIR/bin:\$PATH\" >> ~/.bashrc
)
fi
if [[ x"$OLD_RCD" != x"$RUNSWIFT_CHECKOUT_DIR" ]]; then
  trap "myecho RUNSWIFT_CHECKOUT_DIR has changed from \'$OLD_RCD\' to \'$RUNSWIFT_CHECKOUT_DIR\'.  please be sure to reload ~/.bashrc before fixing things manually" ERR
fi

# SSH keys
ssh-keygen -l -f ~/.ssh/id_rsa.pub > /dev/null || ssh-keygen
if ! grep -qf ~/.ssh/id_rsa.pub "$RUNSWIFT_CHECKOUT_DIR"/image/home/nao/.ssh/authorized_keys; then
  echo >> "$RUNSWIFT_CHECKOUT_DIR"/image/home/nao/.ssh/authorized_keys
  echo "# $(git config user.name)'s key" >> "$RUNSWIFT_CHECKOUT_DIR"/image/home/nao/.ssh/authorized_keys
  cat ~/.ssh/id_rsa.pub >> "$RUNSWIFT_CHECKOUT_DIR"/image/home/nao/.ssh/authorized_keys
fi

########### Toolchain ##########

# CTC
mkdir -p "$RUNSWIFT_CHECKOUT_DIR"/ctc
cd "$RUNSWIFT_CHECKOUT_DIR"/ctc

if [ ! -f linux32-nao-atom-cross-toolchain-1.14.5.tar.gz ]; then
# You might prefer to visit this Aldebaran URL with a browser, we manually
# work around wget not choosing the correct name with --output-document= here...
# https://community.aldebaran.com/en/resources/download/soft/1393/493-bffc91e7e075f1f8e336c591f8e093b2
wget --continue --output-document=linux32-nao-atom-cross-toolchain-1.14.5.tar.gz https://community.aldebaran.com/en/resources/download/soft/1393/493-bffc91e7e075f1f8e336c591f8e093b2
fi

export CTC_DIR="$RUNSWIFT_CHECKOUT_DIR"/ctc/linux32-nao-atom-cross-toolchain-1.14.5
[[ -d "$CTC_DIR" ]] || ( myecho Extracting cross toolchain, this may take a while... && tar -xvf linux32-nao-atom-cross-toolchain-1.14.5.tar.gz )
if ! grep -q "CTC_DIR" ~/.bashrc ; then
echo export CTC_DIR=\"$CTC_DIR\" >> ~/.bashrc
fi

# Jayen's magic sauce

#[[ -L "$CTC_DIR"/sysroot/usr/include/boost ]] || rm -rf "$CTC_DIR"/sysroot/usr/include/boost
myecho rsync\'ing sysroot/usr, this may take a *long* time...
# Note: rUNSWift actually used v1.14.1 but Aldebaran no longer provides this.
# We hope it's close enough to v1.14.5 that it builds and runs, it did for us
rsync -a --stats --ignore-existing runswift.cse.unsw.edu.au::opennao-1.14.1/ "$CTC_DIR"/sysroot/usr/ --exclude portage

############ Building ###########

myecho Generating Makefiles and doing the initial build
echo

# Build!
for i in release relwithdebinfo; do
  cd "$RUNSWIFT_CHECKOUT_DIR"
  mkdir -p build-$i
  cd build-$i
  myecho $CTC_DIR
  cmake --debug-trycompile .. -DCMAKE_TOOLCHAIN_FILE="$RUNSWIFT_CHECKOUT_DIR"/toolchain-1.14.cmake -DCMAKE_BUILD_TYPE=$i
  make -j`nproc`
done

echo
echo All done! To build, type make -j$(nproc) in the build-release and build-relwithdebinfo directories.
echo

# Finish
echo Please close all shells.  Only new shells will have RUNSWIFT_CHECKOUT_DIR set to $RUNSWIFT_CHECKOUT_DIR
echo 'Alternatively, type . ~/.bashrc in existing shells.'
echo

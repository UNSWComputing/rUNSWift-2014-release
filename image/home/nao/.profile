# ~/.profile: executed by Bourne-compatible login shells.

export PS1='\u@\h [$?] [\w]\$ '

umask 022

LS_OPTIONS='--color=auto'
alias ls='ls $LS_OPTIONS'
alias l='ls $LS_OPTIONS'
alias ll='ls $LS_OPTIONS -l'
alias la='ls $LS_OPTIONS -lA'

alias nao='/etc/init.d/naoqi'
alias naoload='nano /opt/naoqi/modules/lib/autoload.ini'

alias deep_shutdown='harakiri --deep && halt'

if [ x"$SHELL" = x"/bin/sh" ]; then
  cat <<EOF
=====================================================
it appears that you are running sh and not bash
it is recommended that you run the following commands
  opkg update
  opkg install bash bash-completion
  opkg install valgrind libmudflap-dev
  opkg install rsync
  opkg install vim
  opkg install bzip2
  opkg install alsa-dev alsa-oss-dev flite
  sed -i s@/bin/sh@/bin/bash@ /etc/passwd
then log off and on again
=====================================================
EOF
else
  if [ -f ~/.bashrc ]; then
    . ~/.bashrc
  fi
fi

if [ -d /home/nao/bin ]; then
   export PATH="/home/nao/bin:$PATH"
fi

mesg n

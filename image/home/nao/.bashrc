# /etc/bash/bashrc
#
# This file is sourced by all *interactive* bash shells on startup,
# including some apparently interactive shells such as scp and rcp
# that can't tolerate any output.  So make sure this doesn't display
# anything or bad things will happen !


# Test for an interactive shell.  There is no need to set anything
# past this point for scp and rcp, and it's important to refrain from
# outputting anything in those cases.
if [[ $- != *i* ]] ; then
	# Shell is non-interactive.  Be done now!
	return
fi

# Bash won't get SIGWINCH if another process is in the foreground.
# Enable checkwinsize so that bash will check the terminal size when
# it regains control.  #65623
# http://cnswww.cns.cwru.edu/~chet/bash/FAQ (E11)
shopt -s checkwinsize

# Enable history appending instead of overwriting.  #139609
shopt -s histappend

if [ $TERM == xterm-256color ]
then
	export TERM=xterm
fi

# Change the window title of X terminals
case ${TERM} in
	xterm*|rxvt*|Eterm|aterm|kterm|gnome*|interix)
		PROMPT_COMMAND='echo -ne "\033]0;${USER}@${HOSTNAME%%.*}:${PWD/$HOME/~}\007"'
		;;
	screen)
		PROMPT_COMMAND='echo -ne "\033_${USER}@${HOSTNAME%%.*}:${PWD/$HOME/~}\033\\"'
		;;
esac

use_color=false

# Set colorful PS1 only on colorful terminals.
# dircolors --print-database uses its own built-in database
# instead of using /etc/DIR_COLORS.  Try to use the external file
# first to take advantage of user additions.  Use internal bash
# globbing instead of external grep binary.
safe_term=${TERM//[^[:alnum:]]/?}   # sanitize TERM
match_lhs=""
[[ -f ~/.dir_colors   ]] && match_lhs="${match_lhs}$(<~/.dir_colors)"
[[ -f /etc/DIR_COLORS ]] && match_lhs="${match_lhs}$(</etc/DIR_COLORS)"
[[ -z ${match_lhs}    ]] \
	&& type -P dircolors >/dev/null \
	&& match_lhs=$(dircolors --print-database)
[[ $'\n'${match_lhs} == *$'\n'"TERM "${safe_term}* ]] && use_color=true


update_color() {
	if ${use_color} ; then
		_pr_fg_red='\[\033[01;31m\]'
		_pr_fg_green='\[\033[01;32m\]'
		_pr_fg_yellow='\[\033[01;33m\]'
		_pr_fg_blue='\[\033[01;34m\]'
		_pr_fg_pink='\[\033[01;35m\]'
		_pr_fg_cyan='\[\033[01;36m\]'
		_pr_fg_white='\[\033[01;37m\]'
		_pr_reset='\[\033[00m\]'
	else
		unset _pr_fg_red
		unset _pr_fg_green
		unset _pr_fg_yellow
		unset _pr_fg_blue
		unset _pr_fg_pink
		unset _pr_fg_cyan
		unset _pr_fg_white
	fi
}

fancy_prompt() {
	update_color
	# show root@ when we don't have colors
	local _pr_user=$(([[ ${EUID} == 0 ]] && ${use_color}) || echo '\u@')
	local _pr_cwd=$(([[ ${EUID} == 0 ]] && ${use_color}) && echo '\W' || echo '\w')
	if [[ ${EUID} == 0 ]] ; then
		PS1="${_pr_fg_red}root${_pr_fg_yellow}@${_pr_fg_red}\h${_pr_fg_blue} \
${_pr_fg_pink}[\$(eval \"res=\$?\"; [[ \${res} -eq 0 ]] && \
echo -en \"${_pr_reset}\${res}\" || echo -en \"${_pr_fg_red}err \${res}\")${_pr_fg_pink}] \
${_pr_fg_blue}${_pr_cwd} \$${_pr_reset} "
	else
		PS1="${_pr_fg_green}\h${_pr_fg_blue} \
${_pr_fg_pink}[\$(eval \"res=\$?\"; [[ \${res} -eq 0 ]] && \
echo -en \"${_pr_reset}\${res}\" || echo -en \"${_pr_fg_red}err \${res}\")${_pr_fg_pink}] \
${_pr_fg_blue}${_pr_cwd} \$${_pr_reset} "
	fi
}


if ${use_color} ; then
	# Enable colors for ls, etc.  Prefer ~/.dir_colors #64489
	if type -P dircolors >/dev/null ; then
		if [[ -f ~/.dir_colors ]] ; then
			eval $(dircolors -b ~/.dir_colors)
		elif [[ -f /etc/DIR_COLORS ]] ; then
			eval $(dircolors -b /etc/DIR_COLORS)
		fi
	fi
	fancy_prompt

	alias ls='ls --color=auto'
	alias grep='grep --colour=auto'
fi

alias emacs=qemacs
alias e=qemacs
alias vi=vim

alias nao="sudo /etc/init.d/naoqi"

# Try to keep environment pollution down, EPA loves us.
unset use_color safe_term match_lhs

alias netstat='netstat -e -a -t -u -w'

# carls stuff
alias a='ls'

export AL_DIR=/opt/naoqi


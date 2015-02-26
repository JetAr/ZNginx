#!/bin/sh

WWWHOME=/opt/www

if [ "$1" = "" ]; then
	echo 'Content-Type: text/html'
	echo ''
	echo '<TITLE>SMTP EXPN Query</TITLE>'
	echo '<H2 ALIGN=CENTER>SMTP EXPN Query</H2>'
	echo '<I>Enter the email address of the mailgroup or user at'
	echo '"mailhost" that you want information on.<BR>'
	echo '<ISINDEX>'
else
	QUERY="`$WWWHOME/bin/httpdecode \"$*\"`"
	echo 'Content-Type: text/html'
	echo ''
	echo '<TITLE>SMTP EXPN Query Results</TITLE>'
	echo '<H2>SMTP EXPN Query Results</H2>'
	echo '<PRE>'
	(echo 'EXPN '"$QUERY" ; echo QUIT) | telnet mailhost smtp | tail +6 | cut -c 5- | fgrep -v 'closing connection' | $WWWHOME/bin/htmlencode
	echo '</PRE>'
fi

exit 0

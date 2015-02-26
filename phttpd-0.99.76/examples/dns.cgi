#!/bin/sh

WWWHOME=/opt/www

if [ "$QUERY_STRING" = "" ]; then
	echo 'Content-Type: text/html'
	echo ''
	echo '<TITLE>DNS Query</TITLE>'
	echo '<H2 ALIGN=CENTER>DNS Query</H2>'
	echo "<I>Enter the machine's IP address or name.</I>"
	echo '<HR>'
	echo '<FORM ACTION="'"$SCRIPT_NAME"'">'
	echo '<TABLE>'
	echo '<TR>'
	echo '<TD> Host:'
	echo '<TD> <INPUT TYPE="text" NAME="host">'

	echo '<BR><TR>'
	echo '<TD> Server:'
	echo '<TD> <INPUT TYPE="text" NAME="server" VALUE="ns.ifm.liu.se">'

	echo '<BR><TR>'
	echo '<TD> Query type:'
	echo '<TD> <SELECT NAME="type">'
	echo '<OPTION>A'
	echo '<OPTION>MX'
	echo '<OPTION>PTR'
	echo '<OPTION>NS'
	echo '<OPTION>MD'
	echo '<OPTION>MB'
	echo '<OPTION>MG'
	echo '<OPTION>SOA'
	echo '<OPTION>CNAME'
	echo '<OPTION>HINFO'
	echo '<OPTION>MINFO'
	echo '<OPTION>ANY'
	echo '<OPTION>Other:'
	echo '</SELECT>'
	echo '<INPUT TYPE="text" NAME="other_type">'

	echo '</TABLE>'
	echo '<HR>'
	echo '<INPUT TYPE="submit">'
	echo '</FORM>'
else
	HOST="`$WWWHOME/bin/httpdecode \"$*\"`"
	if [ "$PHTTPD_QUERY_type" = "Other:" ]; then
		PHTTPD_QUERY_type="$PHTTPD_QUERY_other_type"
	fi
	echo 'Content-Type: text/html'
	echo ''
	echo '<TITLE>DNS Query Results</TITLE>'
	echo '<H2>DNS Query Results</H2>'
	echo '<PRE>'
	/usr/sbin/nslookup -querytype="$PHTTPD_QUERY_type" "$PHTTPD_QUERY_host" | $WWWHOME/bin/htmlencode
	echo '</PRE>'
fi

exit 0

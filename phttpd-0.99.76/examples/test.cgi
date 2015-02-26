#!/bin/sh

echo 'Content-Type: text/html'
echo ''
echo '<h2>Command line arguments:</h2>'
echo '<pre>'
echo 'Argv[0] = '$0
echo 'Argv[1-*] = '$*
echo '</pre>'
echo '<h2>Environment variables:</h2>'
echo '<pre>'
/usr/ucb/printenv
echo '</pre>'

exit 0

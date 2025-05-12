#!/bin/sh
kill -6 $(ps aux | grep '[./]app' | awk '{print $2}')
sleep 1
kill -1 $(ps aux | grep '[./]app' | awk '{print $2}')
sleep 1
kill -2 $(ps aux | grep '[./]app' | awk '{print $2}')
sleep 1
kill -3 $(ps aux | grep '[./]app' | awk '{print $2}')
sleep 1
kill -15 $(ps aux | grep '[./]app' | awk '{print $2}')

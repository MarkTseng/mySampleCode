#!/bin/bash
firstString="C4P/bringup"
secondString="_"
echo "${firstString/'/'/'_'}"    # prints 'I love Sara and Marry'
firstString=${firstString/'/'/'_'}
echo $firstString


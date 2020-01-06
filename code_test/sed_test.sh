#!/bin/bash
makeconfig_quick_sub_config="0,/^QUICK_SUB_CONFIG.*$/s/^QUICK_SUB_CONFIG.*$/QUICK_SUB_CONFIG\ =\ CONFIG_100_22111/g"
sed -i "$makeconfig_quick_sub_config" test_file



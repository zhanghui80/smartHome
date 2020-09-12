<?php

$data="Maker116114000201";
$right=substr($data,11,1);
$curtain=substr($data,12,1);
$air=substr($data,13,1);
$tmp=substr($data,14,2);
$drink=substr($data,16,1);

echo "拼凑的数据".'Maker116114' .strval($right) .strval( $curtain) .strval( $air) . strval($tmp ). strval($drink)."\n";
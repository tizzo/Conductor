<?php

$plugin = array(
  'title' => t('Test Activity'),
  'description' => t('This activity performs operations in order to test Conductor\'s responses to Activity actions.'),
  'handler' => array(
    'class' => 'ConductorActivityTest',
  ),
);

<?php

$plugin = array(
  'title' => t('Start'),
  'description' => t('This is always the first activity in a workflow, it cannot be removed.'),
  'handler' => array(
    'class' => 'ConductorActivityStart',
  ),
);

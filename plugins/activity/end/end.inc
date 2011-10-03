<?php

$plugin = array(
  'title' => t('End'),
  'description' => t('This is always the last activity in a workflow, it cannot be removed.'),
  'handler' => array(
    'class' => 'ConductorActivityEnd',
  ),
);

<?php

$plugin = array(
  'title' => t('Stop'),
  'description' => t('This is always the last activity in a workflow, it cannot be removed.'),
  'description' => t(''),
  'handler' => array(
    'class' => 'ConductorStop',
  ),  
  'conductor_ui_template' => 'conductor_activity',
  // This should be added to Drupal.Conductor.activities.
  'conductur_ui_js_object' => 'activity',
);

<?php

$plugin = array(
  'title' => t('End'),
  'description' => t('This is always the last activity in a workflow, it cannot be removed.'),
  'handler' => array(
    'class' => 'ConductorEnd',
  ),  
  'conductor_ui_template' => 'conductor_activity',
  // This should be added to Drupal.Conductor.activities.
  'conductur_ui_js_object' => 'activity',
);

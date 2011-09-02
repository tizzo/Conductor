<?php

$plugin = array(
  'title' => t('Start'),
  'description' => t('This is always the first activity in a workflow, it cannot be removed.'),
  'handler' => array(
    'class' => 'ConductorStart',
  ),
  'conductor_ui_template' => 'conductor_activity',
  // This should be added to Drupal.Conductor.activities.
  'conductur_ui_js_object' => 'activity',
);

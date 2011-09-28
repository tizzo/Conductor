<?php

$plugin = array(
  'title' => t('Placeholder for a generic activity'),
  'description' => t(''),
  'handler' => array(
    'class' => 'ConductorActivity',
  ),
  'conductor_ui_template' => 'conductor_activity',
  // This should be a member of Drupal.Conductor.activities.
  'conductur_ui_js_object' => 'activity',
);

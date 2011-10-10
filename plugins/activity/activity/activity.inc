<?php

// TODO: Remove this plugin, activity should be an abstract class
// extended by actual activities, this one doesn't do anything.
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

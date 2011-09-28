<?php

$plugin = array(
  'title' => t('Relational Database'),
  'description' => t('This plugin uses any storage engine for which you have a Drupal 7 database driver, defaulting to the main MySQL database.'),
  'handler' => array(
    'class' => 'ConductorStateStorageDatabase',
  ),  
)

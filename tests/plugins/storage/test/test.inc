<?php
$plugin = array(
  'title' => t('In memory mock storage'),
  'description' => t('This plugin uses an in memory array as the storage engine for unit testing..'),
  'handler' => array(
    'class' => 'ConductorStorageTest',
  ),
);
